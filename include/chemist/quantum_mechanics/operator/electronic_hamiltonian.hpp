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

#pragma once
#include <chemist/dsl/dsl.hpp>
#include <chemist/quantum_mechanics/operator/core_hamiltonian.hpp>
#include <chemist/quantum_mechanics/operator/detail_/linear_combination_impl.hpp>

namespace chemist::qm_operator {

/** @brief The subset of the Hamiltonian terms which depend on electrons.
 *
 *  In the electronic structure problem we are focused on Hamiltonian terms
 *  which depend on the electrons. The "electronic" Hamiltonian is the subset
 *  of the full Hamiltonian containing only terms that involve electrons.
 */
class ElectronicHamiltonian
  : public detail_::LinearCombinationImpl<ElectronicHamiltonian> {
private:
    /// Type *this actually inherits from
    using impl_type = detail_::LinearCombinationImpl<ElectronicHamiltonian>;

    /// Type of parser base expects *this to use
    using typename impl_type::parser_type;

public:
    /// Pull in base class types
    ///@{
    using typename impl_type::base_pointer;
    using typename impl_type::container_type;
    using typename impl_type::visitor_reference;
    ///@}

    /// Type representing the core Hamiltonian
    using core_hamiltonian_type = CoreHamiltonian;

    /// Type of a read-only reference to an object of type core_hamiltonian_type
    using const_core_hamiltonian_reference = core_hamiltonian_type;

    /** @brief CreatesElectronic with no terms.
     *
     *  @throw None No throw guarantee.
     */
    ElectronicHamiltonian() = default;

    explicit ElectronicHamiltonian(container_type linear_combination) :
      impl_type(std::move(linear_combination)) {}

    /** @brief CreatesElectronic from a linear-combination of operators.
     *
     *  @tparam T The type of the operator expression.
     *
     *  This ctor will will walk the abstract syntax tree represented by
     *  @p expression and unpack it into a series of weighted terms.
     *
     *  @param[in] expression The mathematical form of tElectronic.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state.
     * Strong throw guarantee.
     */
    template<typename T>
    explicit ElectronicHamiltonian(const dsl::Term<T>& expression) :
      impl_type(parser_type(expression.downcast()).m_terms) {}

    /** @brief Initializes *this with a deep copy of @p other.
     *
     *  @param[in] other TElectronic to deep copy.
     *
     *  @throw std::bad_alloc if there is a problem deep copying @p other.
     *                        Strong throw guarantee.
     */
    ElectronicHamiltonian(const ElectronicHamiltonian& other) = default;

    /** @brief Initializes *this by taking the state of @p other.
     *
     *  @param[in,out] other TElectronic whose state to take. After this
     *                        operation @p other will be in a valid, but
     *                        otherwise undefined state.
     *
     *  @throw None No throw guarantee.
     */
    ElectronicHamiltonian(ElectronicHamiltonian&& other) noexcept = default;

    /** @brief Returns the subset of *this forming the CoreHamiltonian.
     *
     *  The electronic Hamiltonian in general contains one and two electron
     *  terms. The core Hamiltonian are the one-electron pieces of the
     *  Hamiltonian.
     *
     *  @return The subset of terms in *this comprising the CoreHamiltonian.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Strong throw guarantee.
     */
    const_core_hamiltonian_reference core_hamiltonian() const;

    /** @brief Overwrites *this with a deep copy of @p other.
     *
     *  @param[in] other TElectronic to copy.
     *
     *  @return *this after overwriting its state with a copy of the state in
     *          @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    ElectronicHamiltonian& operator=(const ElectronicHamiltonian& other) =
      default;

    /** @brief Overwrites *this with the state in @p other.
     *
     *  @param[in,out] other TElectronic to take the state of. After
     * this operation @p other will be in a valid, but otherwise undefined
     * state.
     *
     *  @throw None No throw guarantee.
     */
    ElectronicHamiltonian& operator=(ElectronicHamiltonian&& other) noexcept =
      default;
};
} // namespace chemist::qm_operator