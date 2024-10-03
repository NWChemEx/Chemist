#include "../../test_helpers.hpp"
#include "../test_qm.hpp"
#include <chemist/quantum_mechanics/braket/braket_class.hpp>
#include <chemist/quantum_mechanics/operator/kinetic.hpp>
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>

using namespace chemist;
using namespace chemist::braket;

TEST_CASE("bra_ket_base_type") {
    using vector_space_type = wavefunction::AOs;
    using op_type           = qm_operator::Kinetic<Electron>;

    using tr_type =
      chemist::braket::detail_::bra_ket_base_type<vector_space_type, op_type,
                                                  vector_space_type>;

    STATIC_REQUIRE(std::is_same_v<TensorRepresentation, tr_type>);
}

TEST_CASE("BraKet<AOs, Kinetic<Electron>, AOs>") {
    using operator_type = qm_operator::Kinetic<Electron>;
    using bra_type      = wavefunction::AOs;
    using ket_type      = wavefunction::AOs;

    operator_type t_e;
    bra_type h2_aos(test_chemist::h2_basis());
    ket_type no_aos;
    BraKet t_mn(h2_aos, t_e, no_aos);

    SECTION("Ctors/assignment") {
        SECTION("value") {
            REQUIRE(t_mn.bra() == h2_aos);
            REQUIRE(t_mn.the_operator() == t_e);
            REQUIRE(t_mn.ket() == no_aos);
        }
        SECTION("copy") {
            BraKet copy_t_mn(t_mn);
            REQUIRE(copy_t_mn == t_mn);
        }
        SECTION("move") {
            BraKet copy_t_mn(t_mn);
            BraKet move_t_mn(std::move(t_mn));
            REQUIRE(copy_t_mn == move_t_mn);
        }
        SECTION("copy assignment") {
            BraKet lhs(no_aos, t_e, no_aos);
            auto plhs = &(lhs = t_mn);
            REQUIRE(lhs == t_mn);
            REQUIRE(plhs == &lhs);
        }
        SECTION("move assignment") {
            BraKet lhs(no_aos, t_e, no_aos);
            BraKet copy_t_mn(t_mn);
            auto plhs = &(lhs = std::move(t_mn));
            REQUIRE(lhs == copy_t_mn);
            REQUIRE(plhs == &lhs);
        }
    }

    SECTION("operator==") {
        SECTION("Same value") {
            BraKet rhs(h2_aos, t_e, no_aos);
            REQUIRE(t_mn == rhs);
        }
        SECTION("Bra and ket flipped") {
            BraKet rhs(no_aos, t_e, h2_aos);
            REQUIRE_FALSE(t_mn == rhs);
        }
        SECTION("Different bra") {
            BraKet rhs(no_aos, t_e, no_aos);
            REQUIRE_FALSE(t_mn == rhs);
        }
        SECTION("Different operator") {
            qm_operator::Kinetic<ManyElectrons> T_e;
            BraKet rhs(h2_aos, T_e, no_aos);
            REQUIRE_FALSE(t_mn == rhs);
        }
        SECTION("Different ket") {
            BraKet rhs(h2_aos, t_e, h2_aos);
            REQUIRE_FALSE(t_mn == rhs);
        }
    }

    SECTION("operator!=") {
        // N.b. implemented in terms of operator== just spot check
        BraKet same(h2_aos, t_e, no_aos);
        REQUIRE_FALSE(t_mn != same);

        BraKet diff(no_aos, t_e, h2_aos);
        REQUIRE(t_mn != diff);
    }
}