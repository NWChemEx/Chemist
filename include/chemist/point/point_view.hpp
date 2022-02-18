#pragma once
#include "chemist/detail_/view_base.hpp"
#include "chemist/point/point.hpp"

namespace chemist {

/** @brief Implements reference-like semantics with respect to a Point instance.
 *
 *  This class extends the ViewBase API to include forwarding of the Point API.
 *
 *  @tparam T The type used to hold the coordinates of the aliased Point. Should
 *          be cv-qualified to match the Point instance it is aliasing.
 *
 *  @tparam AliasedType The type the derived-most class is a view of.
 *                      @p AliasedType is assumed to have been derived from
 *                      Point.
 */
template<typename T, typename AliasedType>
class PointView : public detail_::ViewBase<std::is_const_v<T>, AliasedType> {
private:
    /// Type of @p T w/o cv-qualifiers
    using no_cv_t = std::remove_cv_t<T>;
    /// True if @p T is const-qualified
    static constexpr bool is_const = std::is_const_v<T>;
    /// Type of the base class
    using base_type = detail_::ViewBase<is_const, AliasedType>;

public:
    /// Unsigned integral type used for indexing/offsets
    using size_type = typename AliasedType::size_type;

    /// Forward the base's ctors
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
}; // class PointView

} // namespace chemist
