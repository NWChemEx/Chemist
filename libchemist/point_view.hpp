#pragma once
#include "libchemist/point.hpp"

namespace libchemist {

template<typename T>
class PointView {
public:
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = std::size_t;

    PointView();
    PointView(T* x, T* y, T* z);

    virtual ~PointView() noexcept;

    reference coord(size_type q) { return m_pimpl_.coord(q); }
    const_reference coord(size_type q) const { return m_pimpl_.coord(q); }
    reference x() noexcept { return coord(0); }
    const_reference x() const noexcept { return coord(0); }
    reference y() noexcept { return coord(1); }
    const_reference y() const noexcept { return coord(1); }
    reference z() noexcept { return coord(2); }
    const_reference z() const noexcept { return coord(2); }
    operator Point<T>() const { return m_pimpl_; }
    operator const Point<T>&() const { return m_pimpl_; }

private:
    Point<T> m_pimpl_;
};

extern template class PointView<double>;
extern template class PointView<float>;

} // namespace libchemist
