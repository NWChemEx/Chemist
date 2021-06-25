#include "libchemist/set_theory/subset.hpp"
#include "test_set_theory.hpp"

TEMPLATE_LIST_TEST_CASE("Subset", "", container_types) {
    using family_type = FamilyOfSets<TestType>;
    using subset_type = Subset<TestType>;

    // Make a default/empty and non-default parent set
    TestType default_obj;
    auto non_default_obj = testing::make_object<TestType>();

    // Make family of sets around the parent sets
    family_type non_default(non_default_obj);
    family_type defaulted(default_obj);

    // Only possible subset for an empty parent set
    subset_type empty_defaulted(defaulted);

    // Make some subsets for the non-default parent set
    subset_type empty_non_defaulted(non_default);
    subset_type e0(non_default);
    subset_type e2(non_default);
    subset_type e01(non_default);
    e0.insert(0);
    e2.insert(2);
    e01.insert(0);
    e01.insert(1);

    SECTION("size()") {
        REQUIRE(empty_defaulted.size() == 0);
        REQUIRE(empyty_non_defaulted.size() == 0);
        REQUIRE(e0.size() == 1);
        REQUIRE(e2.size() == 1);
        REQUIRE(e01.size() == 2);
    }

    SECTION("empty()") {
        REQUIRE(empty_defaulted.empty());
        REQUIRE(empty_non_defaulted.empty());
        REQUIRE_FALSE(e0.empty());
        REQUIRE_FALSE(e2.empty());
        REQUIRE_FALSE(e01.empty());
    }
}