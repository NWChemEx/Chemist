#include "../../test_helpers.hpp"
#include "../test_qm.hpp"
#include <chemist/quantum_mechanics/operator/operator.hpp>

/* Testing Strategy.
 *
 * The ElectronicHamiltonian class is a strong type over a linear combination.
 * We know the LinearCombinationImpl class works from testing so here we only
 * test features added on to it by the ElectronicHamiltonian class. These
 * include
 *
 * - The ctors (largely just forward, but are tested for good measure).
 *
 */

using namespace chemist::qm_operator;

TEST_CASE("ElectronicHamiltonian") {
    types::T_e_type T_e;
    types::V_en_type V_en;
    types::V_ee_type V_ee;

    ElectronicHamiltonian defaulted;
    ElectronicHamiltonian H(T_e + V_en + V_ee);

    SECTION("Ctors") {
        SECTION("Defaulted") { REQUIRE(defaulted.size() == 0); }
        SECTION("Value") {
            REQUIRE(H.size() == 3);
            REQUIRE(H.coefficient(0) == 1.0);
            REQUIRE(H.get_operator(0).are_equal(T_e));
            REQUIRE(H.coefficient(1) == 1.0);
            REQUIRE(H.get_operator(1).are_equal(V_en));
            REQUIRE(H.coefficient(2) == 1.0);
            REQUIRE(H.get_operator(2).are_equal(V_ee));
        }

        test_chemist::test_copy_and_move(defaulted, H);
    }

    SECTION("core_hamiltonian") {
        CoreHamiltonian corr(T_e + V_en);
        REQUIRE(H.core_hamiltonian() == corr);
    }
}