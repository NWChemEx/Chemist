#pragma once
#include <chemist/dsl/dsl.hpp>
#include <chemist/quantum_mechanics/operator/detail_/linear_combination_impl.hpp>

namespace chemist::qm_operator {

/** @brief Describes the total energy of the system.
 *
 */
class Hamiltonian : public detail_::LinearCombinationImpl<Hamiltonian> {
private:
    /// Type *this actually inherits from
    using impl_type = detail_::LinearCombinationImpl<Hamiltonian>;

    /// Type of parser base expects *this to use
    using typename impl_type::parser_type;

public:
    /// Pull in base class types
    ///@{
    using typename impl_type::base_pointer;
    using typename impl_type::visitor_reference;
    ///@}

    Hamiltonian() = default;

    template<typename T>
    explicit Hamiltonian(const dsl::Term<T>& expression) :
      impl_type(parser_type(expression.downcast()).m_terms) {}

    Hamiltonian(const Hamiltonian& other)                = default;
    Hamiltonian(Hamiltonian&& other) noexcept            = default;
    Hamiltonian& operator=(const Hamiltonian& other)     = default;
    Hamiltonian& operator=(Hamiltonian&& other) noexcept = default;
};
} // namespace chemist::qm_operator