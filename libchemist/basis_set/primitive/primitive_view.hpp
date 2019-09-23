#pragma once
#include "libchemist/basis_set/primitive/primitive.hpp"

namespace libchemist {

template<typename T>
class PrimitiveView {
public:
    using value_type      = std::remove_const_t<T>;
    using reference       = T&;
    using const_reference = const T&;

    PrimitiveView() noexcept;
    PrimitiveView(T* coef, T* exp, T* x, T* y, T* z) noexcept;

    reference coefficient() noexcept { return m_pimpl_.coefficient(); }
    const_reference coefficient() const noexcept;
    reference exponent() noexcept { return m_pimpl_.exponent(); }
    const_reference exponent() const noexcept { return m_pimpl_.exponent(); }

    reference coord(size_type i) { return m_pimpl_.coord(i); }
    const_reference coord(size_type i) { return m_pimpl_.coord(i); }

    reference x() noexcept { return m_pimpl_.x(); }
    const_reference x() const noexcept { return m_pimpl_.x(); }

    reference y() noexcept { return m_pimpl_.y(); }
    const_reference y() const noexcept { return m_pimpl_.y(); }

    reference z() noexcept { return m_pimpl_.z(); }
    const_reference z() const noexcept { return m_pimpl_.z(); }

    operator const Point<value_type>&() const { return m_pimpl_; }
    operator const Primitive<value_type>&() const { return m_pimpl_; }

private:
    Primitive<value_type> m_pimpl_;
};

// ------------------------------------Implemenations--------------------------

template<typename T>
PrimitiveView<T>::PrimitiveView() noexcept :
  PrimitiveView(nullptr, nullptr, nullptr, nullptr, nullptr) {}

template<typename T>
typename PrimitiveView<T>::const_reference PrimitiveView<T>::coefficient() const
  noexcept {
    return m_pimpl_.coefficient();
}

extern template class PrimitiveView<double>;
extern template class PrimitiveView<const double>;
extern template class PrimitiveView<float>;
extern template class PrimitiveView<const float>;

} // namespace libchemist
