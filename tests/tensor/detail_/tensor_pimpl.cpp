#include "../test_tensor.hpp"
#include "chemist/tensor/detail_/pimpl.hpp"

using namespace chemist::tensor;

/* Testing Strategy:
 *
 * We assume that all allocators and shapes work correctly. this means that
 * functions which depend on the shape and allocator state should work correctly
 * as long as those functions properly call and process the results of
 * interacting with allocators/shapes. In practice there are a lot of
 */

TEST_CASE("TensorWrapperPIMPL<Tensor>") {
    using field_type     = field::Tensor;
    using pimpl_type     = detail_::TensorWrapperPIMPL<field_type>;
    using variant_type   = typename pimpl_type::variant_type;
    using ta_tensor_type = std::variant_alternative_t<0, variant_type>;
    using shape_type     = typename pimpl_type::shape_type;
    using extents_type   = typename pimpl_type::extents_type;

    using outer_tile = typename ta_tensor_type::value_type;
    using inner_tile = typename outer_tile::value_type;

    inner_tile v0(TA::Range({2}), {1.0, 2.0});
    inner_tile v1(TA::Range({2}), {3.0, 4.0});
    inner_tile v2(TA::Range({2}), {5.0, 6.0});
    inner_tile v3(TA::Range({2}), {7.0, 8.0});

    using default_alloc = OneBigTile<field_type>;
    using other_alloc   = SingleElementTiles<field_type>;

    auto alloc   = std::make_unique<default_alloc>();
    auto tensors = testing::get_tensors<ta_tensor_type>();

    variant_type vov_data{tensors.at("vector-of-vectors")};
    variant_type mov_data{tensors.at("matrix-of-vectors")};
    variant_type vom_data{tensors.at("vector-of-matrices")};

    pimpl_type vov(vov_data, alloc->clone());
    pimpl_type mov(mov_data, alloc->clone());
    pimpl_type vom(vom_data, alloc->clone());

    auto vov_shape = std::make_unique<shape_type>(extents_type{2});
    auto mov_shape = std::make_unique<shape_type>(extents_type{2, 2});
    auto vom_shape = std::make_unique<shape_type>(extents_type{2});

    pimpl_type vov2(vov_data, vov_shape->clone(), alloc->clone());
    pimpl_type mov2(mov_data, mov_shape->clone(), alloc->clone());
    pimpl_type vom2(vom_data, vom_shape->clone(), alloc->clone());

    SECTION("CTors") {
        SECTION("No Shape") {
            REQUIRE(vov.allocator() == *alloc);
            REQUIRE(vov.shape() == *vov_shape);
            REQUIRE(std::get<0>(vov.variant()) ==
                    tensors.at("vector-of-vectors"));
            REQUIRE(vov.size() == 2);

            REQUIRE(mov.allocator() == *alloc);
            REQUIRE(mov.shape() == *mov_shape);
            REQUIRE(std::get<0>(mov.variant()) ==
                    tensors.at("matrix-of-vectors"));
            REQUIRE(mov.size() == 4);

            REQUIRE(vom.allocator() == *alloc);
            REQUIRE(vom.shape() == *vom_shape);
            REQUIRE(std::get<0>(vom.variant()) ==
                    tensors.at("vector-of-matrices"));
            REQUIRE(vom.size() == 2);

            // Just checking that it's triggered, reallocate looks at more
            // edge-cases
            SECTION("Reallocates if necessary") {
                auto new_alloc = std::make_unique<other_alloc>();
                auto tr        = new_alloc->make_tiled_range(extents_type{2});
                auto& world    = new_alloc->runtime();

                REQUIRE_THROWS_AS(pimpl_type(vov_data, new_alloc->clone()),
                                  std::runtime_error);

                // ta_tensor_type corr{world, tr, {1, 2, 3}
                // REQUIRE(v3.allocator() == *alloc);
                // REQUIRE(std::get<0>(v3.variant()) == corr);
                // REQUIRE(v3.size() == 3);
            }
        }

        SECTION("With shape") {
            REQUIRE(vov2.allocator() == *alloc);
            REQUIRE(vov2.shape() == *vov_shape);
            REQUIRE(std::get<0>(vov2.variant()) ==
                    tensors.at("vector-of-vectors"));
            REQUIRE(vov2.size() == 2);

            REQUIRE(mov2.allocator() == *alloc);
            REQUIRE(mov2.shape() == *mov_shape);
            REQUIRE(std::get<0>(mov2.variant()) ==
                    tensors.at("matrix-of-vectors"));
            REQUIRE(mov2.size() == 4);

            REQUIRE(vom2.allocator() == *alloc);
            REQUIRE(vom2.shape() == *vom_shape);
            REQUIRE(std::get<0>(vom2.variant()) ==
                    tensors.at("vector-of-matrices"));
            REQUIRE(vom2.size() == 2);

            // Just want to check that the following are triggered,
            // reshape/reallocate will check more in depth
            SECTION("Reshapes if necessary") {
                extents_type four{4};
                auto new_shape = std::make_unique<shape_type>(four);
                auto tr        = alloc->make_tiled_range(four);
                ta_tensor_type corr(alloc->runtime(), tr, {v0, v1, v2, v3});

                REQUIRE_THROWS_AS(
                  pimpl_type(mov_data, new_shape->clone(), alloc->clone()),
                  std::runtime_error);

                // REQUIRE(m3.allocator() == *alloc);
                // REQUIRE(m3.shape() == *new_shape);
                // REQUIRE(std::get<0>(m3.variant()) == corr);
                // REQUIRE(m3.size() == 4);
            }

            SECTION("Applies sparsity if needed") {
                using single_tiles    = SingleElementTiles<field_type>;
                using sparse_shape    = SparseShape<field_type>;
                using sparse_map_type = typename sparse_shape::sparse_map_type;
                using index_type      = typename sparse_map_type::key_type;

                auto new_alloc = std::make_unique<single_tiles>();
                extents_type two{2};
                auto tr     = new_alloc->make_tiled_range(two);
                auto& world = new_alloc->runtime();

                inner_tile empty;
                variant_type input{ta_tensor_type(world, tr, {v0, v1})};
                ta_tensor_type corr(world, tr, {v0, empty});

                index_type i0{0}, i1{1};
                sparse_map_type sm{{i0, {i0}}};
                auto new_shape = std::make_unique<sparse_shape>(two, sm);

                pimpl_type m3(input, new_shape->clone(), new_alloc->clone());
                REQUIRE(m3.allocator() == *new_alloc);
                REQUIRE(m3.shape() == *new_shape);
                REQUIRE(std::get<0>(m3.variant()) == corr);
                REQUIRE(m3.size() == 2);
            }

            SECTION("Reallocates if necessary") {
                auto new_alloc = std::make_unique<other_alloc>();
                auto tr        = new_alloc->make_tiled_range(extents_type{2});
                auto& world    = new_alloc->runtime();

                REQUIRE_THROWS_AS(
                  pimpl_type(vov_data, vov_shape->clone(), new_alloc->clone()),
                  std::runtime_error);

                // ta_tensor_type corr{world, tr, {1, 2, 3}
                // REQUIRE(v3.allocator() == *alloc);
                // REQUIRE(std::get<0>(v3.variant()) == corr);
                // REQUIRE(v3.size() == 3);
            }
        }

        SECTION("clone") {
            auto v_copy = vov.clone();
            REQUIRE(*v_copy == vov);
            // Make sure we didn't just alias
            REQUIRE(&v_copy->allocator() != &vov.allocator());
            REQUIRE(&v_copy->shape() != &vov.shape());

            REQUIRE(*(mov.clone()) == mov);
            REQUIRE(*(vom.clone()) == vom);
        }
    }

    SECTION("allocator") {
        REQUIRE(vov.allocator() == *alloc);
        REQUIRE(mov.allocator() == *alloc);
        REQUIRE(vom.allocator() == *alloc);
    }

    SECTION("shape") {
        REQUIRE(vov.shape() == *vov_shape);
        REQUIRE(mov.shape() == *mov_shape);
        REQUIRE(vom.shape() == *vom_shape);
    }

    SECTION("extents") {
        REQUIRE(vov.extents() == extents_type{2});
        REQUIRE(mov.extents() == extents_type{2, 2});
        REQUIRE(vom.extents() == extents_type{2});
    }

    SECTION("make_annotation") {
        REQUIRE(vov.make_annotation("i") == "i0;i1");
        REQUIRE(mov.make_annotation("j") == "j0,j1;j2");
        REQUIRE(vom.make_annotation("jk") == "jk0;jk1,jk2");
    }

    SECTION("rank") {
        REQUIRE(vov.rank() == 2);
        REQUIRE(mov.rank() == 3);
        REQUIRE(vom.rank() == 3);
    }

    //     SECTION("reallocate") {
    //         auto new_alloc = std::make_unique<other_alloc>();

    //         SECTION("vector") {
    //             auto tr =
    //             new_alloc->make_tiled_range(extents_type{3});
    //             ta_tensor_type corr(new_alloc->runtime(), tr,
    //             {1, 2, 3});

    //             pimpl_type v3(vector, new_alloc->clone());
    //             REQUIRE(v3.allocator() == *alloc);
    //             REQUIRE(std::get<0>(v3.variant()) == corr);
    //         }

    //         SECTION("matrix") {
    //             auto tr =
    //             new_alloc->make_tiled_range(extents_type{2,
    //             2}); ta_tensor_type
    //             corr(new_alloc->runtime(), tr,
    //             {{1, 2}, {3, 4}});

    //             pimpl_type m3(matrix, new_alloc->clone());
    //             REQUIRE(m3.allocator() == *alloc);
    //             REQUIRE(std::get<0>(m3.variant()) == corr);
    //         }

    //         SECTION("tensor") {
    //             auto tr =
    //             new_alloc->make_tiled_range(extents_type{2,
    //             2, 2}); ta_tensor_type
    //             corr(new_alloc->runtime(), tr,
    //                                 {{{1, 2}, {3, 4}}, {{5,
    //                                 6}, {7, 8}}});

    //             pimpl_type t3(tensor, new_alloc->clone());
    //             REQUIRE(t3.allocator() == *alloc);
    //             REQUIRE(std::get<0>(t3.variant()) == corr);
    //         }
    //     }

    //     SECTION("reshape") {
    //         SECTION("Literal reshape") {
    //             SECTION("vector") {
    //                 extents_type four{3, 1};
    //                 auto new_shape =
    //                 std::make_unique<shape_type>(four); auto
    //                 tr = alloc->make_tiled_range(four);
    //                 ta_tensor_type corr(alloc->runtime(), tr,
    //                 {{1}, {2}, {3}});

    //                 pimpl_type m3(vector, new_shape->clone(),
    //                 alloc->clone()); REQUIRE(m3.allocator()
    //                 == *alloc); REQUIRE(m3.shape() ==
    //                 *new_shape);
    //                 REQUIRE(std::get<0>(m3.variant()) ==
    //                 corr); REQUIRE(m3.size() == 3);
    //             }

    //             SECTION("matrix") {
    //                 extents_type four{4};
    //                 auto new_shape =
    //                 std::make_unique<shape_type>(four); auto
    //                 tr = alloc->make_tiled_range(four);
    //                 ta_tensor_type corr(alloc->runtime(), tr,
    //                 {1, 2, 3, 4});

    //                 pimpl_type m3(matrix, new_shape->clone(),
    //                 alloc->clone()); REQUIRE(m3.allocator()
    //                 == *alloc); REQUIRE(m3.shape() ==
    //                 *new_shape);
    //                 REQUIRE(std::get<0>(m3.variant()) ==
    //                 corr); REQUIRE(m3.size() == 4);
    //             }

    //             SECTION("tensor") {
    //                 extents_type four{8};
    //                 auto new_shape =
    //                 std::make_unique<shape_type>(four); auto
    //                 tr = alloc->make_tiled_range(four);
    //                 ta_tensor_type corr(alloc->runtime(), tr,
    //                                     {1, 2, 3, 4, 5, 6, 7,
    //                                     8});

    //                 pimpl_type m3(tensor, new_shape->clone(),
    //                 alloc->clone()); REQUIRE(m3.allocator()
    //                 == *alloc); REQUIRE(m3.shape() ==
    //                 *new_shape);
    //                 REQUIRE(std::get<0>(m3.variant()) ==
    //                 corr); REQUIRE(m3.size() == 8);
    //             }
    //         }

    //         SECTION("Apply sparsity") {
    //             using single_tiles    =
    //             SingleElementTiles<field_type>; using
    //             sparse_shape = SparseShape<field_type>; using
    //             sparse_map_type = typename
    //             sparse_shape::sparse_map_type; using
    //             index_type      = typename
    //             sparse_map_type::key_type;

    //             auto new_alloc =
    //             std::make_unique<single_tiles>(); index_type
    //             i0{0}, i00{0, 0}, i1{1}, i11{1, 1}; auto&
    //             world = new_alloc->runtime();

    //             // Can't apply to vector (need an independent
    //             and a dependent index)

    //             SECTION("matrix") {
    //                 extents_type two{2, 2};
    //                 auto tr =
    //                 new_alloc->make_tiled_range(two);

    //                 variant_type input{ta_tensor_type(world,
    //                 tr, {{1, 2}, {3, 4}})}; ta_tensor_type
    //                 corr(world, tr,
    //                 {{1, 0}, {3, 0}});

    //                 sparse_map_type sm{{i0, {i0}}, {i1,
    //                 {i0}}}; auto new_shape =
    //                 std::make_unique<sparse_shape>(two, sm);

    //                 pimpl_type m3(input, new_shape->clone(),
    //                 new_alloc->clone());
    //                 REQUIRE(m3.allocator() == *new_alloc);
    //                 REQUIRE(m3.shape() == *new_shape);
    //                 REQUIRE(std::get<0>(m3.variant()) ==
    //                 corr); REQUIRE(m3.size() == 4);
    //             }

    //             SECTION("tensor") {
    //                 extents_type two{2, 2, 2};
    //                 auto tr =
    //                 new_alloc->make_tiled_range(two);
    //                 variant_type input{ta_tensor_type(
    //                   world, tr, {{{1, 2}, {3, 4}}, {{5, 6},
    //                   {7, 8}}})};

    //                 SECTION("Rank 1 ind, rank 2 dependent") {
    //                     ta_tensor_type corr(world, tr,
    //                                         {{{1, 0}, {0,
    //                                         0}}, {{5, 0}, {0,
    //                                         0}}});
    //                     sparse_map_type sm{{i0, {i00}}, {i1,
    //                     {i00}}}; auto new_shape =
    //                     std::make_unique<sparse_shape>(two,
    //                     sm);

    //                     pimpl_type m3(input,
    //                     new_shape->clone(),
    //                                   new_alloc->clone());
    //                     REQUIRE(m3.allocator() ==
    //                     *new_alloc); REQUIRE(m3.shape() ==
    //                     *new_shape);
    //                     REQUIRE(std::get<0>(m3.variant()) ==
    //                     corr); REQUIRE(m3.size() == 8);
    //                 }

    //                 SECTION("Rank 2 ind, rank 1 dependent") {
    //                     ta_tensor_type corr(world, tr,
    //                                         {{{1, 0}, {0,
    //                                         0}}, {{0, 0}, {0,
    //                                         8}}});
    //                     sparse_map_type sm{{i00, {i0}}, {i11,
    //                     {i1}}}; auto new_shape =
    //                     std::make_unique<sparse_shape>(two,
    //                     sm);

    //                     pimpl_type m3(input,
    //                     new_shape->clone(),
    //                                   new_alloc->clone());
    //                     REQUIRE(m3.allocator() ==
    //                     *new_alloc); REQUIRE(m3.shape() ==
    //                     *new_shape);
    //                     REQUIRE(std::get<0>(m3.variant()) ==
    //                     corr); REQUIRE(m3.size() == 8);
    //                 }
    //             }
    //         }
    //     }

    SECTION("size") {
        REQUIRE(vov.size() == 2);
        REQUIRE(mov.size() == 4);
        REQUIRE(vom.size() == 2);
    }

    //     SECTION("slice()") {
    //         auto& world = alloc->runtime();
    //         SECTION("Vector") {
    //             auto tr =
    //             alloc->make_tiled_range(extents_type{2});
    //             ta_tensor_type corr_data{world, tr,
    //             {1.0, 2.0}}; pimpl_type corr(corr_data,
    //             alloc->clone()); auto slice = v.slice({0ul},
    //             {2ul}, alloc->clone()); REQUIRE(*slice ==
    //             corr);
    //         }
    //         SECTION("Matrix") {
    //             auto tr =
    //             alloc->make_tiled_range(extents_type{1, 1});
    //             ta_tensor_type corr_data{world, tr, {{2.0}}};
    //             pimpl_type corr(corr_data, alloc->clone());
    //             auto slice = m.slice({0ul, 1ul}, {1ul, 2ul},
    //             alloc->clone()); REQUIRE(*slice == corr);
    //         }
    //         SECTION("Tensor") {
    //             auto tr =
    //             alloc->make_tiled_range(extents_type{2, 2,
    //             1}); ta_tensor_type corr_data{
    //               world, tr, {{{2.0}, {4.0}}, {{6.0},
    //               {8.0}}}};
    //             pimpl_type corr(corr_data, alloc->clone());
    //             auto slice =
    //               t.slice({0ul, 0ul, 1ul}, {2ul, 2ul, 2ul},
    //               alloc->clone());
    //             REQUIRE(*slice == corr);
    //         }
    //         SECTION("Different allocator") {
    //             auto p  =
    //             std::make_unique<other_alloc>(world); auto tr
    //             = p->make_tiled_range(extents_type{2});
    //             ta_tensor_type corr_data{world, tr,
    //             {1.0, 2.0}}; pimpl_type corr(corr_data,
    //             p->clone()); auto slice = v.slice({0ul},
    //             {2ul}, std::move(p)); REQUIRE(*slice ==
    //             corr);
    //         }
    //     }

    //     SECTION("print") {
    //         std::stringstream ss;

    //         SECTION("vector") {
    //             auto pss = &(v.print(ss));

    //             // Returns ss for chaining
    //             REQUIRE(pss == &ss);

    //             std::string corr = "0: [ [0], [3] ) { 1 2 3
    //             }\n"; REQUIRE(corr == ss.str());
    //         }

    //         SECTION("matrix") {
    //             auto pss = &(m.print(ss));

    //             // Returns ss for chaining
    //             REQUIRE(pss == &ss);

    //             std::string corr = "0: [ [0,0], [2,2] ) { 1 2
    //             3 4
    //             }\n"; REQUIRE(corr == ss.str());
    //         }

    //         SECTION("tensor") {
    //             auto pss = &(t.print(ss));

    //             // Returns ss for chaining
    //             REQUIRE(pss == &ss);

    //             std::string corr = "0: [ [0,0,0], [2,2,2] ) {
    //             1 2 3 4 5 6 7 8
    //             }\n"; REQUIRE(corr == ss.str());
    //         }
    //     }

    //     SECTION("hash") {
    //         auto lhs = pluginplay::hash_objects(m2);

    //         SECTION("Same") {
    //             pimpl_type rhs(matrix, m_shape->clone(),
    //             alloc->clone()); REQUIRE(lhs ==
    //             pluginplay::hash_objects(rhs));
    //         }

    //         SECTION("Different values") {
    //             auto tr =
    //             alloc->make_tiled_range(extents_type{2, 2});
    //             ta_tensor_type rhs_data(alloc->runtime(), tr,
    //                                     {{2.0, 3.0},
    //                                     {4.0, 5.0}});
    //             pimpl_type rhs(rhs_data, m_shape->clone(),
    //             alloc->clone()); REQUIRE(lhs !=
    //             pluginplay::hash_objects(rhs));
    //         }

    //         SECTION("Different shape") {
    //             using sparse_shape    =
    //             SparseShape<field_type>; using
    //             sparse_map_type = typename
    //             sparse_shape::sparse_map_type; using
    //             index_type = typename
    //             sparse_map_type::key_type;

    //             index_type i0{0}, i1{1};
    //             sparse_map_type sm{{i0, {i0, i1}}, {i1, {i0,
    //             i1}}}; auto new_shape =
    //               std::make_unique<sparse_shape>(extents_type{2,
    //               2}, sm);

    //             pimpl_type rhs(matrix, new_shape->clone(),
    //             alloc->clone()); REQUIRE(lhs !=
    //             pluginplay::hash_objects(rhs));
    //         }
    //     }

    //     SECTION("operator==") {
    //         SECTION("Same") {
    //             pimpl_type rhs(matrix, m_shape->clone(),
    //             alloc->clone()); REQUIRE(m2 == rhs);
    //         }

    //         SECTION("Different values") {
    //             auto tr =
    //             alloc->make_tiled_range(extents_type{2, 2});
    //             ta_tensor_type rhs_data(alloc->runtime(), tr,
    //                                     {{2.0, 3.0},
    //                                     {4.0, 5.0}});
    //             pimpl_type rhs(rhs_data, m_shape->clone(),
    //             alloc->clone()); REQUIRE_FALSE(m2 == rhs);
    //         }

    //         SECTION("Different shape") {
    //             using sparse_shape    =
    //             SparseShape<field_type>; using
    //             sparse_map_type = typename
    //             sparse_shape::sparse_map_type; using
    //             index_type = typename
    //             sparse_map_type::key_type;

    //             index_type i0{0}, i1{1};
    //             sparse_map_type sm{{i0, {i0, i1}}, {i1, {i0,
    //             i1}}}; auto new_shape =
    //               std::make_unique<sparse_shape>(extents_type{2,
    //               2}, sm);

    //             pimpl_type rhs(matrix, new_shape->clone(),
    //             alloc->clone()); REQUIRE_FALSE(m2 == rhs);
    //         }
    //     }
}
