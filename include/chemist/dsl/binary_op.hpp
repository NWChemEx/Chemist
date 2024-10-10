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
#include <chemist/dsl/term.hpp>
#include <chemist/dsl/term_traits.hpp>
#include <tuple>
#include <type_traits>

namespace chemist::dsl {

/** @brief Code factorization for binary operations.
 *
 *  @tparam DerivedType the operation *this is implementing.
 *  @tparam LHSType The const-qualified type of the object on the left side of
 *                  the operation.
 *  @tparam RHSType The const-qualified type of the object on the right side of
 *                  the operation.
 *
 *  The DSL implementation of most of the binary operations are the same and is
 *  implemented by this class.
 */
template<typename DerivedType, typename LHSType, typename RHSType>
class BinaryOp : public Term<DerivedType> {
private:
    /// Works out the types associated with LHSType
    using lhs_traits = TermTraits<LHSType>;

    /// Works out the types associated with RHSType
    using rhs_traits = TermTraits<RHSType>;

public:
    /// Unqualified type of the object on the left side of the operator
    using lhs_type = typename lhs_traits::value_type;

    /// Type acting like `lhs_type&`, but respecting const-ness of @p LHSType
    using lhs_reference = typename lhs_traits::reference;

    /// Type acting like `const lhs_type&`
    using const_lhs_reference = typename lhs_traits::const_reference;

    /// Unqualified type of the object on the right side of the operator
    using rhs_type = typename rhs_traits::value_type;

    /// Type acting like `rhs_type&`, but respecting const-ness of @p RHSType
    using rhs_reference = typename rhs_traits::reference;

    /// Type acting like `const rhs_type&`.
    using const_rhs_reference = typename rhs_traits::const_reference;

    /** @brief Creates a new binary operation by aliasing @p l and @p r.
     *
     *  Generally speaking binary operations will want to alias the terms on
     *  the left and right of the operator (as opposed to copying them or taking
     *  ownership). This ctor takes references to the two objects and stores
     *  them internally as `TermTraits<T>::holder_type` objects (where T is
     *  @p LHSType and @p RHSType respectively for @p lhs and @p rhs). Thus
     *  whether *this ultimately owns the objects referenced by @p lhs and
     *  @p rhs are controlled by the respective specializations of `TermTraits`.
     *
     *  @param[in] l An alias to the object on the left side of the operator.
     *  @param[in] r An alias to the object on the right side of the operator.
     *
     *  @throw ??? Throws if converting either @p l or @p r to the holder type
     *             throws. Same throw guarantee.
     */
    BinaryOp(lhs_reference l, rhs_reference r) : m_lhs_(l), m_rhs_(r) {}

    // -------------------------------------------------------------------------
    // -- Getters and setters
    // -------------------------------------------------------------------------

    /** @brief Returns a (possibly) mutable reference to the object on the left
     *         of the operator.
     *
     *  *this is associated with two objects. The one that was on the left side
     *  of the operator is termed "lhs" and can be accessed via this method.
     *
     *  @return A (possibly) mutable reference to the object which was on the
     *          left of the operator. The mutable-ness of the return is
     *          controlled by TermTraits<LHSType>.
     *
     *  @throw ??? Throws if converting from the held type to lhs_reference
     *             throws. Same throw guarantee.
     */
    lhs_reference lhs() { return m_lhs_; }

    /** @brief Returns a read-only reference to the object on the left of the
     *         operator.
     *
     *  This method is identical to the non-const version except that the return
     *  is guaranteed to be read-only.
     *
     *  @return A read-only reference to the object on the left of the
     *          operator.
     *
     *  @throw ??? Throws if converting from the held type to
     *             const_lhs_reference throws. Same throw guarantee.
     */
    const_lhs_reference lhs() const { return m_lhs_; }

    /** @brief Returns a (possibly) mutable reference to the object on the right
     *         of the operator.
     *
     *  *this is associated with two objects. The one that was on the right side
     *  of the operator is termed "rhs" and can be accessed via this method.
     *
     *  @return A (possibly) mutable reference to the object which was on the
     *          right of the operator. The mutable-ness of the return is
     *          controlled by TermTraits<RHSType>.
     *
     *  @throw ??? Throws if converting from the held type to rhs_reference
     *             throws. Same throw guarantee.
     */
    rhs_reference rhs() { return m_rhs_; }

    /** @brief Returns a read-only reference to the object on the right of the
     *         operator.
     *
     *  This method is identical to the non-const version except that the return
     *  is guaranteed to be read-only.
     *
     *  @return A read-only reference to the object on the right of the
     *          operator.
     *
     *  @throw ??? Throws if converting from the held type to
     *             const_rhs_reference throws. Same throw guarantee.
     */
    const_rhs_reference rhs() const { return m_rhs_; }

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Is *this the same binary op as @p other?
     *
     *  @tparam DerivedType2 The type @p other implements.
     *  @tparam LHSType2 The type of lhs in @p other.
     *  @tparam RHSType2 The type of rhs in @p other.
     *
     *  Two BinaryOp objects are the same if they:
     *  - Implement the same operation, e.g., both are implementing addition,
     *  - Both have the same value of lhs, and
     *  - Both have the same value of rhs.
     *
     *  It should be noted that following C++ convention, value comparisons are
     *  done with const references and thus the const-ness of @tparam LHSType
     *  and @tparam RHSType vs the respective const-ness of @tparam LHSType2
     *  and @tparam RHSType2 is not considered.
     *
     *  @param[in] other The object to compare to.
     *
     *  @return True if *this is value equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename DerivedType2, typename LHSType2, typename RHSType2>
    bool operator==(
      const BinaryOp<DerivedType2, LHSType2, RHSType2>& other) const noexcept;

    /** @brief Is *this different than @p other?
     *
     *  @tparam DerivedType2 The type @p other implements.
     *  @tparam LHSType2 The type of lhs in @p other.
     *  @tparam RHSType2 The type of rhs in @p other.
     *
     *  This method defines "different" as not value equal. See the description
     *  for operator== for the definition of value equal.
     *
     *  @param[in] other The object to compare to *this.
     *
     *  @return False if *this is value equal to @p other and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename DerivedType2, typename LHSType2, typename RHSType2>
    bool operator!=(
      const BinaryOp<DerivedType2, LHSType2, RHSType2>& other) const noexcept {
        return !((*this) == other);
    }

private:
    /// The object on the left side of the operator
    typename lhs_traits::holder_type m_lhs_;

    /// The object on the right side of the operator
    typename rhs_traits::holder_type m_rhs_;
};

// -----------------------------------------------------------------------------
// -- Out of line inline definitions
// -----------------------------------------------------------------------------

template<typename DerivedType, typename LHSType, typename RHSType>
template<typename DerivedType2, typename LHSType2, typename RHSType2>
bool BinaryOp<DerivedType, LHSType, RHSType>::operator==(
  const BinaryOp<DerivedType2, LHSType2, RHSType2>& other) const noexcept {
    using lhs2_type       = typename TermTraits<LHSType2>::value_type;
    using rhs2_type       = typename TermTraits<RHSType2>::value_type;
    constexpr auto l_same = std::is_same_v<lhs_type, lhs2_type>;
    constexpr auto r_same = std::is_same_v<rhs_type, rhs2_type>;
    if constexpr(l_same && r_same) {
        return std::tie(lhs(), rhs()) == std::tie(other.lhs(), other.rhs());
    } else {
        return false;
    }
}

} // namespace chemist::dsl