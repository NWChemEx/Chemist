#pragma once
#include <memory>

namespace libchemist {
namespace detail_ {
template<typename T>
class PointPIMPL;
}

/**
 * This class is largely code factorization for many of the point-centered
 * quantities in LibChemist such as Gaussians, Shells, and Atoms.
 *
 * @tparam T The type used to store each Cartesian coordinate.
 *
 */
template<typename T>
class Point {
public:
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = std::size_t;

    Point();
    Point(T x, T y, T z);

    virtual ~Point() noexcept;

    reference coord(size_type q);
    const_reference coord(size_type q) const;
    reference x() noexcept { return coord(0); }
    const_reference x() const noexcept { return coord(0); }
    reference y() noexcept { return coord(1); }
    const_reference y() const noexcept { return coord(1); }
    reference z() noexcept { return coord(2); }
    const_reference z() const noexcept { return coord(2); }

protected:
    using pimpl_t = detail_::PointPIMPL<T>;
    Point(std::unique_ptr<pimpl_t> pimpl) noexcept;
    virtual pimpl_t& pimpl() noexcept;
    virtual const pimpl_t& pimpl() const noexcept;

private:
    std::unique_ptr<detail_::PointPIMPL<T>> m_pimpl_;
};

extern template class Point<double>;
extern template class Point<float>;

} // namespace libchemist
