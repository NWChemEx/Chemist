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

/** @brief Describes the total energy of the system.
 *
 */
class Fock : public detail_::LinearCombinationImpl<Fock> {
private:
    /// Type *this actually inherits from
    using impl_type = detail_::LinearCombinationImpl<Fock>;

    /// Type of parser base expects *this to use
    using typename impl_type::parser_type;

public:
    /// Pull in base class types
    ///@{
    using typename impl_type::base_pointer;
    using typename impl_type::visitor_reference;
    ///@}

    /** @brief Creates a Fock with no terms.
     *
     *  @throw None No throw guarantee.
     */
    Fock() = default;

    /** @brief Creates a Fock from a linear-combination of operators.
     *
     *  @tparam T The type of the operator expression.
     *
     *  This ctor will will walk the abstract syntax tree represented by
     *  @p expression and unpack it into a series of weighted terms.
     *
     *  @param[in] expression The mathematical form of the Fock.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state. Strong
     *                        throw guarantee.
     */
    template<typename T>
    explicit Fock(const utilities::dsl::Term<T>& expression) :
      impl_type(parser_type(expression.downcast()).m_terms) {}

    /** @brief Initializes *this with a deep copy of @p other.
     *
     *  @param[in] other The Fock to deep copy.
     *
     *  @throw std::bad_alloc if there is a problem deep copying @p other.
     *                        Strong throw guarantee.
     */
    Fock(const Fock& other) = default;

    /** @brief Initializes *this by taking the state of @p other.
     *
     *  @param[in,out] other The Fock whose state to take. After this
     *                        operation @p other will be in a valid, but
     *                        otherwise undefined state.
     *
     *  @throw None No throw guarantee.
     */
    Fock(Fock&& other) noexcept = default;

    /** @brief Overwrites *this with a deep copy of @p other.
     *
     *  @param[in] other The Fock to copy.
     *
     *  @return *this after overwriting its state with a copy of the state in
     *          @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    Fock& operator=(const Fock& other) = default;

    /** @brief Overwrites *this with the state in @p other.
     *
     *  @param[in,out] other The Fock to take the state of. After
     * this operation @p other will be in a valid, but otherwise undefined
     * state.
     *
     *  @throw None No throw guarantee.
     */
    Fock& operator=(Fock&& other) noexcept = default;
};
} // namespace chemist::qm_operator
