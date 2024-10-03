#include "../../test_qm.hpp"
#include <chemist/quantum_mechanics/braket/braket_class.hpp>
#include <chemist/quantum_mechanics/operator/kinetic.hpp>
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>

/* Testing Strategy.
 *
 * BraKetImpl is an abstract base class so we can not make instances of it, we
 * must make instances of derived objects and test the BraKetBase piece. We
 * spot check one BraKetBase<TensorRepresentation>
 */

using namespace chemist::braket;

TEST_CASE("BraKetBase<TensorRepresentation>") {
    using chemist::Electron;
    using chemist::qm_operator::Kinetic;
    using chemist::wavefunction::AOs;

    using braket_type   = BraKet<AOs, Kinetic<Electron>, AOs>;
    using bra_type      = typename braket_type::bra_type;
    using operator_type = typename braket_type::operator_type;

    bra_type empty_aos;
    bra_type aos(test_chemist::h2_basis());
    operator_type t_e;

    braket_type braket(aos, t_e, aos);

    SECTION("clone") {
        // Relies on copy ctor so spot checking is fine if copy ctor works
        auto pbraket = braket.clone();
        REQUIRE(pbraket->are_equal(braket));
    }

    SECTION("are_equal") {
        // Relies on operator==, so spot checking is fine

        SECTION("Same") {
            braket_type rhs(aos, t_e, aos);
            REQUIRE(braket.are_equal(rhs));
        }

        SECTION("Different, but same derived type") {
            braket_type rhs(empty_aos, t_e, aos);
            REQUIRE_FALSE(braket.are_equal(rhs));
        }
    }

    SECTION("are_different") {
        // Negates are_equal so spot checking is fine if are_equal works

        SECTION("Same") {
            braket_type rhs(aos, t_e, aos);
            REQUIRE_FALSE(braket.are_different(rhs));
        }

        SECTION("Different, but same derived type") {
            braket_type rhs(empty_aos, t_e, aos);
            REQUIRE(braket.are_different(rhs));
        }
    }
}