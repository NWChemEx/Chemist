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

TEST_CASE("TensorWrapperPIMPL") {
    using field_type     = field::Scalar;
    using pimpl_type     = detail_::TensorWrapperPIMPL<field_type>;
    using variant_type   = typename pimpl_type::variant_type;
    using ta_tensor_type = std::variant_alternative_t<0, variant_type>;
    using shape_type     = typename pimpl_type::shape_type;
    using extents_type   = typename pimpl_type::extents_type;

    using default_alloc = OneBigTile<field_type>;
    using other_alloc   = SingleElementTiles<field_type>;

    auto alloc   = std::make_unique<default_alloc>();
    auto tensors = testing::get_tensors<ta_tensor_type>();

    variant_type vector{tensors.at("vector")};
    variant_type matrix{tensors.at("matrix")};
    variant_type tensor{tensors.at("tensor")};

    pimpl_type v(vector, alloc->clone());
    pimpl_type m(matrix, alloc->clone());
    pimpl_type t(tensor, alloc->clone());

    auto v_shape = std::make_unique<shape_type>(extents_type{3});
    auto m_shape = std::make_unique<shape_type>(extents_type{2, 2});
    auto t_shape = std::make_unique<shape_type>(extents_type{2, 2, 2});

    pimpl_type v2(vector, v_shape->clone(), alloc->clone());
    pimpl_type m2(matrix, m_shape->clone(), alloc->clone());
    pimpl_type t2(tensor, t_shape->clone(), alloc->clone());

    SECTION("CTors") {
        SECTION("No Shape") {
            REQUIRE(v.allocator() == *alloc);
            REQUIRE(v.shape() == *v_shape);
            REQUIRE(std::get<0>(v.variant()) == tensors.at("vector"));
            REQUIRE(v.size() == 3);

            REQUIRE(m.allocator() == *alloc);
            REQUIRE(m.shape() == *m_shape);
            REQUIRE(std::get<0>(m.variant()) == tensors.at("matrix"));
            REQUIRE(m.size() == 4);

            REQUIRE(t.allocator() == *alloc);
            REQUIRE(t.shape() == *t_shape);
            REQUIRE(std::get<0>(t.variant()) == tensors.at("tensor"));
            REQUIRE(t.size() == 8);

            // Just checking that it's triggered, reallocate looks at more
            // edge-cases
            SECTION("Reallocates if necessary") {
                auto new_alloc = std::make_unique<other_alloc>();
                auto tr        = new_alloc->make_tiled_range(extents_type{3});
                ta_tensor_type corr(new_alloc->runtime(), tr, {1, 2, 3});

                pimpl_type v3(vector, new_alloc->clone());
                REQUIRE(v3.allocator() == *alloc);
                REQUIRE(std::get<0>(v3.variant()) == corr);
                REQUIRE(v3.size() == 3);
            }
        }

        SECTION("With shape") {
            REQUIRE(v2.allocator() == *alloc);
            REQUIRE(v2.shape() == *v_shape);
            REQUIRE(std::get<0>(v2.variant()) == tensors.at("vector"));
            REQUIRE(v2.size() == 3);

            REQUIRE(m2.allocator() == *alloc);
            REQUIRE(m2.shape() == *m_shape);
            REQUIRE(std::get<0>(m2.variant()) == tensors.at("matrix"));
            REQUIRE(m2.size() == 4);

            REQUIRE(t2.allocator() == *alloc);
            REQUIRE(t2.shape() == *t_shape);
            REQUIRE(std::get<0>(t2.variant()) == tensors.at("tensor"));
            REQUIRE(t2.size() == 8);

            // Just want to check that the following are triggered,
            // reshape/reallocate will check more in depth
            SECTION("Reshapes if necessary") {
                extents_type four{4};
                auto new_shape = std::make_unique<shape_type>(four);
                auto tr        = alloc->make_tiled_range(four);
                ta_tensor_type corr(alloc->runtime(), tr, {1, 2, 3, 4});

                pimpl_type m3(matrix, new_shape->clone(), alloc->clone());
                REQUIRE(m3.allocator() == *alloc);
                REQUIRE(m3.shape() == *new_shape);
                REQUIRE(std::get<0>(m3.variant()) == corr);
                REQUIRE(m3.size() == 4);
            }

            SECTION("Applies sparsity if needed") {
                using single_tiles    = SingleElementTiles<field_type>;
                using sparse_shape    = SparseShape<field_type>;
                using sparse_map_type = typename sparse_shape::sparse_map_type;
                using index_type      = typename sparse_map_type::key_type;

                auto new_alloc = std::make_unique<single_tiles>();
                extents_type two{2, 2};
                auto tr     = new_alloc->make_tiled_range(two);
                auto& world = new_alloc->runtime();
                variant_type input{ta_tensor_type(world, tr, {{1, 2}, {3, 4}})};
                ta_tensor_type corr(world, tr, {{1, 0}, {3, 0}});

                index_type i0{0}, i1{1};
                sparse_map_type sm{{i0, {i0}}, {i1, {i0}}};
                auto new_shape = std::make_unique<sparse_shape>(two, sm);

                pimpl_type m3(input, new_shape->clone(), new_alloc->clone());
                REQUIRE(m3.allocator() == *new_alloc);
                REQUIRE(m3.shape() == *new_shape);
                REQUIRE(std::get<0>(m3.variant()) == corr);
                REQUIRE(m3.size() == 4);
            }

            SECTION("Reallocates if necessary") {
                auto new_alloc = std::make_unique<other_alloc>();
                auto tr        = new_alloc->make_tiled_range(extents_type{3});
                ta_tensor_type corr(new_alloc->runtime(), tr, {1, 2, 3});

                pimpl_type v3(vector, v_shape->clone(), new_alloc->clone());
                REQUIRE(v3.allocator() == *alloc);
                REQUIRE(v3.shape() == *v_shape);
                REQUIRE(std::get<0>(v3.variant()) == corr);
                REQUIRE(v3.size() == 3);
            }
        }

        SECTION("clone") {
            auto v_copy = v.clone();
            REQUIRE(*v_copy == v);
            // Make sure we didn't just alias
            REQUIRE(&v_copy->allocator() != &v.allocator());
            REQUIRE(&v_copy->shape() != &v.shape());

            REQUIRE(*(m.clone()) == m);
            REQUIRE(*(t.clone()) == t);
        }
    }

    SECTION("allocator") {
        REQUIRE(v.allocator() == *alloc);
        REQUIRE(m.allocator() == *alloc);
        REQUIRE(t.allocator() == *alloc);
    }

    SECTION("shape") {
        REQUIRE(v.shape() == *v_shape);
        REQUIRE(m.shape() == *m_shape);
        REQUIRE(t.shape() == *t_shape);
    }

    SECTION("extents") {
        REQUIRE(v.extents() == extents_type{3});
        REQUIRE(m.extents() == extents_type{2, 2});
        REQUIRE(t.extents() == extents_type{2, 2, 2});
    }

    SECTION("make_annotation") {
        REQUIRE(v.make_annotation("i") == "i0");
        REQUIRE(m.make_annotation("j") == "j0,j1");
        REQUIRE(t.make_annotation("jk") == "jk0,jk1,jk2");
    }

    SECTION("rank") {
        REQUIRE(v.rank() == 1);
        REQUIRE(m.rank() == 2);
        REQUIRE(t.rank() == 3);
    }

    SECTION("reallocate") {
        auto new_alloc = std::make_unique<other_alloc>();

        SECTION("vector") {
            auto tr = new_alloc->make_tiled_range(extents_type{3});
            ta_tensor_type corr(new_alloc->runtime(), tr, {1, 2, 3});

            pimpl_type v3(vector, new_alloc->clone());
            REQUIRE(v3.allocator() == *alloc);
            REQUIRE(std::get<0>(v3.variant()) == corr);
        }

        SECTION("matrix") {
            auto tr = new_alloc->make_tiled_range(extents_type{2, 2});
            ta_tensor_type corr(new_alloc->runtime(), tr, {{1, 2}, {3, 4}});

            pimpl_type m3(matrix, new_alloc->clone());
            REQUIRE(m3.allocator() == *alloc);
            REQUIRE(std::get<0>(m3.variant()) == corr);
        }

        SECTION("tensor") {
            auto tr = new_alloc->make_tiled_range(extents_type{2, 2, 2});
            ta_tensor_type corr(new_alloc->runtime(), tr,
                                {{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}});

            pimpl_type t3(tensor, new_alloc->clone());
            REQUIRE(t3.allocator() == *alloc);
            REQUIRE(std::get<0>(t3.variant()) == corr);
        }
    }

    SECTION("reshape") {
        SECTION("Literal reshape") {
            SECTION("vector") {
                extents_type four{3, 1};
                auto new_shape = std::make_unique<shape_type>(four);
                auto tr        = alloc->make_tiled_range(four);
                ta_tensor_type corr(alloc->runtime(), tr, {{1}, {2}, {3}});

                pimpl_type m3(vector, new_shape->clone(), alloc->clone());
                REQUIRE(m3.allocator() == *alloc);
                REQUIRE(m3.shape() == *new_shape);
                REQUIRE(std::get<0>(m3.variant()) == corr);
                REQUIRE(m3.size() == 3);
            }

            SECTION("matrix") {
                extents_type four{4};
                auto new_shape = std::make_unique<shape_type>(four);
                auto tr        = alloc->make_tiled_range(four);
                ta_tensor_type corr(alloc->runtime(), tr, {1, 2, 3, 4});

                pimpl_type m3(matrix, new_shape->clone(), alloc->clone());
                REQUIRE(m3.allocator() == *alloc);
                REQUIRE(m3.shape() == *new_shape);
                REQUIRE(std::get<0>(m3.variant()) == corr);
                REQUIRE(m3.size() == 4);
            }

            SECTION("tensor") {
                extents_type four{8};
                auto new_shape = std::make_unique<shape_type>(four);
                auto tr        = alloc->make_tiled_range(four);
                ta_tensor_type corr(alloc->runtime(), tr,
                                    {1, 2, 3, 4, 5, 6, 7, 8});

                pimpl_type m3(tensor, new_shape->clone(), alloc->clone());
                REQUIRE(m3.allocator() == *alloc);
                REQUIRE(m3.shape() == *new_shape);
                REQUIRE(std::get<0>(m3.variant()) == corr);
                REQUIRE(m3.size() == 8);
            }
        }

        SECTION("Apply sparsity") {
            using single_tiles    = SingleElementTiles<field_type>;
            using sparse_shape    = SparseShape<field_type>;
            using sparse_map_type = typename sparse_shape::sparse_map_type;
            using index_type      = typename sparse_map_type::key_type;

            auto new_alloc = std::make_unique<single_tiles>();
            index_type i0{0}, i00{0, 0}, i1{1}, i11{1, 1};
            auto& world = new_alloc->runtime();

            // Can't apply to vector (need an independent and a dependent index)

            SECTION("matrix") {
                extents_type two{2, 2};
                auto tr = new_alloc->make_tiled_range(two);

                variant_type input{ta_tensor_type(world, tr, {{1, 2}, {3, 4}})};
                ta_tensor_type corr(world, tr, {{1, 0}, {3, 0}});

                sparse_map_type sm{{i0, {i0}}, {i1, {i0}}};
                auto new_shape = std::make_unique<sparse_shape>(two, sm);

                pimpl_type m3(input, new_shape->clone(), new_alloc->clone());
                REQUIRE(m3.allocator() == *new_alloc);
                REQUIRE(m3.shape() == *new_shape);
                REQUIRE(std::get<0>(m3.variant()) == corr);
                REQUIRE(m3.size() == 4);
            }

            SECTION("tensor") {
                extents_type two{2, 2, 2};
                auto tr = new_alloc->make_tiled_range(two);
                variant_type input{ta_tensor_type(
                  world, tr, {{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}})};

                SECTION("Rank 1 ind, rank 2 dependent") {
                    ta_tensor_type corr(world, tr,
                                        {{{1, 0}, {0, 0}}, {{5, 0}, {0, 0}}});
                    sparse_map_type sm{{i0, {i00}}, {i1, {i00}}};
                    auto new_shape = std::make_unique<sparse_shape>(two, sm);

                    pimpl_type m3(input, new_shape->clone(),
                                  new_alloc->clone());
                    REQUIRE(m3.allocator() == *new_alloc);
                    REQUIRE(m3.shape() == *new_shape);
                    REQUIRE(std::get<0>(m3.variant()) == corr);
                    REQUIRE(m3.size() == 8);
                }

                SECTION("Rank 2 ind, rank 1 dependent") {
                    ta_tensor_type corr(world, tr,
                                        {{{1, 0}, {0, 0}}, {{0, 0}, {7, 0}}});
                    sparse_map_type sm{{i00, {i0}}, {i11, {i1}}};
                    auto new_shape = std::make_unique<sparse_shape>(two, sm);

                    pimpl_type m3(input, new_shape->clone(),
                                  new_alloc->clone());
                    REQUIRE(m3.allocator() == *new_alloc);
                    REQUIRE(m3.shape() == *new_shape);
                    REQUIRE(std::get<0>(m3.variant()) == corr);
                    REQUIRE(m3.size() == 8);
                }
            }
        }
    }

    SECTION("size") {
        REQUIRE(v.size() == 3);
        REQUIRE(m.size() == 4);
        REQUIRE(t.size() == 8);
    }

    SECTION("print") {
        std::stringstream ss;

        SECTION("vector") {
            auto pss = &(v.print(ss));

            // Returns ss for chaining
            REQUIRE(pss == &ss);

            std::string corr = "0: [ [0], [3] ) { 1 2 3 }\n";
            REQUIRE(corr == ss.str());
        }

        SECTION("matrix") {
            auto pss = &(m.print(ss));

            // Returns ss for chaining
            REQUIRE(pss == &ss);

            std::string corr = "0: [ [0,0], [2,2] ) { 1 2 3 4 }\n";
            REQUIRE(corr == ss.str());
        }

        SECTION("tensor") {
            auto pss = &(t.print(ss));

            // Returns ss for chaining
            REQUIRE(pss == &ss);

            std::string corr = "0: [ [0,0,0], [2,2,2] ) { 1 2 3 4 5 6 7 8 }\n";
            REQUIRE(corr == ss.str());
        }
    }
}
