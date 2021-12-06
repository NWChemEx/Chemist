#include "chemist/tensor/allocators/allocators.hpp"
#include "chemist/tensor/shapes/shapes.hpp"
#include <catch2/catch.hpp>

using namespace chemist::tensor;

/* Testing Strategy:
 *
 * We assume that the PIMPL works and that the base class works. We need to test
 * that:
 * - the ctor forwards arguments to PIMPL correctly
 * - clone behaves polymorphically
 * - the tensor returned is correct
 * - comparisons
 */
TEST_CASE("SparseShape<field::Scalar>") {
    using field_type      = field::Scalar;
    using shape_type      = SparseShape<field_type>;
    using extents_type    = typename shape_type::extents_type;
    using sparse_map_type = typename shape_type::sparse_map_type;
    using idx_type        = typename sparse_map_type::key_type;
    using idx2mode_type   = typename shape_type::idx2mode_type;

    extents_type matrix_extents{3, 4};
    extents_type tensor_extents{2, 2, 2};

    idx_type i0{0}, i1{1}, i00{0, 0}, i11{1, 1};
    sparse_map_type matrix_sm{{i0, {i0}}, {i1, {i1}}};
    sparse_map_type tensor_sm{{i00, {i0}}, {i11, {i1}}};

    idx2mode_type i2m{1, 0};
    idx2mode_type i2m1{1, 2, 0};

    shape_type m(matrix_extents, matrix_sm);
    shape_type mt(matrix_extents, matrix_sm, i2m);
    shape_type t(tensor_extents, tensor_sm);
    shape_type tt(tensor_extents, tensor_sm, i2m1);

    SECTION("CTors") {
        SECTION("No idx2mode") {
            REQUIRE(m.extents() == matrix_extents);
            REQUIRE(t.extents() == tensor_extents);

            // Make sure there's not an extra copy
            auto pm = matrix_extents.data();
            shape_type m2(std::move(matrix_extents), matrix_sm);
            REQUIRE(m2.extents().data() == pm);

            // Throws if sm is inconsistent with extents
            REQUIRE_THROWS_AS(shape_type(matrix_extents, tensor_sm),
                              std::runtime_error);
        }

        SECTION("idx2mode") {
            REQUIRE(mt.extents() == matrix_extents);
            REQUIRE(tt.extents() == tensor_extents);

            // Make sure there's not an extra copy
            auto pm = matrix_extents.data();
            shape_type m2(std::move(matrix_extents), matrix_sm, i2m);
            REQUIRE(m2.extents().data() == pm);

            // Throws if sm is inconsistent with idx2mode
            REQUIRE_THROWS_AS(shape_type(matrix_extents, matrix_sm, i2m1),
                              std::runtime_error);

            // Throws if an element of idx2mode is out of range
            extents_type i2m2{0, 2};
            REQUIRE_THROWS_AS(shape_type(matrix_extents, matrix_sm, i2m2),
                              std::runtime_error);
        }
    }

    SECTION("make_tensor") {
        using ta_tensor = TA::Tensor<float>;
        using ta_shape  = TA::SparseShape<float>;

        SingleElementTiles<field_type> a;
        auto max = std::numeric_limits<float>::max();

        SECTION("Matrix") {
            auto matrix  = m.make_tensor(a);
            auto corr_tr = a.make_tiled_range(matrix_extents);

            ta_tensor shape_data(TA::Range(3, 4),
                                 {max, 0, 0, 0, 0, max, 0, 0, 0, 0, 0, 0});
            ta_shape corr_shape(shape_data, corr_tr);
            REQUIRE(std::get<0>(matrix).trange() == corr_tr);
            REQUIRE(std::get<0>(matrix).shape() == corr_shape);
        }

        SECTION("Tensor") {
            auto tensor  = t.make_tensor(a);
            auto corr_tr = a.make_tiled_range(tensor_extents);

            ta_tensor shape_data(TA::Range(2, 2, 2),
                                 {max, 0, 0, 0, 0, 0, 0, max});
            ta_shape corr_shape(shape_data, corr_tr);
            REQUIRE(std::get<0>(tensor).trange() == corr_tr);
            REQUIRE(std::get<0>(tensor).shape() == corr_shape);
        }
    }

    SECTION("hash") {
        using pluginplay::hash_objects;
        SECTION("LHS is matrix") {
            auto lhs = hash_objects(m);

            SECTION("Same state") {
                auto rhs = hash_objects(shape_type(matrix_extents, matrix_sm));
                REQUIRE(lhs == rhs);
            }

            SECTION("Different extents") {
                extents_type other_extents{5, 5};
                auto rhs = hash_objects(shape_type(other_extents, matrix_sm));
                REQUIRE(lhs != rhs);
            }

            SECTION("Different sparse maps") {
                sparse_map_type sm2{{i0, {i0, i1}}, {i1, {i0, i1}}};
                auto rhs = hash_objects(shape_type(matrix_extents, sm2));
                REQUIRE(lhs != rhs);
            }

            SECTION("Different permutation") {
                REQUIRE(lhs != hash_objects(mt));
            }

            SECTION("Is polymorphic") {
                Shape<field::Scalar> base(matrix_extents);
                REQUIRE(lhs != hash_objects(base));
            }
        }
    }

    SECTION("comparisons") {
        // Same
        REQUIRE(m == shape_type(matrix_extents, matrix_sm));
        REQUIRE_FALSE(m != shape_type(matrix_extents, matrix_sm));

        // Different fields
        using other_shape_type = SparseShape<field::Tensor>;
        REQUIRE_FALSE(m == other_shape_type(matrix_extents, tensor_sm));
        REQUIRE(m != other_shape_type(matrix_extents, tensor_sm));

        // Different extents
        REQUIRE_FALSE(m == shape_type(extents_type{5, 5}, matrix_sm));
        REQUIRE(m != shape_type(extents_type{5, 5}, matrix_sm));

        // Different sparse maps
        sparse_map_type sm2{{i0, {i0, i1}}, {i1, {i0, i1}}};
        REQUIRE_FALSE(m == shape_type(matrix_extents, sm2));
        REQUIRE(m != shape_type(matrix_extents, sm2));

        // Different permutation
        REQUIRE_FALSE(m == mt);
        REQUIRE(m != mt);

        // Base class's operator== is non-polymorphic
        using base_type = Shape<field_type>;
        base_type b(matrix_extents);
        REQUIRE(b == static_cast<base_type&>(m));
        REQUIRE_FALSE(b != static_cast<base_type&>(m));
    }
}

TEST_CASE("SparseShape<field::Tensor>") {
    using field_type      = field::Tensor;
    using shape_type      = SparseShape<field_type>;
    using extents_type    = typename shape_type::extents_type;
    using sparse_map_type = typename shape_type::sparse_map_type;
    using idx_type        = typename sparse_map_type::key_type;
    using idx2mode_type   = typename shape_type::idx2mode_type;

    extents_type extents{3, 4};

    idx_type i0{0}, i1{1}, i00{0, 0}, i11{1, 1};
    sparse_map_type sm{{i00, {i0}}, {i11, {i1}}};

    idx2mode_type i2m{1, 0};

    shape_type t(extents, sm);
    shape_type tt(extents, sm, i2m);

    SECTION("CTors") {
        SECTION("No idx2mode") {
            REQUIRE(t.extents() == extents);

            // Make sure there's not an extra copy
            auto pm = extents.data();
            shape_type m2(std::move(extents), sm);
            REQUIRE(m2.extents().data() == pm);

            sparse_map_type sm2{{i0, {i0}}, {i1, {i1}}};
            // Throws if sm is inconsistent with extents
            REQUIRE_THROWS_AS(shape_type(extents, sm2), std::runtime_error);
        }

        SECTION("idx2mode") {
            REQUIRE(tt.extents() == extents);

            // Make sure there's not an extra copy
            auto pm = extents.data();
            shape_type m2(std::move(extents), sm, i2m);
            REQUIRE(m2.extents().data() == pm);

            // Throws if sm is inconsistent with idx2mode
            REQUIRE_THROWS_AS(shape_type(extents, sm, idx2mode_type{1, 2, 0}),
                              std::runtime_error);

            // Throws if an element of idx2mode is out of range
            extents_type i2m2{0, 5};
            REQUIRE_THROWS_AS(shape_type(extents, sm, i2m2),
                              std::runtime_error);
        }
    }

    SECTION("make_tensor") {
        using ta_tensor = TA::Tensor<float>;
        using ta_shape  = TA::SparseShape<float>;

        SingleElementTiles<field_type> a;
        auto max = std::numeric_limits<float>::max();

        SECTION("Matrix") {
            auto matrix  = t.make_tensor(a);
            auto corr_tr = a.make_tiled_range(extents);

            ta_tensor shape_data(TA::Range(3, 4),
                                 {max, 0, 0, 0, 0, max, 0, 0, 0, 0, 0, 0});
            ta_shape corr_shape(shape_data, corr_tr);
            REQUIRE(std::get<0>(matrix).trange() == corr_tr);
            REQUIRE(std::get<0>(matrix).shape() == corr_shape);
        }
    }

    SECTION("hash") {
        using pluginplay::hash_objects;
        SECTION("LHS is matrix") {
            auto lhs = hash_objects(t);

            SECTION("Same state") {
                auto rhs = hash_objects(shape_type(extents, sm));
                REQUIRE(lhs == rhs);
            }

            SECTION("Different extents") {
                extents_type other_extents{5, 5};
                auto rhs = hash_objects(shape_type(other_extents, sm));
                REQUIRE(lhs != rhs);
            }

            SECTION("Different sparse maps") {
                sparse_map_type sm2{{i00, {i0, i1}}, {i11, {i0, i1}}};
                auto rhs = hash_objects(shape_type(extents, sm2));
                REQUIRE(lhs != rhs);
            }

            SECTION("Different permutation") {
                REQUIRE(lhs != hash_objects(tt));
            }

            SECTION("Is polymorphic") {
                Shape<field::Scalar> base(extents);
                REQUIRE(lhs != hash_objects(base));
            }
        }
    }

    SECTION("comparisons") {
        // Same
        REQUIRE(t == shape_type(extents, sm));
        REQUIRE_FALSE(t != shape_type(extents, sm));

        // Different fields
        using other_shape_type = SparseShape<field::Scalar>;
        REQUIRE_FALSE(t == other_shape_type(extents_type{3, 4, 5}, sm));
        REQUIRE(t != other_shape_type(extents_type{3, 4, 5}, sm));

        // Different extents
        REQUIRE_FALSE(t == shape_type(extents_type{5, 5}, sm));
        REQUIRE(t != shape_type(extents_type{5, 5}, sm));

        // Different sparse maps
        sparse_map_type sm2{{i00, {i0, i1}}, {i11, {i0, i1}}};
        REQUIRE_FALSE(t == shape_type(extents, sm2));
        REQUIRE(t != shape_type(extents, sm2));

        // Different permutation
        REQUIRE_FALSE(t == tt);
        REQUIRE(t != tt);

        // Base class's operator== is non-polymorphic
        using base_type = Shape<field_type>;
        base_type b(extents);
        REQUIRE(b == t);
        REQUIRE_FALSE(b != t);
    }
}
