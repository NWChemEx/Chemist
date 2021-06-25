#include "libchemist/set_theory/family_of_sets.hpp"
#include "test_set_theory.hpp"

TEMPLATE_LIST_TEST_CASE("FamilyOfSets", "", container_types) {
    using parent_type = TestType;
    using family_type = FamilyOfSets<TestType>;
    using value_type  = typename family_type::value_type;

    SECTION("typedefs") {
        SECTION("value_type") {
            using corr = Subset<parent_type>;
            STATIC_REQUIRE(std::is_same_v<value_type, corr>);
        }

        SECTION("reference_type") {
            using reference_type = typename family_type::reference_type;
            using corr           = value_type&;
            STATIC_REQUIRE(std::is_same_v<reference_type, corr>);
        }

        SECTION("const_reference") {
            using const_reference = typename family_type::const_reference;
            using corr            = const value_type&;
            STATIC_REQUIRE(std::is_same_v<const_reference, corr>);
        }
    }

    TestType default_obj;
    auto non_default_obj = testing::make_object<TestType>();

    family_type non_default(non_default_obj);
    family_type defaulted(default_obj);

    SECTION("CTors/Assignment") {
        SECTION("Value Ctor") {
            REQUIRE(defaulted.object() == default_obj);
            REQUIRE(non_default.object() == non_default_obj);
        }

        SECTION("Copy Ctor") {
            family_type copy(non_default);
            SECTION("value") { REQUIRE(copy.object() == non_default_obj); }
            SECTION("Is a deep-copy") {
                REQUIRE(&(copy.object()) != &(non_default.object()));
            }
        }
    }
}