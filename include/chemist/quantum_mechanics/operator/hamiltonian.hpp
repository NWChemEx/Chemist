#pragma once
#include <chemist/quantum_mechanics/operator/detail_/operator_impl.hpp>

namespace chemist::qm_operator {
namespace detail_ {
class HamiltonianPIMPL;
}

/** @brief Describes the total energy of the system.
 *
 */
// class Hamiltonian : public OperatorBase {
// public:
//     /// Pull in base class types
//     ///@{
//     using OperatorBase::base_pointer;
//     using OperatorBase::visitor_reference;
//     ///@}

//     using pimpl_type    = detail_::HamiltonianPIMPL;
//     using pimpl_pointer = std::unique_ptr<pimpl_type>;

// protected:
//     base_pointer clone_() const override;
//     void visit_(visitor_reference visitor) override;
//     void visit_(visitor_reference visitor) const override;
// };
} // namespace chemist::qm_operator