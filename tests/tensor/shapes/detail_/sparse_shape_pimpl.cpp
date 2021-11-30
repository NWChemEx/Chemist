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

    SECTION("Rank 2 Shapes") {
        sm_type sm{{i0, {i1, i3}}, {i1, {i0, i2, i4}}, {i2, {i0, i4}}};

        SECTION("Single Element Tiles") {
            SingleElementTiles<field_type> t;

            SECTION("No permutation") {
                extents_type matrix{3, 5};
                pimpl_type p(matrix, sm, idx2mode_type{0, 1});

                auto tr = t.make_tiled_range(matrix);
                TA::Tensor<float> corr_data(
                  TA::Range(3, 5), {0.0, fmax, 0.0, fmax, 0.0, fmax, 0.0, fmax,
                                    0.0, fmax, fmax, 0.0, 0.0, 0.0, fmax});
                ta_shape_type corr(corr_data, tr);
                REQUIRE(corr == p.shape(tr));
            }

            SECTION("Permutation") {
                extents_type matrix{5, 3};
                pimpl_type p(matrix, sm, idx2mode_type{1, 0});

                auto tr = t.make_tiled_range(matrix);
                TA::Tensor<float> corr_data(
                  TA::Range(5, 3), {0.0, fmax, fmax, fmax, 0.0, 0.0, 0.0, fmax,
                                    0.0, fmax, 0.0, 0.0, 0.0, fmax, fmax});
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
        sm_type sm{{i0, {i01, i10}}, {i1, {i00, i11}}, {i2, {i00, i01, i10}}};

        SECTION("Single Element Tiles") {
            SingleElementTiles<field_type> t;
            auto tr = t.make_tiled_range(extents);

            SECTION("No permutation") {
                pimpl_type p(extents, sm, idx2mode_type{0, 1, 2});

                TA::Tensor<float> corr_data(TA::Range(3, 2, 2),
                                            {0.0, fmax, fmax, 0.0, fmax, 0.0,
                                             0.0, fmax, fmax, fmax, fmax, 0.0});
                ta_shape_type corr(corr_data, tr);
                REQUIRE(corr == p.shape(tr));
            }

            SECTION("Permutation") {
                pimpl_type p(extents, sm, idx2mode_type{0, 2, 1});

                TA::Tensor<float> corr_data(TA::Range(3, 2, 2),
                                            {0.0, fmax, fmax, 0.0, fmax, 0.0,
                                             0.0, fmax, fmax, fmax, fmax, 0.0});
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
        sm_type sm{{i00, {i0, i2}}, {i01, {i1}}, {i10, {i1}}, {i11, {i1, i2}}};

        SECTION("Single Element Tiles") {
            SingleElementTiles<field_type> t;
            auto tr = t.make_tiled_range(extents);

            SECTION("No permutation") {
                pimpl_type p(extents, sm, idx2mode_type{0, 1, 2});

                TA::Tensor<float> corr_data(TA::Range(2, 2, 3),
                                            {fmax, 0.0, fmax, 0.0, fmax, 0.0,
                                             0.0, fmax, 0.0, 0.0, fmax, fmax});

                ta_shape_type corr(corr_data, tr);
                REQUIRE(corr == p.shape(tr));
            }

            SECTION("Permutation") {
                pimpl_type p(extents, sm, idx2mode_type{1, 0, 2});

                TA::Tensor<float> corr_data(TA::Range(2, 2, 3),
                                            {fmax, 0.0, fmax, 0.0, fmax, 0.0,
                                             0.0, fmax, 0.0, 0.0, fmax, fmax});
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

    SECTION("Rank 2 Shapes") {
        sm_type sm{{i0, {i1, i3}}, {i2, {i4}}};

        SECTION("Single Element Tiles") {
            SingleElementTiles<field_type> t;

            pimpl_type p(extents_type{3, 5}, sm, idx2mode_type{0, 1});

            auto tr = t.make_tiled_range(extents_type{3});
            TA::Tensor<float> corr_data(TA::Range(3), {fmax, 0.0, fmax});
            ta_shape_type corr(corr_data, tr);
            REQUIRE(corr == p.shape(tr));
        }

        SECTION("One Big Tile") {
            OneBigTile<field_type> t;

            pimpl_type p(extents_type{3, 5}, sm, idx2mode_type{0, 1});

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

            pimpl_type p(extents_type{3, 2, 2}, sm, idx2mode_type{0, 1, 2});

            auto tr = t.make_tiled_range(extents_type{3});
            TA::Tensor<float> corr_data(TA::Range(3), {fmax, 0.0, fmax});
            ta_shape_type corr(corr_data, tr);
            REQUIRE(corr == p.shape(tr));
        }

        SECTION("One Big Tile") {
            OneBigTile<field_type> t;

            pimpl_type p(extents_type{3, 2, 2}, sm, idx2mode_type{0, 1, 2});

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
                pimpl_type p(extents_type{2, 2, 3}, sm, idx2mode_type{0, 1, 2});

                auto tr = t.make_tiled_range(extents_type{2, 2});
                TA::Tensor<float> corr_data(TA::Range(2, 2),
                                            {fmax, fmax, 0.0, fmax});
                ta_shape_type corr(corr_data, tr);
                REQUIRE(corr == p.shape(tr));
            }

            SECTION("Permutation") {
                pimpl_type p(extents_type{2, 2, 3}, sm, idx2mode_type{1, 0, 2});

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
                pimpl_type p(extents_type{2, 2, 3}, sm, idx2mode_type{0, 1, 2});

                auto tr = t.make_tiled_range(extents_type{2, 2});
                TA::Tensor<float> corr_data(TA::Range(1, 1), {fmax});
                ta_shape_type corr(corr_data, tr);
                REQUIRE(corr == p.shape(tr));
            }

            SECTION("Permutation") {
                pimpl_type p(extents_type{2, 2, 3}, sm, idx2mode_type{1, 0, 2});

                auto tr = t.make_tiled_range(extents_type{2, 2});
                TA::Tensor<float> corr_data(TA::Range(1, 1), {fmax});
                ta_shape_type corr(corr_data, tr);
                REQUIRE(corr == p.shape(tr));
            }
        }
    }
}
