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
#include <chemist/quantum_mechanics/braket/tensor_element.hpp>
#include <chemist/quantum_mechanics/braket/tensor_representation.hpp>
#include <chemist/traits/quantum_mechanics_traits.hpp>

namespace chemist::braket {
namespace detail_ {

/** @brief Works out the base class for a given instantiation of BraKet.
 *
 *  This template variable assumes that @p BraType and @p KetType both derive
 *  from `BasisFunction` or both derive from `VectorSpace`. Based on whether
 *  they derive from `BasisFunction` or `VectorSpace` it will then evaluate
 *  to `TensorElement` or `TensorRepresentation` respectively.
 *
 *  @tparam BraType The type of the bra. Assumed to derive from either
 *                  BasisFunction or VectorSpace.
 *  @tparam OperatorType The type of the object acting on the ket
 *  @tparam KetType The type of the ket. Assumed to derive from either
 *                  BasisFunction or VectorSpace.
 */
template<typename BraType, typename OperatorType, typename KetType>
using bra_ket_base_type =
  std::conditional_t<is_tensor_element_v<BraType, OperatorType, KetType>,
                     TensorElement<double>, TensorRepresentation>;
} // namespace detail_

/** @brief Specifies the calculation (or a piece of it) that the user wants.
 *
 *  @tparam BraType Type of the object
 */
template<typename BraType, typename OperatorType, typename KetType>
class BraKet
  : public detail_::bra_ket_base_type<BraType, OperatorType, KetType> {
private:
    /// Type of *this
    using my_type = BraKet<BraType, OperatorType, KetType>;

    /// Type of a pointer to a base operator
    using operator_base_pointer =
      typename chemist::qm_operator::OperatorBase::base_pointer;

public:
    /// Type *this inherits from
    using base_type =
      detail_::bra_ket_base_type<BraType, OperatorType, KetType>;

    /// Type of a pointer to the base class
    using base_pointer = typename base_type::base_pointer;

    /// Type of a read-only reference to an object fo type base_type
    using const_base_reference = typename base_type::const_base_reference;

    /// Type of the bra object
    using bra_type = BraType;

    /// Type behaving like a mutable reference to a bra_type object
    using bra_reference = bra_type&;

    /// Type behaving like a read-only reference to a bra_type object
    using const_bra_reference = const bra_type&;

    /// Type of the operator
    using operator_type = OperatorType;

    /// Type behaving like a mutable reference to an operator_type object
    using operator_reference = operator_type&;

    /// Type behaving like a read-only reference to an operator_type object
    using const_operator_reference = const operator_type&;

    /// Type of the ket object
    using ket_type = KetType;

    /// Type behaving like a mutable reference to a ket_type object
    using ket_reference = ket_type&;

    /// Type behaving like a read-only reference to a ket_type object
    using const_ket_reference = const ket_type&;

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Constructs a BraKet with the given bra, operator, and ket.
     *
     *  @tparam BraType2 The type of @p bra. Must be implicitly convertible to
     *          BraType.
     *  @tparam OperatorType2 The type of @p op. Must be implicitly convertible
     *          to OperatorType.
     *  @tparam KetType2 The type of @p ket. Must be implicitly convertible to
     *          KetType.
     *
     *  @note This function is templated so we can automatically handle copying
     *        and moving of the inputs as appropriate.
     *
     *  This ctor is used to create a stateful BraKet object.
     *
     *  @param[in] bra The wavefunction or vector space associated with the bra.
     *  @param[in] op The operator acting on the ket.
     *  @param[in] ket The wavefunction or vector space associated with the ket.
     *
     *  @throw ??? Throws if forwarding any of the arguments throws. Same throw
     *             guarantee.
     */
    template<typename BraType2, typename OperatorType2, typename KetType2>
    BraKet(BraType2&& bra, OperatorType2&& op, KetType2&& ket) :
      m_tuple_{std::forward<BraType2>(bra),
               clone_op_(std::forward<OperatorType2>(op)),
               std::forward<KetType2>(ket)} {}

    /** @brief Initializes *this with a deep copy of @p other.
     *
     *  @param[in] other The BraKet to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating *this. Strong
     *                        throw guarantee.
     */
    BraKet(const BraKet& other) :
      BraKet(other.bra(), other.op(), other.ket()) {};

    /** @brief Initializes *this by taking the state from @p other.
     *
     *  @param[in,out] other The BraKet to take the state from. After this
     *                       operator @p other is in a valid, but otherwise
     *                       undefined state.
     *
     *  @throw None No throw guarantee.
     */
    BraKet(BraKet&& other) noexcept = default;

    /** @brief Overwrites the state in *this with a deep copy of the state in
     *         @p rhs.
     *
     *  @param[in] rhs The BraKet whose state will be deep copied.
     *
     *  @return *this after overwriting its state with a deep copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy. Strong
     *                        throw guarantee.
     */
    BraKet& operator=(const BraKet& rhs) {
        m_tuple_ = tuple_type(rhs.bra(), clone_op_(rhs.op()), rhs.ket());
        return *this;
    };

    /** @brief Overwrites the state in *this with the state from @p rhs.
     *
     *  @param[in,out] rhs The BraKet to take the state from. After this call
     *                     @p rhs will be in a valid but otherwise undefined
     *                     state.
     *
     *  @return *this after overwriting its state with the state from @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    BraKet& operator=(BraKet&& rhs) noexcept = default;

    // -------------------------------------------------------------------------
    // -- Getters and Setters
    // -------------------------------------------------------------------------

    /** @brief Returns a mutable reference to the bra's state.
     *
     *  TL;DR this function probably does exactly what you think it does, but
     *  the description is trying to be mathematically rigorous to avoid
     *  questions.
     *
     *  Formally the bra of a bra-ket is a linear function mapping the ket to
     *  a scalar. The linear function responsible for this mapping is usually
     *  (always?) the inner product with the adjoint of the specified vector,
     *  e.g., @f$\bra{u}@f$ is short hand for "multiply from the left by u
     *  dagger".
     *
     *  If this bra-ket is a tensor element, then this method returns the
     *  adjoint of the vector the linear function should multiply by (e.g., in
     *  the example above you get back "u" NOT "u dagger"), i.e., to evaluate
     *  the bra-ket represented by *this you need to take the adjoint of the
     *  return before left multiplying it with the ket).
     *
     *  If this bra-ket is a tensor representation the tensor element
     *  description still applies, just now for every vector in the vector
     *  space. In other words, the returned vector space is in its "ket form",
     *  e.g., if the 0-th vector in the return is "u", then to form the 0-th
     *  row of the matrix *this represent you need to left multiply each ket
     *  vector by "u dagger".
     *
     *  @return A mutable reference to the bra.
     *
     *  @throw None No throw guarantee.
     */
    bra_reference bra() { return std::get<0>(m_tuple_); }

    /** @brief Returns a read-only reference to the bra's state.
     *
     *  This method works identically to the non-const version except that the
     *  result is read-only. See the description for the non-const version for
     *  more details.
     *
     *  @return A read-only reference to the bra.
     *
     *  @throw None No throw guarantee.
     */
    const_bra_reference bra() const { return std::get<0>(m_tuple_); }

    /** @brief Returns a mutable reference to the operator.
     *
     *  Each bra-ket object has an operator associated with it, even if it is
     *  an overlap bra-ket (in which case the operator is the identity
     *  operator). This method is used to retrieve the operator.
     *
     *  @note "operator" is a keyword in C++ so we op-ted (ha) to break our
     *        guidelines and abbreviate it.
     *
     *  @return A mutable reference to the operator.
     *
     *  @throw None No throw guarantee.
     */
    operator_reference op() {
        return *(static_cast<operator_type*>(std::get<1>(m_tuple_).get()));
    }

    /** @brief Returns a read-only reference to the operator.
     *
     *  This method works identically to the non-const version except that the
     *  result is read-only. See the description for the non-const version for
     *  more details.
     *
     *  @return A read-only reference to the operator.
     *
     *  @throw None No throw guarantee.
     */
    const_operator_reference op() const {
        return *(static_cast<operator_type*>(std::get<1>(m_tuple_).get()));
    }

    /** @brief Returns a mutable reference to the ket's state.
     *
     *  Formally, the ket in a bra-ket is always a vector. Here the ket can be
     *  a single vector or a set of vectors depending on whether *this
     *  represents a single tensor element or the entire tensor. This method
     *  returns the vector if *this is a tensor element, and the vector space
     *  if *this is a tensor representation.
     *
     *  @return A mutable reference to the ket.
     *
     *  @throw None No throw guarantee.
     */
    ket_reference ket() { return std::get<2>(m_tuple_); }

    /** @brief Returns a read-only reference to the ket's state.
     *
     *  This method works identically to the non-const version except that the
     *  result is read-only. See the description for the non-const version for
     *  more details.
     *
     *  @return A read-only reference to the ket.
     *
     *  @throw None No throw guarantee.
     */
    const_ket_reference ket() const { return std::get<2>(m_tuple_); }

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Is *this value equal to @p rhs?
     *
     *  Two BraKet objects are value equal if:
     *  - They have the same values for each template type parameter
     *  - the bra states compare equal,
     *  - the operator states compare equal, and
     *  - the ket states compare equal.
     *
     *  N.b. this operator is templated on the types so that users can compare
     *  bra-kets with different types (interacting with mixed types happens in
     *  generic programming).
     *
     *  @tparam BraType2 The type of the bra object stored in @p rhs.
     *  @tparam OperatorType2 The type of the operator object stored in @p rhs.
     *  @tparam KetType2 The type of the ket object stored in @p rhs.
     *
     *  @param[in] rhs the bra-ket object to compare to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename BraType2, typename OperatorType2, typename KetType2>
    bool operator==(
      const BraKet<BraType2, OperatorType2, KetType2>& rhs) const noexcept;

    /** @brief Is *this different from @p rhs?
     *
     *  This method defines "different" as "not value equal" and thus simply
     *  negates the result of operator==. See the description of operator== for
     *  the definition of value equal.
     *
     *  @tparam BraType2 The type of object @p rhs has for a bra.
     *  @tparam OperatorType2 The type of object @p rhs has for an operator.
     *  @tparam KetType2 The type of object @p rhs has for a ket.
     *
     *  @param[in] rhs The bra-ket to compare to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename BraType2, typename OperatorType2, typename KetType2>
    bool operator!=(
      const BraKet<BraType2, OperatorType2, KetType2>& rhs) const noexcept {
        return !(*this == rhs);
    }

protected:
    /// Implements clone by calling copy ctor
    base_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }

    /// Implements are_equal by downcasting @p rhs and comparing via
    /// operator==
    bool are_equal_(const_base_reference rhs) const noexcept override {
        auto prhs = dynamic_cast<const my_type*>(&rhs);
        if(prhs == nullptr) return false; // Different types
        return (*this) == *prhs;
    }

private:
    /// Type of the tuple which will store the state.
    using tuple_type = std::tuple<bra_type, operator_base_pointer, ket_type>;

    /// The actual state defining the bra-ket
    tuple_type m_tuple_;

    /// Gets a clone pointer from the incoming operator
    operator_base_pointer clone_op_(const operator_type& op) {
        return op.clone();
    }
};

/// Enables deducing the template type parameters from the value ctor.
template<typename BraType2, typename OperatorType2, typename KetType2>
BraKet(BraType2&&, OperatorType2&&, KetType2&&)
  -> BraKet<std::decay_t<BraType2>, std::decay_t<OperatorType2>,
            std::decay_t<KetType2>>;

// -----------------------------------------------------------------------------
// Out of line inline definitions
// -----------------------------------------------------------------------------

template<typename BraType, typename OperatorType, typename KetType>
template<typename BraType2, typename OperatorType2, typename KetType2>
bool BraKet<BraType, OperatorType, KetType>::operator==(
  const BraKet<BraType2, OperatorType2, KetType2>& rhs) const noexcept {
    constexpr bool same_bra_type = std::is_same_v<BraType, BraType2>;
    constexpr bool same_op_type  = std::is_same_v<OperatorType, OperatorType2>;
    constexpr bool same_ket_type = std::is_same_v<KetType, KetType2>;

    if constexpr(same_bra_type && same_op_type && same_ket_type) {
        return bra() == rhs.bra() && op().are_equal(rhs.op()) &&
               ket() == rhs.ket();
    } else {
        return false;
    }
}

} // namespace chemist::braket