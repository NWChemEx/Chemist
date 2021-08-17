#include "libchemist/orbital_space/orbital_space.hpp"
#include "libchemist/orbital_space/type_traits/is_independent.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::orbital_space;

using independent_tuple = std::tuple<IndDerivedSpace, CanonicalIndSpace>;
using dependent_tuple =
  std::tuple<DepAOSpaceD, DepDerivedSpace, CanonicalDepSpace, AOSpaceD,
             DerivedSpaceD, CanonicalSpaceD>;

TEMPLATE_LIST_TEST_CASE("is_independent_v(true)", "", independent_tuple) {
    using type = TestType;

    STATIC_REQUIRE(is_independent_v<type>);
}

TEMPLATE_LIST_TEST_CASE("is_independent_v(false)", "", dependent_tuple) {
    using type = TestType;

    STATIC_REQUIRE_FALSE(is_independent_v<type>);
}
