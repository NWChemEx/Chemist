#include "libchemist/operator/coulomb.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;

using coulomb_types =
  std::tuple<ElectronRepulsion, NuclearRepulsion, ElectronNuclearCoulomb,
             ElectronEDensityCoulomb>;

TEMPLATE_LIST_TEST_CASE("CoulombInteraction", "", coulomb_types) {
    using r12_type = TestType;

    r12_type r12;

    SECTION("CTors") {}
}
