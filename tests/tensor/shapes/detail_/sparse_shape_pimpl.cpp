#include "chemist/tensor/allocators/allocators.hpp"
#include "chemist/tensor/shapes/detail_/sparse_shape_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace chemist::tensor;

/* Testing Strategy:
 *
 * The SparseShapePIMPL class primarily wraps the process of creating a
 * TA::SparseShape instance. The resulting shape depends on:
 *
 * - Whether the field is scalars or tensors,
 * - the rank of the shape,
 * - the tiling of the tensor,
 * - and idx2mode
 *
 * The following two test cases respectively test scalar and tensor fields.
 * Within each test case we consider:
 *
 * - rank 2 and rank 3 shapes,
 * - one big tile, and single element tiles
 * - having idx2mode permutate or not
 *
 * The behavior is tested by creating the object and calling shape()
 */

TEST_CASE("SparseShapePIMPL<field::Scalar>") {
    using field_type    = field::Scalar;
    using pimpl_type    = detail_::SparseShapePIMPL<field_type>;
    using extents_type  = typename pimpl_type::extents_type;
    using sm_type       = typename pimpl_type::sparse_map_type;
    using el_index      = typename sm_type::key_type;
    using idx2mode_type = typename pimpl_type::idx2mode_type;
    using ta_shape_type = typename pimpl_type::ta_shape_type;

    // This is the non-zero value which gets put in the TA shape
    auto fmax = std::numeric_limits<float>::max();

    el_index i0{0}, i1{1}, i2{2}, i3{3}, i4{4};
    el_index i00{0, 0}, i01{0, 1}, i10{1, 0}, i11{1, 1};

    SECTION("CTors") {
        sm_type sm{{i00, {i4}}};
        extents_type extents{2, 3, 6};
        idx2mode_type i2m{0, 1, 2};

        SECTION("Value") {
            // Extents rank doesn't match SM
            extents_type bad_extents{2};
            REQUIRE_THROWS_AS(pimpl_type(bad_extents, sm, i2m),
                              std::runtime_error);

            // i2m rank doesn't match SM (and thus extents)
            idx2mode_type bad_i2m{0};
            REQUIRE_THROWS_AS(pimpl_type(extents, sm, bad_i2m),
                              std::runtime_error);

            // rank in i2m is out of bounds
            idx2mode_type bad_i2m2{0, 4, 1};
            REQUIRE_THROWS_AS(pimpl_type(extents, sm, bad_i2m2),
                              std::out_of_range);
        }

        SECTION("copy") {
            pimpl_type p0(extents, sm, i2m);
            pimpl_type p1(p0);

            REQUIRE(p0 == p1);
        }
    }

    SECTION("shape()") {
        SECTION("Rank 2 Shapes") {
            sm_type sm{{i0, {i1, i3}}, {i1, {i0, i2, i4}}, {i2, {i0, i4}}};

            SECTION("Single Element Tiles") {
                SingleElementTiles<field_type> t;

                SECTION("No permutation") {
                    extents_type matrix{3, 5};
                    pimpl_type p(matrix, sm, idx2mode_type{0, 1});

                    auto tr = t.make_tiled_range(matrix);
                    TA::Tensor<float> corr_data(TA::Range(3, 5),
                                                {0.0, fmax, 0.0, fmax, 0.0,
                                                 fmax, 0.0, fmax, 0.0, fmax,
                                                 fmax, 0.0, 0.0, 0.0, fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }

                SECTION("Permutation") {
                    extents_type matrix{5, 3};
                    pimpl_type p(matrix, sm, idx2mode_type{1, 0});

                    auto tr = t.make_tiled_range(matrix);
                    TA::Tensor<float> corr_data(TA::Range(5, 3),
                                                {0.0, fmax, fmax, fmax, 0.0,
                                                 0.0, 0.0, fmax, 0.0, fmax, 0.0,
                                                 0.0, 0.0, fmax, fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }
            }

            SECTION("One Big Tile") {
                OneBigTile<field_type> t;

                SECTION("No permutation") {
                    extents_type matrix{3, 5};
                    pimpl_type p(matrix, sm, idx2mode_type{0, 1});

                    auto tr = t.make_tiled_range(matrix);
                    TA::Tensor<float> corr_data(TA::Range(1, 1), {fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }

                SECTION("Permutation") {
                    extents_type matrix{5, 3};
                    pimpl_type p(matrix, sm, idx2mode_type{1, 0});

                    auto tr = t.make_tiled_range(matrix);
                    TA::Tensor<float> corr_data(TA::Range(1, 1), {fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }
            }
        }

        SECTION("Rank 1-2 Shapes") {
            extents_type extents{3, 2, 2};
            sm_type sm{
              {i0, {i01, i10}}, {i1, {i00, i11}}, {i2, {i00, i01, i10}}};

            SECTION("Single Element Tiles") {
                SingleElementTiles<field_type> t;
                auto tr = t.make_tiled_range(extents);

                SECTION("No permutation") {
                    pimpl_type p(extents, sm, idx2mode_type{0, 1, 2});

                    TA::Tensor<float> corr_data(
                      TA::Range(3, 2, 2), {0.0, fmax, fmax, 0.0, fmax, 0.0, 0.0,
                                           fmax, fmax, fmax, fmax, 0.0});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }

                SECTION("Permutation") {
                    pimpl_type p(extents, sm, idx2mode_type{0, 2, 1});

                    TA::Tensor<float> corr_data(
                      TA::Range(3, 2, 2), {0.0, fmax, fmax, 0.0, fmax, 0.0, 0.0,
                                           fmax, fmax, fmax, fmax, 0.0});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }
            }

            SECTION("One Big Tile") {
                OneBigTile<field_type> t;
                auto tr = t.make_tiled_range(extents);

                SECTION("No permutation") {
                    pimpl_type p(extents, sm, idx2mode_type{0, 1, 2});
                    TA::Tensor<float> corr_data(TA::Range(1, 1, 1), {fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }

                SECTION("Permutation") {
                    pimpl_type p(extents, sm, idx2mode_type{0, 2, 1});
                    TA::Tensor<float> corr_data(TA::Range(1, 1, 1), {fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }
            }
        }

        SECTION("Rank 2-1 Shapes") {
            extents_type extents{2, 2, 3};
            sm_type sm{
              {i00, {i0, i2}}, {i01, {i1}}, {i10, {i1}}, {i11, {i1, i2}}};

            SECTION("Single Element Tiles") {
                SingleElementTiles<field_type> t;
                auto tr = t.make_tiled_range(extents);

                SECTION("No permutation") {
                    pimpl_type p(extents, sm, idx2mode_type{0, 1, 2});

                    TA::Tensor<float> corr_data(
                      TA::Range(2, 2, 3), {fmax, 0.0, fmax, 0.0, fmax, 0.0, 0.0,
                                           fmax, 0.0, 0.0, fmax, fmax});

                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }

                SECTION("Permutation") {
                    pimpl_type p(extents, sm, idx2mode_type{1, 0, 2});

                    TA::Tensor<float> corr_data(
                      TA::Range(2, 2, 3), {fmax, 0.0, fmax, 0.0, fmax, 0.0, 0.0,
                                           fmax, 0.0, 0.0, fmax, fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }
            }

            SECTION("One Big Tile") {
                OneBigTile<field_type> t;
                auto tr = t.make_tiled_range(extents);

                SECTION("No permutation") {
                    pimpl_type p(extents, sm, idx2mode_type{0, 1, 2});
                    TA::Tensor<float> corr_data(TA::Range(1, 1, 1), {fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }

                SECTION("Permutation") {
                    pimpl_type p(extents, sm, idx2mode_type{1, 0, 2});
                    TA::Tensor<float> corr_data(TA::Range(1, 1, 1), {fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }
            }
        }

        SECTION("Throws if tiled range is inconsistent") {
            sm_type sm{{i0, {i1, i3}}, {i1, {i0, i2, i4}}, {i2, {i0, i4}}};
            extents_type matrix{3, 5};
            pimpl_type p(matrix, sm, idx2mode_type{0, 1});
            TA::TiledRange tr{{0, 3}, {0, 5}, {0, 6}};
            REQUIRE_THROWS_AS(p.shape(tr), std::runtime_error);
        }
    }

    // Checks that extents get forwarded to the base class correctly
    SECTION("extents") {
        sm_type sm{{i0, {i1, i3}}, {i1, {i0, i2, i4}}, {i2, {i0, i4}}};
        extents_type matrix{3, 5};
        auto* pmatrix = matrix.data();
        pimpl_type p(std::move(matrix), sm, idx2mode_type{0, 1});

        REQUIRE(p.extents() == extents_type{3, 5});
        REQUIRE(p.extents().data() == pmatrix);
    }

    SECTION("hash") {
        using pluginplay::hash_objects;

        sm_type sm{{i0, {i1, i3}}, {i1, {i0, i2, i4}}, {i2, {i0, i4}}};
        extents_type matrix{3, 5};
        idx2mode_type i2m{0, 1};
        auto lhs = hash_objects(pimpl_type(matrix, sm, i2m));

        REQUIRE(lhs == hash_objects(pimpl_type(matrix, sm, i2m)));

        SECTION("Different extents") {
            extents_type mat2{5, 5};
            auto rhs = hash_objects(pimpl_type(mat2, sm, i2m));
            REQUIRE(lhs != rhs);
        }

        SECTION("Different sm") {
            sm_type sm2{{i0, {i1}}, {i1, {i0, i2, i4}}, {i2, {i0, i4}}};
            auto rhs = hash_objects(pimpl_type(matrix, sm2, i2m));
            REQUIRE(lhs != rhs);
        }

        SECTION("Different permutation") {
            idx2mode_type i2m2{1, 0};
            auto rhs = hash_objects(pimpl_type(matrix, sm, i2m2));
            REQUIRE(lhs != rhs);
        }

        SECTION("Different most derived classes") {
            auto rhs = hash_objects(detail_::ShapePIMPL<field_type>(matrix));
            REQUIRE(lhs != rhs);
        }
    }

    SECTION("comparison") {
        sm_type sm{{i0, {i1, i3}}, {i1, {i0, i2, i4}}, {i2, {i0, i4}}};
        extents_type matrix{3, 5};
        idx2mode_type i2m{0, 1};
        pimpl_type lhs(matrix, sm, i2m);

        REQUIRE(lhs == pimpl_type(matrix, sm, i2m));

        // Different extents
        REQUIRE_FALSE(lhs == pimpl_type(extents_type{5, 5}, sm, i2m));

        // Different SM
        sm_type sm2{{i0, {i1}}, {i1, {i0, i2, i4}}, {i2, {i0, i4}}};
        REQUIRE_FALSE(lhs == pimpl_type(matrix, sm2, i2m));

        // Different permutation
        REQUIRE_FALSE(lhs == pimpl_type(matrix, sm, idx2mode_type{1, 0}));
    }
}

TEST_CASE("SparseShapePIMPL<field::Tensor>") {
    using field_type    = field::Tensor;
    using pimpl_type    = detail_::SparseShapePIMPL<field_type>;
    using extents_type  = typename pimpl_type::extents_type;
    using sm_type       = typename pimpl_type::sparse_map_type;
    using el_index      = typename sm_type::key_type;
    using idx2mode_type = typename pimpl_type::idx2mode_type;
    using ta_shape_type = typename pimpl_type::ta_shape_type;

    // This is the non-zero value which gets put in the TA shape
    auto fmax = std::numeric_limits<float>::max();

    el_index i0{0}, i1{1}, i2{2}, i3{3}, i4{4};
    el_index i00{0, 0}, i01{0, 1}, i10{1, 0}, i11{1, 1};

    SECTION("CTors") {
        sm_type sm{{i00, {i4}}};
        extents_type extents{2, 3};
        idx2mode_type i2m{0, 1};

        SECTION("Value") {
            // Extents rank doesn't match SM
            extents_type bad_extents{2};
            REQUIRE_THROWS_AS(pimpl_type(bad_extents, sm, i2m),
                              std::runtime_error);

            // i2m rank doesn't match SM (and thus extents)
            idx2mode_type bad_i2m{0};
            REQUIRE_THROWS_AS(pimpl_type(extents, sm, bad_i2m),
                              std::runtime_error);

            // rank in i2m is out of bounds
            idx2mode_type bad_i2m2{0, 2};
            REQUIRE_THROWS_AS(pimpl_type(extents, sm, bad_i2m2),
                              std::out_of_range);
        }

        SECTION("copy") {
            pimpl_type p0(extents, sm, i2m);
            pimpl_type p1(p0);

            REQUIRE(p0 == p1);
        }
    }

    SECTION("shape()") {
        SECTION("Rank 2 Shapes") {
            sm_type sm{{i0, {i1, i3}}, {i2, {i4}}};

            SECTION("Single Element Tiles") {
                SingleElementTiles<field_type> t;

                pimpl_type p(extents_type{3}, sm, idx2mode_type{0});

                auto tr = t.make_tiled_range(extents_type{3});
                TA::Tensor<float> corr_data(TA::Range(3), {fmax, 0.0, fmax});
                ta_shape_type corr(corr_data, tr);
                REQUIRE(corr == p.shape(tr));
            }

            SECTION("One Big Tile") {
                OneBigTile<field_type> t;

                pimpl_type p(extents_type{3}, sm, idx2mode_type{0});

                auto tr = t.make_tiled_range(extents_type{3});
                TA::Tensor<float> corr_data(TA::Range(1), {fmax});
                ta_shape_type corr(corr_data, tr);
                REQUIRE(corr == p.shape(tr));
            }
        }

        SECTION("Rank 1-2 Shapes") {
            sm_type sm{{i0, {i10, i01}}, {i2, {i00}}};

            SECTION("Single Element Tiles") {
                SingleElementTiles<field_type> t;

                pimpl_type p(extents_type{3}, sm, idx2mode_type{0});

                auto tr = t.make_tiled_range(extents_type{3});
                TA::Tensor<float> corr_data(TA::Range(3), {fmax, 0.0, fmax});
                ta_shape_type corr(corr_data, tr);
                REQUIRE(corr == p.shape(tr));
            }

            SECTION("One Big Tile") {
                OneBigTile<field_type> t;

                pimpl_type p(extents_type{3}, sm, idx2mode_type{0});

                auto tr = t.make_tiled_range(extents_type{3});
                TA::Tensor<float> corr_data(TA::Range(1), {fmax});
                ta_shape_type corr(corr_data, tr);
                REQUIRE(corr == p.shape(tr));
            }
        }

        SECTION("Rank 2-1 Shapes") {
            sm_type sm{{i00, {i0, i1}}, {i01, {i0, i2}}, {i11, {i0, i2}}};

            SECTION("Single Element Tiles") {
                SingleElementTiles<field_type> t;

                SECTION("No permutation") {
                    pimpl_type p(extents_type{2, 2}, sm, idx2mode_type{0, 1});

                    auto tr = t.make_tiled_range(extents_type{2, 2});
                    TA::Tensor<float> corr_data(TA::Range(2, 2),
                                                {fmax, fmax, 0.0, fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }

                SECTION("Permutation") {
                    pimpl_type p(extents_type{2, 2}, sm, idx2mode_type{1, 0});

                    auto tr = t.make_tiled_range(extents_type{2, 2});
                    TA::Tensor<float> corr_data(TA::Range(2, 2),
                                                {fmax, 0.0, fmax, fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }
            }

            SECTION("One Big Tile") {
                OneBigTile<field_type> t;

                SECTION("No permutation") {
                    pimpl_type p(extents_type{2, 2}, sm, idx2mode_type{0, 1});

                    auto tr = t.make_tiled_range(extents_type{2, 2});
                    TA::Tensor<float> corr_data(TA::Range(1, 1), {fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }

                SECTION("Permutation") {
                    pimpl_type p(extents_type{2, 2}, sm, idx2mode_type{1, 0});

                    auto tr = t.make_tiled_range(extents_type{2, 2});
                    TA::Tensor<float> corr_data(TA::Range(1, 1), {fmax});
                    ta_shape_type corr(corr_data, tr);
                    REQUIRE(corr == p.shape(tr));
                }
            }
        }

        SECTION("Throws if tiled range is inconsistent") {
            sm_type sm{{i0, {i1, i3}}, {i1, {i0, i2, i4}}, {i2, {i0, i4}}};
            extents_type matrix{3};
            pimpl_type p(matrix, sm, idx2mode_type{0});
            TA::TiledRange tr{{0, 3}, {0, 5}};
            REQUIRE_THROWS_AS(p.shape(tr), std::runtime_error);
        }
    }

    // Checks that extents get forwarded to the base class correctly
    SECTION("extents") {
        sm_type sm{{i0, {i1, i3}}, {i1, {i0, i2, i4}}, {i2, {i0, i4}}};
        extents_type matrix{3};
        auto* pmatrix = matrix.data();
        pimpl_type p(std::move(matrix), sm, idx2mode_type{0});

        REQUIRE(p.extents() == extents_type{3});
        REQUIRE(p.extents().data() == pmatrix);
    }

    SECTION("hash") {
        using pluginplay::hash_objects;

        sm_type sm{{i00, {i1, i3}}, {i10, {i0, i2, i4}}, {i11, {i0, i4}}};
        extents_type extents{2, 2};
        idx2mode_type i2m{0, 1};
        auto lhs = hash_objects(pimpl_type(extents, sm, i2m));

        // Same
        REQUIRE(lhs == hash_objects(pimpl_type(extents, sm, i2m)));

        SECTION("Different extents") {
            extents_type mat2{5, 5};
            auto rhs = hash_objects(pimpl_type(mat2, sm, i2m));
            REQUIRE(lhs != rhs);
        }

        SECTION("Different sm") {
            sm_type sm2{{i00, {i1}}, {i01, {i0, i2, i4}}, {i11, {i0, i4}}};
            auto rhs = hash_objects(pimpl_type(extents, sm2, i2m));
            REQUIRE(lhs != rhs);
        }

        SECTION("Different permutation") {
            idx2mode_type i2m2{1, 0};
            auto rhs = hash_objects(pimpl_type(extents, sm, i2m2));
            REQUIRE(lhs != rhs);
        }

        SECTION("Different most derived classes") {
            auto rhs = hash_objects(detail_::ShapePIMPL<field_type>(extents));
            REQUIRE(lhs != rhs);
        }
    }

    SECTION("comparisons") {
        sm_type sm{{i00, {i1, i3}}, {i10, {i0, i2, i4}}, {i11, {i0, i4}}};
        extents_type extents{2, 2};
        idx2mode_type i2m{0, 1};
        pimpl_type lhs(extents, sm, i2m);

        // Same
        REQUIRE(lhs == pimpl_type(extents, sm, i2m));

        REQUIRE_FALSE(lhs == pimpl_type(extents_type{5, 5}, sm, i2m));

        // Different SM
        sm_type sm2{{i00, {i1}}, {i01, {i0, i2, i4}}, {i11, {i0, i4}}};
        REQUIRE_FALSE(lhs == pimpl_type(extents, sm2, i2m));

        // Different permutation
        REQUIRE_FALSE(lhs == pimpl_type(extents, sm, idx2mode_type{1, 0}));
    }
}
