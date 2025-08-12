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
#include <chemist/quantum_mechanics/operator/operator_base.hpp>
#include <tuple>
#include <utilities/dsl/dsl.hpp>
#include <vector>

namespace chemist::qm_operator::detail_ {

/** @brief Unpacks an expression involving operators.
 *
 *  When a user combines operators using the DSL it creates an abstract syntax
 *  tree. As long as the operators are a linear combination, this class can
 *  parse the AST and flatten it into an array of terms (and their
 *  coefficients).
 */
struct UnpackLinearCombination {
    /// Type of a leaf
    using leaf_type = OperatorBase;

    /// Type of a read-only reference to an object of type leaf_type
    using const_leaf_reference = const leaf_type&;

    /// Type of a pointer to an object of type leaf_type
    using leaf_pointer = typename leaf_type::base_pointer;

    /// Type of a scalar (a different leaf)
    using scalar_type = double;

    /// Type of a scalar_type, leaf_pointer pair
    using operator_pair = std::pair<scalar_type, leaf_pointer>;

    /// Type of a container filled with operator_pair objects
    using container_type = std::vector<operator_pair>;

    /// Disables a function via SFINAE if @p T is not a floating-point type
    template<typename T>
    using enable_if_is_floating_point_t =
      std::enable_if_t<std::is_floating_point_v<T>>;

    /// A parser with an empty set of terms
    UnpackLinearCombination() noexcept = default;

    /// Leaf handlers
    ///@{
    /** @brief Selected when argument is an operator.
     *
     *  This ctor ends recursion because an operator has been found. The
     *  operator is assumed to have a coefficient of +1.0 (if it had been
     *  multiplied by a different value one of the other two leaf handlers
     *  would have been selected).
     *
     *  @param[in] op The operator.
     *
     *  @throw std::bad_alloc if there is a problem copying @p op. Strong throw
     *                        guarantee.
     */
    explicit UnpackLinearCombination(const_leaf_reference op) :
      UnpackLinearCombination(1.0, op) {}

    /** @brief Selected when an operator is left multiplied by a scalar.
     *
     *  @tparam T The type of the scalar. Assumed to be a floating point type.
     *  @tparam <Anonymous> Used to disable this overload via SFINAE if @p T is
     *                      not a floating-point type.
     *
     *  @param[in] op The operator.
     *  @param[in] scalar The floating-point value scaling @p op.
     *
     *  @throw std::bad_alloc if there is a problem copying @p op. Strong throw
     *                        guarantee.
     */
    template<typename T, typename = enable_if_is_floating_point_t<T>>
    UnpackLinearCombination(const_leaf_reference op, T scalar) {
        m_terms.emplace_back(std::make_pair(scalar, op.clone()));
    }

    /** @brief Selected when an operator is right multiplied by a scalar.
     *
     *  @tparam T The type of the scalar. Assumed to be a floating point type.
     *  @tparam <Anonymous> Used to disable this overload via SFINAE if @p T is
     *                      not a floating-point type.
     *
     *  @param[in] scalar The floating-point value scaling @p op.
     *  @param[in] op The operator.
     *
     *  @throw std::bad_alloc if there is a problem copying @p op. Strong throw
     *                        guarantee.
     */
    template<typename T, typename = enable_if_is_floating_point_t<T>>
    UnpackLinearCombination(T scalar, const_leaf_reference op) {
        m_terms.emplace_back(std::make_pair(scalar, op.clone()));
    }
    ///@}

    /** @brief Handles the addition of two terms.
     *
     *  @tparam LHSType The type of the term to the left of the plus sign.
     *  @tparam RHSType The type of the term to the right of the plus sign.
     *
     *  @param[in] op The addition operation to unpack.
     *
     *  @throw ??? if unpacking the operation throws. Same through guarantee.
     */
    template<typename LHSType, typename RHSType>
    UnpackLinearCombination(const utilities::dsl::Add<LHSType, RHSType>& op) :
      UnpackLinearCombination(op.lhs()) {
        unpack_into_mterms(1.0, op.rhs());
    }

    /** @brief Handles the multiplication of two terms.
     *
     *  @tparam LHSType The type of the term to the left of the times sign.
     *  @tparam RHSType The type of the term to the right of the times sign.
     *
     *  @note The code will fail to compile if neither @p LHSType or @p RHSType
     *        is a floating point type.
     *
     *  @param[in] op The multiplication operation to unpack.
     *
     *  @throw ??? if unpacking the operation throws. Same through guarantee.
     */
    template<typename LHSType, typename RHSType>
    UnpackLinearCombination(
      const utilities::dsl::Multiply<LHSType, RHSType>& op) :
      UnpackLinearCombination(op.lhs(), op.rhs()) {}

    /** @brief Handles the subtraction of two terms.
     *
     *  @tparam LHSType The type of the term to the left of the minus sign.
     *  @tparam RHSType The type of the term to the right of the minus sign.
     *
     *  @param[in] op The subtraction operation to unpack.
     *
     *  @throw ??? if unpacking the operation throws. Same through guarantee.
     */
    template<typename LHSType, typename RHSType>
    UnpackLinearCombination(
      const utilities::dsl::Subtract<LHSType, RHSType>& op) :
      UnpackLinearCombination(op.lhs()) {
        unpack_into_mterms(-1.0, op.rhs());
    }

    /** @brief Used to unpack in place a term.
     *
     *  @tparam T The type of the term we are unpacking.
     *
     *  Our ctor overloads can only handle one term at a time. Thus when we
     *  unpack a binary operation only one of the two terms can be forwarded
     *  to a new constructor, the other term must be dealt with in the current
     *  ctor. The solution to this is simple, create a new
     * UnpackLinearCombination object to unpack the term we don't forward. This
     * method wraps the process of creating a new UnpackLinearCombination object
     * to unpack @p rhs. After unpacking
     *  @p rhs it will add it to `m_terms`. The unpacked terms will be scaled
     *  by @p scale.
     *
     *  @param[in] scale The value the unpacked terms should be scaled by. This
     *                   is usually the value @p rhs was scaled by.
     *  @param[in] rhs The term we need to unpack in place.
     *
     *  @throw ??? if unpacking @p rhs throws. Same throw guarantee.
     *  @throw std::bad_alloc if adding the unpacked terms to `m_terms` throws.
     *                        Weak throw guarantee.
     */
    template<typename T>
    void unpack_into_mterms(double scale, T&& rhs) {
        for(auto& [c, o] :
            UnpackLinearCombination(std::forward<T>(rhs)).m_terms) {
            m_terms.emplace_back(std::make_pair(scale * c, std::move(o)));
        }
    }

    /// These are the unpacked terms
    container_type m_terms;
};

} // namespace chemist::qm_operator::detail_
