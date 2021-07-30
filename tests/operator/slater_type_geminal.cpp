#include "libchemist/operator/slater_type_geminal.hpp"
#include <catch2/catch.hpp>

using stg_types = std::tuple<libchemist::STG>;

TEMPLATE_LIST_TEST_CASE("SlaterTypeGeminal", "", stg_types) {
    using stg_type      = TestType;
    using coef_type     = typename stg_type::coefficient_type;
    using exponent_type = typename stg_type::exponent_type;

    SECTION("Typedefs") {
        SECTION("coefficient_type") {
            STATIC_REQUIRE(std::is_same_v<coef_type, double>);
        }

        SECTION("exponent_type") {
            STATIC_REQUIRE(std::is_same_v<exponent_type, double>);
        }
    }
}
