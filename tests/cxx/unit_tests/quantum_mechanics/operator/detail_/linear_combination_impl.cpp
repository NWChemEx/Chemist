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

#include "../../test_qm.hpp"
#include <chemist/quantum_mechanics/operator/operator.hpp>
/* Testing Strategy.
 *
 * The LinearCombinationImpl class cna not be directly instantiated so we go
 * through a derived class (here Hamiltonian). The identity of the derived class
 * is somewhat immaterial as far as the implementations are concerned.
 *
 * The derived classes will test the ctors in the process of testing the
 * derived class's ctors.
 */

using namespace chemist::qm_operator;

namespace {

class LCVisitor : public OperatorVisitor {
public:
    LCVisitor(types::T_e_type T_e, types::V_en_type V_en, types::V_ee_type V_ee,
              types::V_nn_type V_nn) :
      m_T_e_(T_e), m_V_en_(V_en), m_V_ee_(V_ee), m_V_nn_(V_nn) {}

    void run(const types::T_e_type& T) { REQUIRE(T == m_T_e_); }
    void run(const types::V_en_type& T) { REQUIRE(T == m_V_en_); }
    void run(const types::V_ee_type& T) { REQUIRE(T == m_V_ee_); }
    void run(const types::V_nn_type& T) { REQUIRE(T == m_V_nn_); }

private:
    types::T_e_type m_T_e_;
    types::V_en_type m_V_en_;
    types::V_ee_type m_V_ee_;
    types::V_nn_type m_V_nn_;
};

} // namespace

TEST_CASE("LinearCombinationImpl") {
    types::T_e_type T_e;
    types::V_en_type V_en;
    types::V_ee_type V_ee;
    types::V_nn_type V_nn;

    Hamiltonian defaulted;
    Hamiltonian H(T_e + V_en + V_ee + V_nn);

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(H.size() == 4);
    }

    SECTION("coefficient() const") {
        REQUIRE_THROWS_AS(defaulted.coefficient(0), std::out_of_range);

        REQUIRE(H.coefficient(0) == 1.0);
        REQUIRE(H.coefficient(1) == 1.0);
        REQUIRE(H.coefficient(2) == 1.0);
        REQUIRE(H.coefficient(3) == 1.0);
        REQUIRE_THROWS_AS(defaulted.coefficient(4), std::out_of_range);
    }

    SECTION("get_operator() const") {
        REQUIRE_THROWS_AS(std::as_const(defaulted).get_operator(0),
                          std::out_of_range);

        REQUIRE(std::as_const(H).get_operator(0).are_equal(T_e));
        REQUIRE(std::as_const(H).get_operator(1).are_equal(V_en));
        REQUIRE(std::as_const(H).get_operator(2).are_equal(V_ee));
        REQUIRE(std::as_const(H).get_operator(3).are_equal(V_nn));
        REQUIRE_THROWS_AS(std::as_const(H).get_operator(4), std::out_of_range);
    }

    SECTION("emplace_back") {
        defaulted.emplace_back(H.coefficient(0), H.get_operator(0).clone());
        defaulted.emplace_back(H.coefficient(1), H.get_operator(1).clone());
        defaulted.emplace_back(H.coefficient(2), H.get_operator(2).clone());
        defaulted.emplace_back(H.coefficient(3), H.get_operator(3).clone());
        REQUIRE(defaulted == H);
    }

    SECTION("clone") {
        REQUIRE(defaulted.clone()->are_equal(defaulted));
        REQUIRE(H.clone()->are_equal(H));
    }

    SECTION("visit()") {
        LCVisitor visitor(T_e, V_en, V_ee, V_nn);
        H.visit(visitor);
    }

    SECTION("visit() const") {
        LCVisitor visitor(T_e, V_en, V_ee, V_nn);
        std::as_const(H).visit(visitor);
    }

    SECTION("operator==") {
        SECTION("Same value") {
            REQUIRE(defaulted == Hamiltonian{});
            REQUIRE(H == Hamiltonian(T_e + V_en + V_ee + V_nn));
        }

        SECTION("Different scalars") {
            REQUIRE_FALSE(H == Hamiltonian(T_e + 2.0 * V_en + V_ee + V_nn));
        }

        SECTION("Different order") {
            REQUIRE_FALSE(H == Hamiltonian(T_e + V_en + V_nn + V_ee));
        }

        SECTION("Different terms") { REQUIRE_FALSE(defaulted == H); }
    }

    SECTION("operator!=") {
        // Implemented by negating operator==, so just spot check
        REQUIRE_FALSE(defaulted != Hamiltonian{});
        REQUIRE(defaulted != H);
    }

    SECTION("swap") {
        Hamiltonian copy_defaulted(defaulted);
        Hamiltonian copy_H(H);

        H.swap(defaulted);

        REQUIRE(copy_defaulted == H);
        REQUIRE(copy_H == defaulted);
    }
}
