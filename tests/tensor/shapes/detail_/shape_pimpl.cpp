#include "chemist/tensor/shapes/detail_/shape_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace chemist::tensor;

TEST_CASE("ShapePIMPL") {
    using field_type   = field::Scalar;
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

    SECTION("extents() const") {
        REQUIRE(defaulted.extents() == scalar_extents);
        REQUIRE(scalar.extents() == scalar_extents);
        REQUIRE(vector.extents() == vector_extents);
        REQUIRE(matrix.extents() == matrix_extents);
        REQUIRE(tensor.extents() == tensor_extents);
    }
}
