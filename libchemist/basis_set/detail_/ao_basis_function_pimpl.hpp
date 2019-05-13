#pragma once
#include "libchemist/basis_set/primitive.hpp"
#include "libchemist/types.hpp"
#include <algorithm>
#include <vector>

namespace libchemist::basis_set::detail_ {

template<typename T, typename PrimType = Gaussian<T>>
class AOBasisFxnPIMPL {
private:
    using vector_type = std::vector<PrimType>;

public:
    using value_type      = PrimType;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using iterator        = typename vector_type::iterator;
    using const_iterator  = typename vector_type::const_iterator;

    template<typename InputIt>
    AOBasisFxnPIMPL(InputIt&& first, InputIt&& last);

    type::size size() const noexcept { return m_prims_.size(); }
    reference operator[](type::size i) noexcept { return m_prims_[i]; }
    const_reference operator[](type::size i) const noexcept;

    iterator begin() noexcept { return m_prims_.begin(); }
    iterator end() noexcept { return m_prims_.end(); }
    const_iterator begin() const noexcept { return m_prims_.begin(); }
    const_iterator end() const noexcept { return m_prims_.end(); }

private:
    std::vector<PrimType> m_prims_;
};

template<typename T, typename PrimType = GaussianAlias<T>>
class AOBasisFxnAliasPIMPL {
public:
    using value_type      = PrimType;
    using reference       = PrimType&;
    using const_reference = const PrimType&;
    using iterator        = PrimType*;
    using const_iterator  = const PrimType*;

    template<typename RandomAccessIt>
    AOBasisFxnAliasPIMPL(RandomAccessIt&& first, RandomAccessIt&& last);

    type::size size() const noexcept { return m_size_; }
    reference operator[](type::size i) { return m_begin_ + i; }
    const_reference operator[](type::size i) const { return m_begin_ + i; }

    iterator begin() { return m_begin_; }
    iterator end() { return m_end_; }
    const_iterator begin() const { return m_begin_; }
    const_iterator end() const { return m_end_; }

private:
    type::size m_size_;
    iterator m_begin_;
    iterator m_end_;
};

//-------------------------Implementations--------------------------------------
template<typename T, typename PrimType>
template<typename InputIt>
AOBasisFxnPIMPL<T, PrimType>::AOBasisFxnPIMPL(InputIt&& first, InputIt&& last) :
  m_prims_(std::forward<InputIt>(first), std::forward<InputIt>(last)) {}

template<typename T, typename PrimType>
typename AOBasisFxnPIMPL<T, PrimType>::const_reference
  AOBasisFxnPIMPL<T, PrimType>::operator[](type::size i) const noexcept {
    return m_prims_[i];
}

template<typename T, typename PrimType>
template<typename RandomAccessIt>
AOBasisFxnAliasPIMPL<T, PrimType>::AOBasisFxnAliasPIMPL(RandomAccessIt&& first,
                                                        RandomAccessIt&& last) :
  m_size_(last - first),
  m_begin_(std::forward<RandomAccessIt>(first)),
  m_end_(std::forward<RandomAccessIt>(last)) {}

} // namespace libchemist::basis_set::detail_
