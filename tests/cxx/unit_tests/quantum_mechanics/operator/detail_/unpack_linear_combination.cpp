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

#include "../../../catch.hpp"
#include <chemist/quantum_mechanics/operator/detail_/unpack_linear_combination.hpp>
#include <chemist/quantum_mechanics/operator/operator.hpp>

using namespace chemist::qm_operator;

namespace {

template<typename LHSType, typename RHSType>
void compare_terms(LHSType& lhs, RHSType& rhs) {
    REQUIRE(lhs.size() == rhs.size());

    for(decltype(lhs.size()) i = 0; i < lhs.size(); ++i) {
        const auto& [c_lhs, op_lhs] = lhs.at(i);
        const auto& [c_rhs, op_rhs] = rhs.at(i);
        REQUIRE(c_lhs == c_rhs);
        REQUIRE(op_lhs->are_equal(*op_rhs));
    }
}

} // namespace

TEST_CASE("UnpackLinearCombination") {
    using container_type =
      typename detail_::UnpackLinearCombination::container_type;

    types::T_e_type T_e;
    types::V_en_type V_en;
    types::V_ee_type V_ee;
    types::V_nn_type V_nn;

    SECTION("CTors") {
        SECTION("Default") {
            detail_::UnpackLinearCombination ops;
            REQUIRE(ops.m_terms == container_type{});
        }

        SECTION("Single operator") {
            detail_::UnpackLinearCombination ops(T_e);
            container_type corr;
            corr.emplace_back(std::make_pair(1.0, T_e.clone()));
            compare_terms(ops.m_terms, corr);
        }

        SECTION("Operator and scalar") {
            detail_::UnpackLinearCombination ops(T_e, 2.0);
            container_type corr;
            corr.emplace_back(std::make_pair(2.0, T_e.clone()));
            compare_terms(ops.m_terms, corr);
        }

        SECTION("Scalar and operator") {
            detail_::UnpackLinearCombination ops(2.0, T_e);
            container_type corr;
            corr.emplace_back(std::make_pair(2.0, T_e.clone()));
            compare_terms(ops.m_terms, corr);
        }

        SECTION("Add") {
            detail_::UnpackLinearCombination ops(T_e + V_en);
            container_type corr;
            corr.emplace_back(std::make_pair(1.0, T_e.clone()));
            corr.emplace_back(std::make_pair(1.0, V_en.clone()));
            compare_terms(ops.m_terms, corr);
        }

        SECTION("Multiply (scalar, op)") {
            detail_::UnpackLinearCombination ops(3.0 * T_e);
            container_type corr;
            corr.emplace_back(std::make_pair(3.0, T_e.clone()));
            compare_terms(ops.m_terms, corr);
        }

        SECTION("Multiply (op, scalar)") {
            detail_::UnpackLinearCombination ops(T_e * 3.0);
            container_type corr;
            corr.emplace_back(std::make_pair(3.0, T_e.clone()));
            compare_terms(ops.m_terms, corr);
        }

        SECTION("Subtract") {
            detail_::UnpackLinearCombination ops(T_e - V_en);
            container_type corr;
            corr.emplace_back(std::make_pair(1.0, T_e.clone()));
            corr.emplace_back(std::make_pair(-1.0, V_en.clone()));
            compare_terms(ops.m_terms, corr);
        }
    }

    SECTION("unpack_into_mterms") {
        detail_::UnpackLinearCombination ops(T_e - V_en);
        ops.unpack_into_mterms(2.0, 4.0 * V_ee);
        container_type corr;
        corr.emplace_back(std::make_pair(1.0, T_e.clone()));
        corr.emplace_back(std::make_pair(-1.0, V_en.clone()));
        corr.emplace_back(std::make_pair(8.0, V_ee.clone()));
        compare_terms(ops.m_terms, corr);
    }

    SECTION("Works with the actual Hamiltonian") {
        detail_::UnpackLinearCombination ops(T_e + V_en + V_ee + V_nn);
        container_type corr;
        corr.emplace_back(std::make_pair(1.0, T_e.clone()));
        corr.emplace_back(std::make_pair(1.0, V_en.clone()));
        corr.emplace_back(std::make_pair(1.0, V_ee.clone()));
        corr.emplace_back(std::make_pair(1.0, V_nn.clone()));
        compare_terms(ops.m_terms, corr);
    }
}