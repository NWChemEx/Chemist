#include <catch2/catch.hpp>
#include <chemist/basis_set2/primitive/primitive.hpp>
#include <chemist/basis_set2/primitive/primitive_traits.hpp>
#include <chemist/basis_set2/primitive/primitive_view.hpp>

using namespace chemist::basis_set;

// This unit test basically just ensures that all of the types we plan to use
// PrimitiveTraits with actually define the correct API to be used with
// PrimitiveTraits

TEMPLATE_TEST_CASE("PrimitiveTraits", "", float, double) {
    using prim_type = Primitive<TestType>;

    PrimitiveTraits<prim_type> prim_traits;

    PrimitiveTraits<PrimitiveView<prim_type>> prim_view_traits;

    PrimitiveTraits<PrimitiveView<const prim_type>> const_prim_view_traits;
}
