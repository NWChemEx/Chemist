#pragma once
#include "libchemist/basis_set/ao_basis_function.hpp"
namespace libchemist::basis_set {

/** @brief A shell of an AO centered basis functions.
 *
 * This class assumes that all basis functions have the same primitives (as is
 * usually the case). If this is not true it will be necessary to implement
 * another AOShell class to account for this case. This class also is not
 * suitable for use with general contractions.
 *
 * @tparam T
 * @tparam BFType
 */
template<typename T, typename SizeType = type::size,
         typename BFType = AOBasisFunction<T, SizeType>>
class AOShell {
public:
    using reference       = BFType&;
    using const_reference = const BFType&;

    reference operator[](type::size) { return m_bf_; }
    const_reference operator[](type::size) const { return m_bf_; }

    type::size size() const;

    /** @brief Computes the number of primitives in each basis function.
     *
     * @return The number of primitives in each basis function in this shell.
     * @throw none No throw guarantee.
     */
    type::size nprims() const noexcept { return m_bf_.size(); }

    bool& pure() noexcept { return m_pure_; }
    const bool& pure() const noexcept { return m_pure_; }

    SizeType& l() { return m_bf_.l(); }
    const SizeType& l() const { return m_bf_.l(); }

    auto& center() noexcept { return m_bf_.center(); }

    T& coef(type::size i) noexcept {}
    const T& coef(type::size i) const noexcept {}

    T& alpha(type::size i) noexcept {}
    const T& alpha(type::size i) const noexcept {}

private:
    bool m_pure_;
    BFType m_bf_;
};

template<typename T, typename SizeType, typename BFType>
type::size AOShell<T, SizeType, BFType>::size() const {
    return pure() ? 2ul * l() + 1ul :
                    utilities::binomial_coefficient<type::size>(2ul + l(), l());
}

} // namespace libchemist::basis_set
