#include "chemist/tensor/shapes/detail_/shape_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace chemist::tensor;

using fields = std::tuple<field::Scalar, field::Tensor>;

TEMPLATE_LIST_TEST_CASE("ShapePIMP", "", fields) {
    using field_type   = TestType;
    using pimpl_type   = detail_::ShapePIMPL<field_type>;
    using extents_type = typename pimpl_type::extents_type;

    extents_type scalar_extents;
    extents_type vector_extents{3};
    extents_type matrix_extents{3, 4};
    extents_type tensor_extents{3, 4, 5};

    pimpl_type defaulted;
    pimpl_type scalar(scalar_extents);
    pimpl_type vector(vector_extents);
    pimpl_type matrix(matrix_extents);
    pimpl_type tensor(tensor_extents);

    SECTION("CTors") {
        SECTION("Default") { REQUIRE(defaulted.extents() == scalar_extents); }

        SECTION("Value") {
            REQUIRE(scalar.extents() == scalar_extents);
            REQUIRE(vector.extents() == vector_extents);
            REQUIRE(matrix.extents() == matrix_extents);
            REQUIRE(tensor.extents() == tensor_extents);

            // Make sure object is forwarded correctly (i.e. no copy)
            auto pt = tensor_extents.data();
            pimpl_type tensor2(std::move(tensor_extents));
            REQUIRE(tensor2.extents().data() == pt);
        }
    }

    SECTION("clone()") {
        REQUIRE(*scalar.clone() == scalar);
        REQUIRE(*vector.clone() == vector);
        REQUIRE(*matrix.clone() == matrix);
        REQUIRE(*tensor.clone() == tensor);
    }

    SECTION("extents() const") {
        REQUIRE(defaulted.extents() == scalar_extents);
        REQUIRE(scalar.extents() == scalar_extents);
        REQUIRE(vector.extents() == vector_extents);
        REQUIRE(matrix.extents() == matrix_extents);
        REQUIRE(tensor.extents() == tensor_extents);
    }

    SECTION("hash") {
        using pluginplay::hash_objects;
        REQUIRE(hash_objects(defaulted) == hash_objects(scalar));

        const auto v2_hash = hash_objects(pimpl_type(vector_extents));
        REQUIRE(hash_objects(vector) == v2_hash);

        const auto m2_hash = hash_objects(pimpl_type(matrix_extents));
        REQUIRE(hash_objects(matrix) == m2_hash);

        REQUIRE_FALSE(hash_objects(defaulted) == hash_objects(vector));

        REQUIRE_FALSE(hash_objects(vector) == hash_objects(matrix));

        const auto v3_hash = hash_objects(pimpl_type(extents_type{5}));
        REQUIRE_FALSE(hash_objects(vector) == v3_hash);
    }

    SECTION("Equality") {
        REQUIRE(defaulted == scalar);
        REQUIRE(vector == pimpl_type(vector_extents));
        REQUIRE(matrix == pimpl_type(matrix_extents));

        REQUIRE_FALSE(defaulted == vector); // default doesn't equal filled
        REQUIRE_FALSE(vector == matrix);    // different ranks
        REQUIRE_FALSE(vector == pimpl_type(extents_type{5})); // different size
    }
}
