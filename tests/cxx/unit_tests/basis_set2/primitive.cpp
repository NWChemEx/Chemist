#include <catch2/catch.hpp>
#include <chemist/basis_set2/primitive/primitive.hpp>

TEMPLATE_TEST_CASE("Primitive", "", float, double) {
    using prim_type = chemist::basis_set::Primitive<TestType>;
}
