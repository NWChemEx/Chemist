#pragma once
#include <chemist/dsl/dsl.hpp>
#include <chemist/quantum_mechanics/operator/detail_/linear_combination_impl.hpp>
#include <chemist/quantum_mechanics/operator/electronic_hamiltonian.hpp>

namespace chemist::qm_operator {

/** @brief Describes the total energy of the system.
 *
 *  The Hamiltonian is (in general) a many-particle operator describing an
 *  indeterminate number of particles. Tensor elements of the Hamiltonian
 *  describe the total energy of the system in a particular state.
 *
 *  @note At present the definition of which terms are in the electronic
 *        Hamiltonian and the core Hamiltonian are determined by the
 *        implementation. If necessary an overload taking a visitor could be
 *        added to override this.
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

    /// Type of a Hamiltonian-like operator describing the electrons
    using electronic_hamiltonian_type = ElectronicHamiltonian;

    /// Read-only reference to an object of type electronic_hamiltonian_type
    using const_electronic_hamiltonian_reference = electronic_hamiltonian_type;

    /// Type of a Hamiltonian-like operator comprised of one-electron terms.
    using core_hamiltonian_type = CoreHamiltonian;

    /// Type of a read-only reference to an object of type core_hamiltonian_type
    using const_core_hamiltonian_reference = core_hamiltonian_type;

    /** @brief Creates a Hamiltonian with no terms.
     *
     *  @throw None No throw guarantee.
     */
    Hamiltonian() = default;

    /** @brief Creates a Hamiltonian from a linear-combination of operators.
     *
     *  @tparam T The type of the operator expression.
     *
     *  This ctor will will walk the abstract syntax tree represented by
     *  @p expression and unpack it into a series of weighted terms.
     *
     *  @param[in] expression The mathematical form of the Hamiltonian.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state. Strong
     *                        throw guarantee.
     */
    template<typename T>
    explicit Hamiltonian(const dsl::Term<T>& expression) :
      impl_type(parser_type(expression.downcast()).m_terms) {}

    /** @brief Returns the subset of *this which depends on the electrons.
     *
     *  The Hamiltonian contains terms that depend on all particles. In
     *  general, some of these terms will not depend on the electrons. The
     *  terms which do depend on the electrons form what is called the
     *  electronic hamiltonian. This method retrieves those pieces.
     *
     *  @return The subset of *this comprising the electronic Hamiltonian.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Strong throw guarantee.
     */
    const_electronic_hamiltonian_reference electronic_hamiltonian() const;

    /** @brief Returns the subset of *this forming the CoreHamiltonian.
     *
     *  The Hamiltonian in general contains one and two electron
     *  terms. The core Hamiltonian are the one-electron pieces of the
     *  Hamiltonian.
     *
     *  @return The subset of terms in *this comprising the CoreHamiltonian.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Strong throw guarantee.
     */
    const_core_hamiltonian_reference core_hamiltonian() const;

    /** @brief Initializes *this with a deep copy of @p other.
     *
     *  @param[in] other The Hamiltonian to deep copy.
     *
     *  @throw std::bad_alloc if there is a problem deep copying @p other.
     *                        Strong throw guarantee.
     */
    Hamiltonian(const Hamiltonian& other) = default;

    /** @brief Initializes *this by taking the state of @p other.
     *
     *  @param[in,out] other The Hamiltonian whose state to take. After this
     *                        operation @p other will be in a valid, but
     *                        otherwise undefined state.
     *
     *  @throw None No throw guarantee.
     */
    Hamiltonian(Hamiltonian&& other) noexcept = default;

    /** @brief Overwrites *this with a deep copy of @p other.
     *
     *  @param[in] other The Hamiltonian to copy.
     *
     *  @return *this after overwriting its state with a copy of the state in
     *          @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    Hamiltonian& operator=(const Hamiltonian& other) = default;

    /** @brief Overwrites *this with the state in @p other.
     *
     *  @param[in,out] other The Hamiltonian to take the state of. After this
     *                       operation @p other will be in a valid, but
     *                       otherwise undefined state.
     *
     *  @throw None No throw guarantee.
     */
    Hamiltonian& operator=(Hamiltonian&& other) noexcept = default;
};
} // namespace chemist::qm_operator