#include <catch2/catch.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>
#include <chemist/fragmenting/fragmented_nuclei.hpp>

template<typename DerivedType>
void test_fragmented_base() {
    using class_type = chemist::fragmenting::FragmentedBase<DerivedType>;

    DerivedType empty_supersystem;

    class_type defaulted;
    class_type empty(empty_supersystem);

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(defaulted.is_null()); }

        SECTION("value") {
            REQUIRE_FALSE(empty.is_null());
            REQUIRE(empty.supersystem() == empty_supersystem);
        }
    }

    SECTION("is_null") {
        REQUIRE(defaulted.is_null());
        REQUIRE_FALSE(empty.is_null());
    }
}
