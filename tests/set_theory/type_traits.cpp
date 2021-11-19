#include "chemist/set_theory/type_traits.hpp"
#include <catch2/catch.hpp>
#include <vector>

using namespace chemist::set_theory;

using type_list = std::tuple<int, double, float, std::string>;

TEMPLATE_LIST_TEST_CASE("IsFamilyOfSets", "", type_list) {
    using T   = TestType;
    using fos = FamilyOfSets<std::vector<T>>;
    SECTION("Not a family of sets") {
        STATIC_REQUIRE_FALSE(detail_::IsFamilyOfSets<T>::value);
    }
    SECTION("Is a family of sets") {
        STATIC_REQUIRE(detail_::IsFamilyOfSets<fos>::value);
    }
}

TEMPLATE_LIST_TEST_CASE("is_family_of_sets_v", "", type_list) {
    using T   = TestType;
    using fos = FamilyOfSets<std::vector<T>>;
    SECTION("Not a family") { STATIC_REQUIRE_FALSE(is_family_of_sets_v<T>); }
    SECTION("Is a family") { STATIC_REQUIRE(is_family_of_sets_v<fos>); }
}
