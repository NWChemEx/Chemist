#include "../../test_helpers.hpp"
#include "../test_qm.hpp"
#include <chemist/quantum_mechanics/operator/operator.hpp>

/* Testing Strategy.
 *
 * The majority of the Hamiltonian class is implemented by LinearCombinationImpl
 * and is tested there. Here we test:
 *
 * - The ctors (largely just forward, but are tested for good measure).
 *
 */

using namespace chemist::qm_operator;

TEST_CASE("Hamiltonian") {
    types::T_e_type T_e;
    types::V_en_type V_en;
    types::V_ee_type V_ee;
    types::V_nn_type V_nn;

    Hamiltonian defaulted;
    Hamiltonian H(T_e + V_en + V_ee + V_nn);

    SECTION("Ctors") {
        SECTION("Defaulted") { REQUIRE(defaulted.size() == 0); }
        SECTION("Value") {
            REQUIRE(H.size() == 4);
            REQUIRE(H.coefficient(0) == 1.0);
            REQUIRE(H.get_operator(0).are_equal(T_e));
            REQUIRE(H.coefficient(1) == 1.0);
            REQUIRE(H.get_operator(1).are_equal(V_en));
            REQUIRE(H.coefficient(2) == 1.0);
            REQUIRE(H.get_operator(2).are_equal(V_ee));
            REQUIRE(H.coefficient(3) == 1.0);
            REQUIRE(H.get_operator(3).are_equal(V_nn));
        }

        test_chemist::test_copy_and_move(defaulted, H);
    }

    SECTION("electronic_hamiltonian") {
        ElectronicHamiltonian corr(T_e + V_en + V_ee);
        REQUIRE(H.electronic_hamiltonian() == corr);
    }

    SECTION("core_hamiltonian") {
        CoreHamiltonian corr(T_e + V_en);
        REQUIRE(H.core_hamiltonian() == corr);
    }
}