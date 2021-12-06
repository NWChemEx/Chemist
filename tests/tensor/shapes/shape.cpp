#include "chemist/tensor/allocators/allocators.hpp"
#include "chemist/tensor/shapes/shapes.hpp"
#include <catch2/catch.hpp>

using namespace chemist::tensor;

/* Testing Strategy:
 *
 *
 * For both specializations we assume that the underlying PIMPLs work. Thus for
 * functions which forward to the PIMPL we only need to check if the forwarding
 * works, which can be done with one sample input. For polymorphic functions we
 * leave it to derived classes to ensure they interact correctly with the base
 * class, and only test functionality explicitly implemented in the base class
 * in these tests.
 *
 */

using fields = std::tuple<field::Scalar, field::Tensor>;

TEMPLATE_LIST_TEST_CASE("Shape", "", fields) {
    using field_type           = TestType;
    constexpr bool is_scalar_v = std::is_same_v<field_type, field::Scalar>;
    using other_field =
      std::conditional_t<is_scalar_v, field::Tensor, field::Scalar>;
    using shape_type   = Shape<field_type>;
    using extents_type = typename shape_type::extents_type;
    using tensor_type  = typename shape_type::tensor_type;

    extents_type vector_extents{4};
    extents_type matrix_extents{3, 5};

    shape_type defaulted;
    shape_type vector(vector_extents);
    shape_type matrix(matrix_extents);

    SECTION("CTors") {
        SECTION("Value") {
            REQUIRE(vector.extents() == vector_extents);
            REQUIRE(matrix.extents() == matrix_extents);

            // Ensure that extents are properly moved
            auto* vp = vector_extents.data();
            shape_type v2(std::move(vector_extents));
            REQUIRE(v2.extents().data() == vp);
        }

        SECTION("Clone") {
            auto pv = vector.clone();
            REQUIRE(*pv == vector);
        }
    }

    SECTION("extents") {
        REQUIRE_THROWS_AS(defaulted.extents(), std::runtime_error);
        REQUIRE(vector.extents() == vector_extents);
        REQUIRE(matrix.extents() == matrix_extents);
    }

    SECTION("make_tensor") {
        auto a   = default_allocator<field_type>();
        auto t   = vector.make_tensor(*a);
        auto& t0 = std::get<0>(t);
        REQUIRE(t0.trange() == a->make_tiled_range(vector.extents()));
    }

    SECTION("Comparisons") {
        // LHS is defaulted
        REQUIRE(defaulted == shape_type{});
        REQUIRE_FALSE(defaulted != shape_type{});
        REQUIRE_FALSE(defaulted == vector);
        REQUIRE(defaulted != vector);
        REQUIRE_FALSE(defaulted == matrix);
        REQUIRE(defaulted != matrix);

        // LHS is vector
        REQUIRE(vector == shape_type(vector_extents));
        REQUIRE_FALSE(vector != shape_type(vector_extents));
        REQUIRE_FALSE(vector == matrix);
        REQUIRE(vector != matrix);

        // Different Fields
        REQUIRE(defaulted != Shape<other_field>{});
        REQUIRE_FALSE(defaulted == Shape<other_field>{});
    }

    SECTION("hash") {
        using pluginplay::hash_objects;

        SECTION("LHS is defaulted") {
            auto lhs = hash_objects(defaulted);

            REQUIRE(lhs == hash_objects(shape_type{}));
            REQUIRE(lhs != hash_objects(vector));
            REQUIRE(lhs != hash_objects(matrix));
            // TODO: enable when hashing properly accounts for types
            // REQUIRE(lhs != hash_objects(Shape<other_field>{}));
        }

        SECTION("LHS is vector") {
            auto lhs = hash_objects(vector);

            REQUIRE(lhs == hash_objects(shape_type(vector_extents)));
            REQUIRE(lhs != hash_objects(matrix));
        }
    }
}
