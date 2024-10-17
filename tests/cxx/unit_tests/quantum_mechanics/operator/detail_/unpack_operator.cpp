#include "../../../catch.hpp"
#include <chemist/quantum_mechanics/operator/detail_/unpack_operator.hpp>
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

TEST_CASE("UnpackOperator") {
    using operator_pair  = typename detail_::UnpackOperator::operator_pair;
    using container_type = std::vector<operator_pair>;

    types::T_e_type T_e;
    types::V_en_type V_en;
    types::V_ee_type V_ee;
    types::V_nn_type V_nn;

    SECTION("CTors") {
        SECTION("Default") {
            detail_::UnpackOperator ops;
            REQUIRE(ops.m_terms == container_type{});
        }

        SECTION("Single operator") {
            detail_::UnpackOperator ops(T_e);
            container_type corr;
            corr.emplace_back(std::make_pair(1.0, T_e.clone()));
            compare_terms(ops.m_terms, corr);
        }

        SECTION("Operator and scalar") {
            detail_::UnpackOperator ops(T_e, 2.0);
            container_type corr;
            corr.emplace_back(std::make_pair(2.0, T_e.clone()));
            compare_terms(ops.m_terms, corr);
        }

        SECTION("Scalar and operator") {
            detail_::UnpackOperator ops(2.0, T_e);
            container_type corr;
            corr.emplace_back(std::make_pair(2.0, T_e.clone()));
            compare_terms(ops.m_terms, corr);
        }

        SECTION("Add") {
            detail_::UnpackOperator ops(T_e + V_en);
            container_type corr;
            corr.emplace_back(std::make_pair(1.0, T_e.clone()));
            corr.emplace_back(std::make_pair(1.0, V_en.clone()));
            compare_terms(ops.m_terms, corr);
        }

        // SECTION("Multiply (scalar, op)") {
        //     detail_::UnpackOperator ops(3.0 * T_e);
        //     container_type corr;
        //     corr.emplace_back(std::make_pair(3.0, T_e.clone()));
        //     compare_terms(ops.m_terms, corr);
        // }

        SECTION("Multiply (op, scalar)") {
            detail_::UnpackOperator ops(T_e * 3.0);
            container_type corr;
            corr.emplace_back(std::make_pair(3.0, T_e.clone()));
            compare_terms(ops.m_terms, corr);
        }

        SECTION("Subtract") {
            detail_::UnpackOperator ops(T_e - V_en);
            container_type corr;
            corr.emplace_back(std::make_pair(1.0, T_e.clone()));
            corr.emplace_back(std::make_pair(-1.0, V_en.clone()));
            compare_terms(ops.m_terms, corr);
        }
    }
}