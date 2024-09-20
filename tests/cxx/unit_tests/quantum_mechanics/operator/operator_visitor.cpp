#include "../../catch.hpp"
#include <chemist/quantum_mechanics/operator/kinetic.hpp>
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>

using namespace chemist;
using namespace chemist::qm_operator;

/* Testing strategy.
 *
 * Here we define a visitor that overrides both Kinetic<Electron> and
 * const Kinetic<Electron>, but only Kinetic<ManyElectron>. We then pass that
 * visitor to mutable and read-only verssions of two operators:
 * - Kinetic<Electron>
 * - Kinetic<ManyElectrons>
 */

class TestVisitor : public OperatorVisitor {
public:
    TestVisitor(Kinetic<Electron>& t, Kinetic<ManyElectrons>& T) :
      m_t_(t), m_T_(T) {}

    void run(Kinetic<Electron>& t) { REQUIRE(t == m_t_); }
    void run(const Kinetic<Electron>& t) { REQUIRE(t == m_t_); }
    void run(Kinetic<ManyElectrons>& T) { REQUIRE(T == m_T_); }

private:
    Kinetic<Electron> m_t_;
    Kinetic<ManyElectrons> m_T_;
};

TEST_CASE("OperatorVisitor") {
    Kinetic<Electron> t;
    Kinetic<ManyElectrons> T(ManyElectrons{3});

    OperatorBase& t_base             = t;
    OperatorBase& T_base             = T;
    const OperatorBase& const_t_base = t;
    const OperatorBase& const_T_base = T;

    TestVisitor visitor(t, T);

    // Should dispatch to the mutable version in the derived class
    SECTION("Mutable defined") {
        t_base.visit(visitor);
        T_base.visit(visitor);
    }

    SECTION("Read-only defined") { const_t_base.visit(visitor); }

    SECTION("Read-only not defined") {
        REQUIRE_THROWS_AS(const_T_base.visit(visitor), std::runtime_error);
    }
}