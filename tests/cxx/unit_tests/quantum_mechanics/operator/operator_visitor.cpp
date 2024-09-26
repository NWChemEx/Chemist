/*
 * Copyright 2024 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../../catch.hpp"
#include <chemist/quantum_mechanics/operator/kinetic.hpp>
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>

using namespace chemist;
using namespace chemist::qm_operator;

/* Testing strategy.
 *
 * Here we define one visitor that overrides both Kinetic<Electron> and
 * const Kinetic<Electron>, but only Kinetic<ManyElectron>. We then pass that
 * visitor to mutable and read-only verssions of two operators:
 * - Kinetic<Electron>
 * - Kinetic<ManyElectrons>
 *
 * and one visitor that only overrides the const version of Kinetic<Electron>
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

class TestVisitor2 : public OperatorVisitor {
public:
    TestVisitor2(Kinetic<Electron>& t) : m_t_(t) {}

    void run(const Kinetic<Electron>& t) { REQUIRE(t == m_t_); }

private:
    Kinetic<Electron> m_t_;
};

TEST_CASE("OperatorVisitor") {
    Kinetic<Electron> t;
    Kinetic<ManyElectrons> T(ManyElectrons{3});

    OperatorBase& t_base             = t;
    OperatorBase& T_base             = T;
    const OperatorBase& const_t_base = t;
    const OperatorBase& const_T_base = T;

    TestVisitor visitor(t, T);
    TestVisitor2 visitor2(t);

    SECTION("Mutable operator") {
        SECTION("Visitor supports mutable and read-only defined") {
            t_base.visit(visitor);
        }

        SECTION("Visitor supports mutable, but not read-only") {
            T_base.visit(visitor);
        }

        SECTION("Visitor supports read-only, but not mutable") {
            t_base.visit(visitor2);
        }

        SECTION("Visitor doesn't support either mutable or read-only") {
            REQUIRE_THROWS_AS(T_base.visit(visitor2), std::runtime_error);
        }
    }

    SECTION("Read-only operator") {
        SECTION("Visitor supports mutable and read-only defined") {
            const_t_base.visit(visitor);
        }

        SECTION("Visitor supports mutable, but not read-only") {
            REQUIRE_THROWS_AS(const_T_base.visit(visitor), std::runtime_error);
        }

        SECTION("Visitor supports read-only, but not mutable") {
            const_t_base.visit(visitor2);
        }

        SECTION("Visitor doesn't support either mutable or read-only") {
            REQUIRE_THROWS_AS(const_T_base.visit(visitor2), std::runtime_error);
        }
    }
}
