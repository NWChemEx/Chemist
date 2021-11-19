/** @file point_pimpl.hpp
 *
 *  point_pimpl.hpp contains the definition of the PIMPL for the Point class.
 *  It is assumed that this file will only be included from source files and
 *  thus the API of the PointPIMPL class is subject to change at any time.
 */
#pragma once
#include "chemist/point/point.hpp"
#include <array>
#include <stdexcept>
#include <string>
#include <utilities/containers/own_or_borrow.hpp>

namespace chemist::detail_ {

/** @brief Holds the Point class's state
 *
 *  This class is responsible for holding the Point class's state and decoupling
 *  the storage mechanism from the Point class's API.
 *
 *  @tparam T The type used for holding the point's coordinates. Assumed to be
 *            a non-cv qualified POD of floating-point variety.
 */
template<typename T>
class PointPIMPL {
public:
    /// Type of the point this PIMPL is associated with
    using point_type = Point<T>;
    /// Type of a read/write reference to a coordinate of the point
    using reference = typename point_type::reference;
    /// Type of a read-only reference to a coordinate of the point
    using const_reference = typename point_type::const_reference;
    /// Type of an index or offset, assumed to be integral and POD-like
    using size_type = typename point_type::size_type;

    /** @brief Creates a 3D Cartesian point at the origin.
     *
     *  This function is used to create a 3D Cartesian point at the origin. The
     *  point can be translated by calling `coord` and modifying the resulting
     *  reference.
     *
     *  @throw none No throw guarantee.
     *
     *  Complexity: constant
     */
    PointPIMPL() noexcept = default;

    /** @brief Makes a PointPIMPL that is a deep copy of another PointPIMPL.
     *
     *  The copy ctor will initialize a new PointPIMPL instance that is a deep
     *  copy of another PointPIMPL regardless of whether the other PointPIMPL
     *  owns or is aliasing its state.
     *
     *  @param[in] rhs The PointPIMPL to copy the state of.
     *
     *  @throw none No throw guarantee.
     *
     *  Complexity: constant
     */
    PointPIMPL(const PointPIMPL<T>& rhs) noexcept;

    /** @brief Constructs a new PointPIMPL by moving the state from another
     *         instance.
     *
     *  Assuming @p T is a POD this is basically the same as the copy ctor,
     *  except that if @p rhs is aliasing, then the resulting instance will
     *  alias as well.
     *
     * @param[in,out] rhs The instance to move the state from. If @p T is a POD
     *                @p rhs will be in the same state after this call.
     *
     * @throw none No throw guarantee.
     */
    PointPIMPL(PointPIMPL<T>&& rhs) noexcept = default;

    /** @brief Makes this PointPIMPL a deep copy of another PointPIMPL.
     *
     *  The copy assignment operator will set the current PointPIMPL's state to
     *  a deep copy of another PointPIMPL's state, regardless of whether the
     *  other PointPIMPL owns or is aliasing its state.
     *
     *  @param[in] rhs The PointPIMPL to copy the state of.
     *
     *  @return The current instance after setting its state to a copy of
     *          @p rhs's state.
     *  @throw none No throw guarantee.
     */
    PointPIMPL<T>& operator=(const PointPIMPL<T>& rhs) noexcept;

    /** @brief Moves the state from anothe instance into the current PIMPL.
     *
     *  Assuming @p T is a POD this is basically the same as the copy ctor,
     *  except that if @p rhs is aliasing, then the resulting instance will
     *  alias as well.
     *
     * @param[in,out] rhs The instance to move the state from. If @p T is a POD
     *                @p rhs will be in the same state after this call.
     *
     * @return The current instance with its state set to @p rhs's state.
     *
     * @throw none No throw guarantee.
     */
    PointPIMPL<T>& operator=(PointPIMPL<T>&& rhs) noexcept = default;

    /** @brief Constructs a new PointPIMPL with the initial state taken from
     *         the arguments.
     *
     *  This ctor can be used to create a PointPIMPL that owns the memory
     *  associated with 0 to three of its components. The remaining components
     *  will be stored as aliases. Whether a component is owned or aliased
     *  depends on whether the user provides the value of the component by value
     *  or pointer (the former leads to the instance owning the memory and the
     *  latter to aliasing).
     *
     * @tparam X The type of the provided x-coordinate. Should be some
     *           cv-qualified variant of T or T*.
     * @tparam Y The type of the provided y-coordinate. Should be some
     *           cv-qualified variant of T or T*.
     * @tparam Z The type of the provided z-coordinate. Should be some
     *           cv-qualified variant of T or T*.
     * @param[in] x The value, or a pointer to the value, of the resulting
     *              instance's x-coordinate.
     * @param y The value, or a pointer to the value, of the resulting
     *              instance's y-coordinate.
     * @param z The value, or a pointer to the value, of the resulting
     *              instance's z-coordinate.
     *
     * @throw none No throw guarantee.
     */
    template<typename X, typename Y, typename Z>
    PointPIMPL(X&& x, Y&& y, Z&& z) noexcept;

    /** @brief Returns the @p i-th component of the point.
     *
     *  @param[in] i Which component to return? @p i should be in the range
     *             [0, 3).
     *
     *  @return A read/write reference to the requested component.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, 3). Strong
     *         throw guarantee.
     */
    reference coord(size_type i);

    /** @brief Returns the @p i-th component of the point as read-only.
     *
     *  @param[in] i Which component to return? @p i should be in the range
     *             [0, 3).
     *
     *  @return A read-only reference to the requested component.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, 3). Strong
     *         throw guarantee.
     */
    const_reference coord(size_type i) const;

private:
    /// Encapsulates throwing if the bounds check fails
    void check_index_(size_type i) const;

    /// The actual set of coordinates
    std::array<utilities::OwnOrBorrow<T>, 3> m_qs_;
}; // class PointPIMPL

// -------------------------Implementations-----------------------------------

/// Cleans-up the template<typename T> that appears in the following defs
#define POINT_PIMPL_PARAMS template<typename T>
/// Cleans-up the PointPIMPL<T> that appears in the following defs
#define POINT_PIMPL PointPIMPL<T>

POINT_PIMPL_PARAMS
POINT_PIMPL::PointPIMPL(const POINT_PIMPL& rhs) noexcept :
  PointPIMPL(rhs.coord(0), rhs.coord(1), rhs.coord(2)) {}

POINT_PIMPL_PARAMS
POINT_PIMPL& POINT_PIMPL::operator=(const POINT_PIMPL& rhs) noexcept {
    *this = std::move(PointPIMPL<T>(rhs));
    return *this;
}

POINT_PIMPL_PARAMS
template<typename X, typename Y, typename Z>
POINT_PIMPL::PointPIMPL(X&& x, Y&& y, Z&& z) noexcept :
  m_qs_{utilities::OwnOrBorrow<T>{std::forward<X>(x)},
        utilities::OwnOrBorrow<T>{std::forward<Y>(y)},
        utilities::OwnOrBorrow<T>{std::forward<Z>(z)}} {}

// -------------------------- Accessors-----------------------------------------

POINT_PIMPL_PARAMS
typename POINT_PIMPL::reference POINT_PIMPL::coord(size_type i) {
    return const_cast<reference>(std::as_const(*this).coord(i));
}

POINT_PIMPL_PARAMS
typename POINT_PIMPL::const_reference POINT_PIMPL::coord(size_type i) const {
    check_index_(i);
    return m_qs_[i].value();
}

// -------------------------- Private Fxns -------------------------------------

POINT_PIMPL_PARAMS
void POINT_PIMPL::check_index_(size_type i) const {
    if(i < 3) return;
    const auto msg = std::string("Index i == ") + std::to_string(i) +
                     " is not in the range [0, 3).";
    throw std::out_of_range(msg);
}

#undef POINT_PIMPL
#undef POINT_PIMPL_PARAMS

} // namespace chemist::detail_
