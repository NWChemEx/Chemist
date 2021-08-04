#include "libchemist/operator/correlation_factor.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;

using type_list = std::tuple<libchemist::ElectronElectronSTG>;

TEMPLATE_LIST_TEST_CASE("CorrelationFactor", "", type_list) {
    using f12_type = TestType;

    f12_type f12;

    libchemist::STG stg(1.2, 2.3);
    libchemist::Electron e;
    f12_type non_default(stg, e, e);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(f12.template at<0>() == libchemist::STG{});
            REQUIRE(f12.template at<1>() == e);
            REQUIRE(f12.template at<2>() == e);
        }
        SECTION("Value") {
            REQUIRE(non_default.template at<0>() == stg);
            REQUIRE(non_default.template at<1>() == e);
            REQUIRE(non_default.template at<2>() == e);
        }
    }
}
