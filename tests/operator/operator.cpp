#include "libchemist/operator/coulomb.hpp"
#include "libchemist/operator/exchange.hpp"
#include "libchemist/operator/exchange_correlation.hpp"
#include "libchemist/operator/kinetic.hpp"
#include "libchemist/operator/operator_base.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace libchemist;

template<typename OpType, std::size_t RefNE, std::size_t RefNN>
void check_operator_particle_count() {
    STATIC_REQUIRE(OpType::n_electrons == RefNE);
    STATIC_REQUIRE(OpType::n_nuclei == RefNN);
}

template<typename OpType>
void check_stateless_operator_equality() {
    OpType lhs, rhs;
    auto& lhs_as_operator = static_cast<const OperatorBase&>(lhs);
    auto& rhs_as_operator = static_cast<const OperatorBase&>(rhs);
    REQUIRE(lhs == rhs);
    REQUIRE(lhs_as_operator == rhs_as_operator); // Trivially true
    REQUIRE(lhs_as_operator.is_equal(rhs));
    REQUIRE(lhs_as_operator.is_equal(rhs_as_operator));
    REQUIRE(lhs.is_equal(rhs));
    REQUIRE(lhs.is_equal(rhs_as_operator));
}

TEST_CASE("Operator Sanity") {
    SECTION("Particle Count") {
        check_operator_particle_count<ElectronKinetic, 1, 0>();
        check_operator_particle_count<ElectronNuclearCoulomb, 1, 1>();
        check_operator_particle_count<ElectronElectronCoulomb, 2, 0>();

        check_operator_particle_count<ElectronEDensityCoulomb, 1, 0>();
        check_operator_particle_count<ElectronEDensityExchange, 1, 0>();
        check_operator_particle_count<MeanFieldKohnShamExchangeCorrelation, 1,
                                      0>();

        // check_operator_particle_count< ClassicalElectronDipoleField,     1,
        // 0>(); check_operator_particle_count<
        // ClassicalElectronQuadrupoleField, 1, 0>();
        // check_operator_particle_count< ClassicalElectronOctupoleField,   1,
        // 0>();
    }

    SECTION("Stateless Operator Comparison") {
        check_stateless_operator_equality<ElectronKinetic>();
        check_stateless_operator_equality<ElectronElectronCoulomb>();
        check_stateless_operator_equality<ElectronEDensityCoulomb>();
        check_stateless_operator_equality<ElectronEDensityExchange>();
        check_stateless_operator_equality<
          MeanFieldKohnShamExchangeCorrelation>();
    }

    SECTION("Electron-Nuclear Coulomb Comparison") {
        Molecule a{Atom(1ul, std::array<double, 3>{0.0, 0.0, 0.0}),
                   Atom(1ul, std::array<double, 3>{0.0, 0.0, 2.0})};
        Molecule b{Atom(2ul, std::array<double, 3>{0.0, 0.0, 0.0}),
                   Atom(2ul, std::array<double, 3>{0.0, 0.0, 2.0})};
        ElectronNuclearCoulomb lhs(Electron{}, a), rhs(Electron{}, b);

        REQUIRE(lhs == lhs);
        REQUIRE(rhs == rhs);
        REQUIRE(lhs != rhs);

        const auto& lhs_as_operator = static_cast<const OperatorBase&>(lhs);
        const auto& rhs_as_operator = static_cast<const OperatorBase&>(rhs);

        // These are trivial
        REQUIRE(lhs_as_operator == lhs_as_operator);
        REQUIRE(rhs_as_operator == rhs_as_operator);
        REQUIRE(lhs_as_operator == rhs_as_operator);

        // These are not
        REQUIRE(lhs.is_equal(lhs));
        REQUIRE(lhs_as_operator.is_equal(lhs));
        REQUIRE(lhs_as_operator.is_equal(lhs_as_operator));
        REQUIRE(lhs.is_equal(lhs_as_operator));

        REQUIRE(rhs.is_equal(rhs));
        REQUIRE(rhs_as_operator.is_equal(rhs));
        REQUIRE(rhs_as_operator.is_equal(rhs_as_operator));
        REQUIRE(rhs.is_equal(rhs_as_operator));

        REQUIRE(!lhs.is_equal(rhs));
        REQUIRE(!lhs_as_operator.is_equal(rhs));
        REQUIRE(!lhs_as_operator.is_equal(rhs_as_operator));
        REQUIRE(!lhs.is_equal(rhs_as_operator));

        REQUIRE(!rhs.is_equal(lhs));
        REQUIRE(!rhs_as_operator.is_equal(lhs));
        REQUIRE(!rhs_as_operator.is_equal(lhs_as_operator));
        REQUIRE(!rhs.is_equal(lhs_as_operator));
    }
}
