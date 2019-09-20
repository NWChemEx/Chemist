#pragma once
#include "libchemist/point/point.hpp"
#include <memory>

namespace libchemist {
namespace detail_ {
template<typename T>
class PrimitivePIMPL;
}

template<typename T>
class Primitive : public Point<T> {
public:
    using reference       = T&;
    using const_reference = const T&;
    Primitive();
    Primitive(T coef, T exp, T x, T y, T z);
    Primitive(const Primitive<T>& rhs);
    Primitive(Primitive<T>&& rhs) noexcept;
    Primitive<T>& operator=(const Primitive<T>& rhs) noexcept;
    Primitive<T>& operator=(Primitive<T>&& rhs) noexcept;

    ~Primitive() noexcept override;
    reference coefficient() noexcept;
    const_reference coefficient() const noexcept;
    reference exponent() noexcept;
    const_reference exponent() const noexcept;

private:
    using pimpl_t = detail_::PrimitivePIMPL<T>;
    std::unique_ptr<pimpl_t> m_pimpl_;
};

extern template class Primitive<double>;
extern template class Primitive<float>;

} // namespace libchemist
