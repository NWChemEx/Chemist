#pragma once
#include "libchemist/detail_/view_base.hpp"
#include "libchemist/point/point.hpp"

namespace libchemist {

/** @brief Implements reference-like semantics with respect to a Point instance.
 *
 *  This class works by wrapping a Point instance which gets its state from a
 *  PIMPL holding aliases. By holding a Point instance (instead of a PIMPL
 *  directly) we are able to have the PointView be usable, by reference,
 *  anywhere a Point can.
 *
 *  Note that when using PointView, its const-semantics are such that:
 *
 *  @begincode
 *  Point<double> p{1.0, 2.0, 3.0};
 *  const PointView<T> const_pv{p};
 *  PointView<T> non_const_pv{const_pv};
 *  @endcode
 *
 *  which would allow you to bypass the const-ness of `const_pv`. This is
 *  similar to how iterators work in the STL:
 *
 *  @begincode
 *  std::vector<double> v{1.0, 2.0, 3.0};
 *  // The type of a read/write iterator to an std::vector<double>
 *  using iterator = typename std::vector<double>::iterator;
 *  const iterator const_itr = v.begin();
 *  iterator non_const_itr{v};
 *  @endcode
 *
 *  If you want to return a PointView to a Point<double> and you don't want
 *  people modifying the aliased container the `const` needs to go in the
 *  PointView's type, i.e., PointView<const double>.
 *
 *  @tparam T The type used to hold the coordinates of the aliased Point. Should
 *          be cv-qualified to match the Point instance it is aliasing.
 */
template<typename T, typename AliasedType>
class PointViewBase
  : public detail_::ViewBase<std::is_const_v<T>, AliasedType> {
private:
    using no_cv_t                  = std::remove_cv_t<T>;
    static constexpr bool is_const = std::is_const_v<T>;
    using base_type                = detail_::ViewBase<is_const, AliasedType>;

public:
    /// Unsigned integral type used for indexing/offsets
    using size_type = std::size_t;

    using detail_::ViewBase<is_const, AliasedType>::ViewBase;

    /** @brief Returns the @p q-th coordinate of the aliased Point
     *
     *  This function can be used to retrieve one of the components of the
     *  aliased Point. The resulting component is read-/write-able if @p T is
     *  non-const. Since the returned component is an alias, modifying it will
     *  also modify the parent Point.
     *
     *  @param[in] q Which component should we return? Must be in the range
     *             [0, 3).
     *  @return The requested component, (possibly) in a read-/write-able
     *          format.
     *  @throw std::out_of_range if @p q is not in the range [0, 3)
     */
    decltype(auto) coord(size_type q) { return this->pimpl().coord(q); }

    /** @brief Returns the @p q-th coordinate of the aliased Point
     *
     *  This function can be used to retrieve one of the components of the
     *  aliased Point. The resulting component is read-only.
     *
     *  @param[in] q Which component should we return? Must be in the range
     *             [0, 3).
     *  @return The requested component, in a read-only format.
     *
     *  @throw std::out_of_range if @p q is not in the range [0, 3)
     */
    decltype(auto) coord(size_type q) const { return this->pimpl().coord(q); }

    /** @brief Returns the x-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(0)`.
     *
     *  @return The x-coordinate of the aliased Point in a (possibly)
     *          read-/write-able state.
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) x() noexcept { return coord(0); }

    /** @brief Returns the x-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(0) const`.
     *
     *  @return The x-coordinate of the aliased Point in a read-only state.
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) x() const noexcept { return coord(0); }

    /** @brief Returns the y-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(1)`.
     *
     *  @return The y-coordinate of the aliased Point in a (possibly)
     *          read-/write-able state.
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) y() noexcept { return coord(1); }

    /** @brief Returns the y-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(1) const`.
     *
     *  @return The y-coordinate of the aliased Point in a read-only state.
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) y() const noexcept { return coord(1); }

    /** @brief Returns the z-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(2)`.
     *
     *  @return The z-coordinate of the aliased Point in a (possibly)
     *          read-/write-able state.
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) z() noexcept { return coord(2); }

    /** @brief Returns the z-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(2) const`.
     *
     *  @return The x-coordinate of the aliased Point in a read-only state.
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) z() const noexcept { return coord(2); }

    operator const Point<no_cv_t>&() const { return this->pimpl(); }
};

template<typename T>
using PointView = PointViewBase<T, Point<std::remove_cv_t<T>>>;

extern template class PointViewBase<double, Point<double>>;
extern template class PointViewBase<const double, Point<double>>;
extern template class PointViewBase<float, Point<float>>;
extern template class PointViewBase<const float, Point<float>>;

} // namespace libchemist
