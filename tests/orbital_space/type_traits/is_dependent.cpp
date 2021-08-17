#include "libchemist/orbital_space/orbital_space.hpp"
#include "libchemist/orbital_space/type_traits/is_dependent.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::orbital_space;

using dependent_tuple =
  std::tuple<DepAOSpaceD, DepDerivedSpace, CanonicalDepSpace>;
using not_dependent_tuple = std::tuple<AOSpaceD, DerivedSpaceD, CanonicalSpaceD,
                                       IndDerivedSpace, CanonicalIndSpace>;

TEMPLATE_LIST_TEST_CASE("is_dependent_v(true)", "", dependent_tuple) {
    using type = TestType;

    STATIC_REQUIRE(is_dependent_v<type>);
}

TEMPLATE_LIST_TEST_CASE("is_dependent_v(false)", "", not_dependent_tuple) {
    using type = TestType;

    STATIC_REQUIRE_FALSE(is_dependent_v<type>);
}
