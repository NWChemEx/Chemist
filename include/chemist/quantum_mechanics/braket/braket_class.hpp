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
  std::conditional_t<is_tensor_representation_v<BraType, OperatorType, KetType>,
                     TensorRepresentation, TensorElement>;
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

public:
    /// Type *this inherits from
    using base_type =
      detail_::bra_ket_base_type<BraType, OperatorType, KetType>;

    /// Type of a pointer to the base class
    using base_pointer = typename base_type::base_pointer;

    /// Type of a read-only reference to an object fo type base_type
    using const_base_reference = typename base_type::const_base_reference;

    template<typename BraType2, typename OperatorType2, typename KetType2>
    BraKet(BraType2&& bra, OperatorType2&& op, KetType2&& ket) :
      m_bra_(std::forward<BraType2>(bra)),
      m_operator_(std::forward<OperatorType2>(op)),
      m_ket_(std::forward<KetType2>(ket)) {}

    BraKet(const BraKet& other) = default;

    decltype(auto) bra() { return m_bra_; }

    decltype(auto) bra() const { return m_bra_; }

    decltype(auto) the_operator() { return m_operator_; }

    decltype(auto) the_operator() const { return m_operator_; }

    decltype(auto) ket() { return m_ket_; }

    decltype(auto) ket() const { return m_ket_; }

    template<typename BraType2, typename OperatorType2, typename KetType2>
    bool operator==(
      const BraKet<BraType2, OperatorType2, KetType2>& rhs) const noexcept;

    template<typename BraType2, typename OperatorType2, typename KetType2>
    bool operator!=(
      const BraKet<BraType2, OperatorType2, KetType2>& rhs) const noexcept {
        return !(*this == rhs);
    }

protected:
    base_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }

    bool are_equal_(const_base_reference rhs) const noexcept override {
        auto prhs = dynamic_cast<const my_type*>(&rhs);
        if(prhs == nullptr) return false; // Different types
        return (*this) == *prhs;
    }

private:
    BraType m_bra_;
    OperatorType m_operator_;
    KetType m_ket_;
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
        return std::tie(m_bra_, m_operator_, m_ket_) ==
               std::tie(rhs.m_bra_, rhs.m_operator_, rhs.m_ket_);
    } else {
        return false;
    }
}

} // namespace chemist::braket