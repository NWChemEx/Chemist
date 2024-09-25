#include "../../../test_helpers.hpp"
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
 *
 * - Note that visit_ is tested in the VisitorBase tests.
 * - We take this test as an opportunity to test the few functions actually
 *   implemented in OperatorBase as well.
 */

TEST_CASE("OperatorImpl") {
    Kinetic<Electron> t;
    Kinetic<ManyElectrons> T{ManyElectrons{2}};

    SECTION("n_electrons") {
        STATIC_REQUIRE(t.n_electrons() == 1);
        STATIC_REQUIRE(t.n_electrons() == 1);
    }

    SECTION("at()") {
        REQUIRE(t.at<0>() == Electron{});
        REQUIRE(T.at<0>() == ManyElectrons{2});
    }

    SECTION("at() const") {
        REQUIRE(std::as_const(t).at<0>() == Electron{});
        REQUIRE(std::as_const(T).at<0>() == ManyElectrons{2});
    }

    SECTION("operator==") {
        // n.b. Kinetic does not shadow operator== so this goes through
        // OperatorImpl::operator==. Also note it won't compile if the left and
        // right sides have different types
        REQUIRE(t == Kinetic<Electron>{});
        REQUIRE(T == Kinetic<ManyElectrons>{ManyElectrons{2}});
        REQUIRE_FALSE(T == Kinetic<ManyElectrons>{ManyElectrons{1}});
    }

    SECTION("operator!=") {
        REQUIRE_FALSE(t != Kinetic<Electron>{});
        REQUIRE_FALSE(T != Kinetic<ManyElectrons>{ManyElectrons{2}});
        REQUIRE(T != Kinetic<ManyElectrons>{ManyElectrons{1}});
    }

    SECTION("clone") {
        auto pt = t.clone();
        REQUIRE(pt->are_equal(t));

        auto pT = T.clone();
        REQUIRE(pT->are_equal(T));
    }

    SECTION("are_equal") {
        REQUIRE(t.are_equal(Kinetic<Electron>{}));
        REQUIRE_FALSE(t.are_equal(T));
    }

    SECTION("are_different") {
        REQUIRE_FALSE(t.are_different(Kinetic<Electron>{}));
        REQUIRE(t.are_different(T));
    }
}