#include "chemist/orbital_space/canonical_space.hpp"
#include "chemist/orbital_space/type_traits/is_canonical.hpp"
#include <catch2/catch.hpp>

using namespace chemist::orbital_space;

using non_canonical_tuple = std::tuple<DerivedSpaceD, ToTDerivedSpace>;
using canonical_tuple     = std::tuple<CanonicalSpaceD, CanonicalToTSpace>;

TEMPLATE_LIST_TEST_CASE("IsCanonicalSpace (false)", "", non_canonical_tuple) {
    STATIC_REQUIRE_FALSE(detail_::IsCanonicalSpace<TestType>::value);
}

TEMPLATE_LIST_TEST_CASE("IsCanonicalSpace (true)", "", canonical_tuple) {
    STATIC_REQUIRE(detail_::IsCanonicalSpace<TestType>::value);
}

TEMPLATE_LIST_TEST_CASE("is_canonical_space_v (false)", "",
                        non_canonical_tuple) {
    STATIC_REQUIRE_FALSE(is_canonical_space_v<TestType>);
}

TEMPLATE_LIST_TEST_CASE("is_canonical_space_v (true)", "", canonical_tuple) {
    STATIC_REQUIRE(is_canonical_space_v<TestType>);
}
