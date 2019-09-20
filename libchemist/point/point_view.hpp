#pragma once
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
template<typename T>
class PointView {
private:
    /// Is @p T const qualified
    static constexpr bool is_const_ = std::is_const_v<T>;

public:
    /// The non-cv qualified type of the components of the aliased Point
    using value_type = std::remove_cv_t<T>;
    /// The type of a read/write Point class
    using point_type = Point<value_type>;
    /// The type of a const Point class
    using const_point = const Point<value_type>;
    /// Type of this view's Point class
    using parent_type = std::conditional_t<is_const_, const_point, point_type>;
    /// Type of a (possibly) read/write reference to an aliased component
    using reference = T&;
    /// Type of a read-only reference to an aliased component
    using const_reference = const T&;
    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /** @brief Creates a new PointView which does not alias any Point.
     *
     *  The default ctor creates a PointView which does not alias any Point.
     *  Attempting to use the resulting PointView as anything other than a
     *  placeholder is undefined behavior and likely will result in a segfault.
     *  The resulting instance can be made to alias a Point by copy/move
     *  assignment.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPL. Strong throw guarantee.
     *
     */
    PointView() : PointView(nullptr, nullptr, nullptr) {}

    /** @brief Creates an alias to the specified point
     *
     *  This ctor can be used to create a PointView instance that aliases the
     *  provided point.
     *
     *  @param[in] point The point to alias.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy @p point.
     *         Strong throw guarantee.
     */
    explicit PointView(parent_type& point);

    /** @brief Creates a PointView which aliases the Point
     *
     *  This ctor makes a PointView that aliases the provided coordinates.
     *  Strictly speaking the coordinates do not need to come from the same
     *  Point object (such a scenario is common when the coordinates are stored
     *  contiguously inside the class creating the PointView instance).
     *
     *  @param[in] x The x-coordinate this view will alias.
     *  @param[in] y The y-coordinate this view will alias.
     *  @param[in] z The z-coordinate this view will alias.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPL inside the wrapped Point object. Strong throw guarantee.
     */
    PointView(T* x, T* y, T* z);

    /** @brief Makes a new PointView that aliases the same Point as @p rhs.
     *
     *  This function creates a new PointView which aliases the same Point as
     *  @p rhs. Thus modifying the aliased Point through @p rhs or this instance
     *  will have the same result. The PointView resulting from this operation
     *  is distinct from @p rhs in the sense that modifying @p rhs's state (such
     *  as making it alias another Point) will not change this instance. Since
     *  it is only possible to copy from PointView instances with the same
     *  template type parameter, copy construction can not be used to bypass
     *  const, e.g. `PointView<double> non_const{PointView<const double> pv};`
     *  will not compile.
     *
     * @param[in] rhs The PointView to copy.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *        PIMPL. Strong throw guarantee.
     */
    PointView(const PointView<T>& rhs);

    /** @brief Creates a PointView by taking another PointView's state.
     *
     *  This ctor will initialize a new PointView instance by transferring
     *  ownership of another PointView's state. The resulting PointView will
     *  continue to alias the same Point as @p rhs did and all references remain
     *  valid.
     *
     *  @param[in, out] rhs The PointView to take the state from. After this
     *                      ctor, @p rhs will be PIMPL-less and thus in an
     *                      invalid state. @p rhs can be restored to a valid
     *                      state by move assigning to it.
     *
     *  @throw none No throw guarantee.
     */
    PointView(PointView<T>&& rhs) noexcept = default;

    /** @brief Makes this PointView alias the same Point as @p rhs.
     *
     *  This function causes the current PointView to aliases the same Point as
     *  @p rhs. Thus modifying the aliased Point through @p rhs or this instance
     *  will have the same result. The PointView resulting from this operation
     *  is distinct from @p rhs in the sense that modifying @p rhs's state (such
     *  as making it alias another Point) will not change this instance. Since
     *  it is only possible to copy from PointView instances with the same
     *  template type parameter, copy assignment can not be used to bypass
     *  const.
     *
     * @param[in] rhs The PointView to copy.
     *
     * @return the current instance, but now aliasing the same Point as @p rhs.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *        PIMPL. Strong throw guarantee.
     */
    PointView<T>& operator=(const PointView<T>& rhs);

    /** @brief Transfers another PointView's state to the current instance.
     *
     *  This function will transfer ownership of another PointView's state to
     *  the current instance. After the transfer this instance will alias the
     *  same Point as @p rhs did and all references to that Point's state remain
     *  valid.
     *
     *  @param[in, out] rhs The PointView to take the state from. After this
     *                      ctor, @p rhs will be PIMPL-less and thus in an
     *                      invalid state. @p rhs can be restored to a valid
     *                      state by move assigning to it.
     *
     *  @return The current instance after transferring ownership of @p rhs's
     *          state.
     *
     *  @throw none No throw guarantee.
     */
    PointView<T>& operator=(PointView<T>&& rhs) noexcept = default;

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
    reference coord(size_type q) { return m_pimpl_.coord(q); }

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
    const_reference coord(size_type q) const { return m_pimpl_.coord(q); }

    /** @brief Returns the x-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(0)`.
     *
     *  @return The x-coordinate of the aliased Point in a (possibly)
     *          read-/write-able state.
     *
     *  @throw none No throw guarantee.
     */
    reference x() noexcept { return coord(0); }

    /** @brief Returns the x-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(0) const`.
     *
     *  @return The x-coordinate of the aliased Point in a read-only state.
     *
     *  @throw none No throw guarantee.
     */
    const_reference x() const noexcept { return coord(0); }

    /** @brief Returns the y-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(1)`.
     *
     *  @return The y-coordinate of the aliased Point in a (possibly)
     *          read-/write-able state.
     *
     *  @throw none No throw guarantee.
     */
    reference y() noexcept { return coord(1); }

    /** @brief Returns the y-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(1) const`.
     *
     *  @return The y-coordinate of the aliased Point in a read-only state.
     *
     *  @throw none No throw guarantee.
     */
    const_reference y() const noexcept { return coord(1); }

    /** @brief Returns the z-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(2)`.
     *
     *  @return The z-coordinate of the aliased Point in a (possibly)
     *          read-/write-able state.
     *
     *  @throw none No throw guarantee.
     */
    reference z() noexcept { return coord(2); }

    /** @brief Returns the z-coordinate of the aliased Point.
     *
     *  This function is a convenience wrapper around `coord(2) const`.
     *
     *  @return The x-coordinate of the aliased Point in a read-only state.
     *
     *  @throw none No throw guarantee.
     */
    const_reference z() const noexcept { return coord(2); }

    operator const Point<value_type>&() const { return m_pimpl_; }

private:
    /// Our aliasing PIMPL wrapped in the Point API for convenience
    Point<value_type> m_pimpl_;
};

extern template class PointView<double>;
extern template class PointView<const double>;
extern template class PointView<float>;
extern template class PointView<const float>;

} // namespace libchemist
