#include "../../test_helpers.hpp"
#include <chemist/quantum_mechanics/operator/kinetic.hpp>

using namespace chemist;
using namespace chemist::qm_operator;

using types2test = std::tuple<Electron, ManyElectrons>;

TEMPLATE_LIST_TEST_CASE("Kinetic", "", types2test) {
    types2test defaulted_values;
    types2test values{Electron{}, ManyElectrons{3}};

    auto corr_defaulted = std::get<TestType>(defaulted_values);
    auto corr_value     = std::get<TestType>(values);

    using kinetic_type = Kinetic<TestType>;

    kinetic_type defaulted;
    kinetic_type value{corr_value};

    SECTION("Ctors and assignment") {
        SECTION("Defaulted") {
            REQUIRE(defaulted.particle() == corr_defaulted);
        }

        SECTION("Value") { REQUIRE(value.particle() == corr_value); }
    }
}