#pragma once
#include "libchemist/basis_set/detail_/ao_basis_function_pimpl.hpp"

namespace libchemist::basis_set {

/** @brief An API to an AO basis function consistent with how most integral
 *         libraries think abou them.
 *
 *  An AO basis function is a collection of primitive basis functions; however,
 *  those primitives are shared with other basis functions in the shell. Thus,
 *  this class provides a read-only view to the underlying primitives.
 *
 * @tparam T
 * @tparam PrimType
 */

template<typename T, typename PIMPLType = detail_::AOBasisFxnPIMPL<T>>
class AOBasisFunction {
public:
    using value_type      = typename PIMPLType::value_type;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using iterator        = typename PIMPLType::iterator;
    using const_iterator  = typename PIMPLType::const_iterator;

    template<typename RandomAccessIt>
    AOBasisFunction(RandomAccessIt&& begin, RandomAccessIt&& end);

    reference operator[](type::size i) { return m_pimpl_[i]; }
    const_reference operator[](type::size i) const { return m_pimpl_[i]; }
    type::size size() const noexcept { return m_pimpl_.size(); }

    iterator begin() { return m_pimpl_.begin(); }
    iterator end() { return m_pimpl_.end(); }
    const_iterator begin() const { return m_pimpl_.begin(); }
    const_iterator end() const { return m_pimpl_.end(); }
    const_iterator cbegin() const { return m_pimpl_.begin(); }
    const_iterator cend() const { return m_pimpl_.end(); }

    template<typename U, typename V>
    bool operator==(const AOBasisFunction<U, V>& rhs) const noexcept;

    template<typename U, typename V>
    bool operator!=(const AOBasisFunction<U, V>& rhs) const noexcept;

private:
    PIMPLType m_pimpl_;
};

template<typename T>
using AOBasisFunctionAlias =
  AOBasisFunction<T, detail_::AOBasisFxnAliasPIMPL<T>>;

//----------------------------Implementations-----------------------------------

template<typename T, typename PrimType>
template<typename RandomAccessIt>
AOBasisFunction<T, PrimType>::AOBasisFunction(RandomAccessIt&& begin,
                                              RandomAccessIt&& end) :
  m_pimpl_(std::forward<RandomAccessIt>(begin),
           std::forward<RandomAccessIt>(end)) {}

template<typename T, typename PrimType>
template<typename U, typename V>
bool AOBasisFunction<T, PrimType>::operator==(
  const AOBasisFunction<U, V>& rhs) const noexcept {
    return std::equal(cbegin(), cend(), rhs.cbegin(), rhs.cend());
}

template<typename T, typename PrimType>
template<typename U, typename V>
bool AOBasisFunction<T, PrimType>::operator!=(
  const AOBasisFunction<U, V>& rhs) const noexcept {
    return !((*this) == rhs);
}

} // namespace libchemist::basis_set
