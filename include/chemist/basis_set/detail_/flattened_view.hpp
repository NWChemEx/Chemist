#pragma once
#include <functional>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist::detail_ {

template<typename T, typename ConstT>
class FlattenedView
  : public utilities::IndexableContainerBase<FlattenedView<T, ConstT>> {
private:
    using my_type   = FlattenedView<T, ConstT>;
    using base_type = utilities::IndexableContainerBase<my_type>;

public:
    using value_type = typename T::aliased_type;
    /// Unsigned integral type used for offsets/indexing
    using size_type = typename base_type::size_type;

    template<typename U, typename V, typename W>
    FlattenedView(U&& size_fxn, V&& at_fxn, W&& const_at_fxn) noexcept;

private:
    friend base_type;
    size_type size_() const noexcept { return m_size_(); }
    T at_(size_type i) { return m_at_(i); }
    ConstT at_(size_type i) const { return m_const_at_(i); }

    std::function<size_type()> m_size_;
    std::function<T(size_type)> m_at_;
    std::function<ConstT(size_type)> m_const_at_;
};

template<typename T, typename ConstT>
template<typename U, typename V, typename W>
FlattenedView<T, ConstT>::FlattenedView(U&& size_fxn, V&& at_fxn,
                                        W&& const_at_fxn) noexcept :
  m_size_(std::forward<U>(size_fxn)),
  m_at_(std::forward<V>(at_fxn)),
  m_const_at_(std::forward<W>(const_at_fxn)) {}

} // namespace chemist::detail_
