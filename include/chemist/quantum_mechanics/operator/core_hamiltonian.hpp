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
#include <chemist/quantum_mechanics/operator/detail_/linear_combination_impl.hpp>
#include <utilities/dsl/dsl.hpp>

namespace chemist::qm_operator {

/** @brief A one-electron Hamiltonian
 *
 *  The Hamiltonian will in general contain terms which depend on one or two
 *  electrons. We can analytically solve the Schrodinger equation for terms
 *  that only depend on one-electron (assuming the electrons do not interact
 *  quantum-mechanically with the nuclei) and so it is often useful to partition
 *  the Hamiltonian into a "core Hamiltonian" where the core Hamiltonian is the
 *  subst of terms that depend on only one electron.
 *
 */
class CoreHamiltonian : public detail_::LinearCombinationImpl<CoreHamiltonian> {
private:
    /// Type *this actually inherits from
    using impl_type = detail_::LinearCombinationImpl<CoreHamiltonian>;

    /// Type of parser base expects *this to use
    using typename impl_type::parser_type;

public:
    /// Pull in base class types
    ///@{
    using typename impl_type::base_pointer;
    using typename impl_type::container_type;
    using typename impl_type::visitor_reference;
    ///@}

    /** @brief Creates a CoreHamiltonian with no terms.
     *
     *  @throw None No throw guarantee.
     */
    CoreHamiltonian() = default;

    explicit CoreHamiltonian(container_type linear_combination) :
      impl_type(std::move(linear_combination)) {}

    /** @brief Creates a CoreHamiltonian from a linear-combination of operators.
     *
     *  @tparam T The type of the operator expression.
     *
     *  This ctor will will walk the abstract syntax tree represented by
     *  @p expression and unpack it into a series of weighted terms.
     *
     *  @param[in] expression The mathematical form of the CoreHamiltonian.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state. Strong
     *                        throw guarantee.
     */
    template<typename T>
    explicit CoreHamiltonian(const utilities::dsl::Term<T>& expression) :
      impl_type(parser_type(expression.downcast()).m_terms) {}

    /** @brief Initializes *this with a deep copy of @p other.
     *
     *  @param[in] other The CoreHamiltonian to deep copy.
     *
     *  @throw std::bad_alloc if there is a problem deep copying @p other.
     *                        Strong throw guarantee.
     */
    CoreHamiltonian(const CoreHamiltonian& other) = default;

    /** @brief Initializes *this by taking the state of @p other.
     *
     *  @param[in,out] other The CoreHamiltonian whose state to take. After this
     *                        operation @p other will be in a valid, but
     *                        otherwise undefined state.
     *
     *  @throw None No throw guarantee.
     */
    CoreHamiltonian(CoreHamiltonian&& other) noexcept = default;

    /** @brief Overwrites *this with a deep copy of @p other.
     *
     *  @param[in] other The CoreHamiltonian to copy.
     *
     *  @return *this after overwriting its state with a copy of the state in
     *          @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    CoreHamiltonian& operator=(const CoreHamiltonian& other) = default;

    /** @brief Overwrites *this with the state in @p other.
     *
     *  @param[in,out] other The CoreHamiltonian to take the state of. After
     * this operation @p other will be in a valid, but otherwise undefined
     * state.
     *
     *  @throw None No throw guarantee.
     */
    CoreHamiltonian& operator=(CoreHamiltonian&& other) noexcept = default;
};
} // namespace chemist::qm_operator
