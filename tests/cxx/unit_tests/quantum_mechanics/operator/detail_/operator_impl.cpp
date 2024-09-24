#include "../../test_helpers.hpp"
#include <chemist/chemical_system/electron/electron.hpp>
#include <chemist/quantum_mechanics/operator/kinetic.hpp>

using namespace chemist;
using namespace chemist::qm_operator;

/* Testing strategy
 *
 * For most of the derived classes representing operators, OperatorImpl
 * actually implements most of the functionality. Here we test those
 * implementations by ensuring the functionality works as intended for several
 * representative derived classes. The testing here should suffice for all
 * derived classes so long as the class does not override the implementation
 * supplied by OperatorImpl (in which case the derived class should also do
 * testing).
 */

TEST_CASE("OperatorImpl") {
    Kinetic<Electron> t;

    SECTION("clone") {
        auto pt = t.clone();
        REQUIRE(pt->are_equal(t));
    }

    SECTION("are_equal") {}
}