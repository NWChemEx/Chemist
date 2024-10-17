#pragma once
#include <chemist/dsl/dsl.hpp>
#include <chemist/quantum_mechanics/operator/operator_base.hpp>
#include <tuple>
#include <vector>

namespace chemist::qm_operator::detail_ {

struct UnpackOperator {
    using operator_base_type       = OperatorBase;
    using const_operator_reference = const operator_base_type&;
    using operator_pointer         = typename operator_base_type::base_pointer;
    using scalar_type              = double;
    using operator_pair            = std::pair<scalar_type, operator_pointer>;

    template<typename T>
    using enable_if_is_floating_point_t =
      std::enable_if_t<std::is_floating_point_v<T>>;

    UnpackOperator() noexcept = default;

    /// Leaf handlers
    ///@{
    explicit UnpackOperator(const_operator_reference op) {
        m_terms.emplace_back(std::make_pair(1.0, op.clone()));
    }

    template<typename T, typename = enable_if_is_floating_point_t<T>>
    UnpackOperator(const_operator_reference op, T scalar) {
        m_terms.emplace_back(std::make_pair(scalar, op.clone()));
    }

    template<typename T, typename = enable_if_is_floating_point_t<T>>
    UnpackOperator(T scalar, const_operator_reference op) {
        m_terms.emplace_back(std::make_pair(scalar, op.clone()));
    }
    ///@}

    template<typename LHSType, typename RHSType>
    UnpackOperator(const dsl::Add<LHSType, RHSType>& op) :
      UnpackOperator(op.lhs()) {
        unpack_into_mterms(1.0, op.rhs());
    }

    template<typename LHSType, typename RHSType>
    UnpackOperator(const dsl::Multiply<LHSType, RHSType>& op) :
      UnpackOperator(op.lhs(), op.rhs()) {}

    template<typename LHSType, typename RHSType>
    UnpackOperator(const dsl::Subtract<LHSType, RHSType>& op) :
      UnpackOperator(op.lhs()) {
        unpack_into_mterms(-1.0, op.rhs());
    }

    template<typename T>
    void unpack_into_mterms(double scale, T& rhs) {
        for(auto& [c, o] : UnpackOperator(rhs).m_terms) {
            m_terms.emplace_back(std::make_pair(scale * c, std::move(o)));
        }
    }

    std::vector<operator_pair> m_terms;
};

} // namespace chemist::qm_operator::detail_