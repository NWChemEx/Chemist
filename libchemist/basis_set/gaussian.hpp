#pragma once
#include "libchemist/basis_set/detail_/utility.hpp"
#include "libchemist/types.hpp"
#include <optional>

namespace libchemist::basis_set {

/** @brief Models a Gaussian primitive
 *
 *  Typically in electronic structure theory we think of the radial part of a
 *  Gaussian primitive, @f$G@f$ as being of the form:
 *  @f[
 *     G(\vec{r};c, \vec{\ell}, \alpha) =
 *     c  g\left(\vec{\ell}\right)\exp{-\alpha \vec{r}^2},
 *  @f]
 *  where @f$\vec{r}@f$ is the Cartesian coordinate of the Gaussian's center,
 *  @f$c@f$ is a scale factor (typically the normalization factor combined with
 *  the contraction coefficient), @f$g\left(\vec{\ell}\right)@f$ is a prefactor
 *  that depends on the angular momentum and whether the primitive is Cartesian
 *  or spherical, and @f$\alpha@f$ is the Gaussian's exponent. That said, since
 *  we usually are interested in sets of Gaussian primitives (typically shells)
 *  it is common to only consider a Gaussian primitive as having the parameters
 *  that are unique to it (@f$c@f$ and @f$\alpha@f$). This class supports both
 *  scenarios by wrapping all parameters aside from the coefficient and the
 *  exponent in std::optional instances.
 *
 *  Additionally virtually every Gaussian integral library will take care of the
 *  @f$g\left(\vec{\ell}\right)@f$ prefactor given:
 *  - whether the primitive is Cartesian or spherical, and
 *  - the magnitude of @f$\vec{\ell}@f$.
 *  This information is typically stored at the shell level and consequentially
 *  not part of this class.
 *
 *
 *  @note Instances of this class are capable of being used as compile-time
 *        literals.
 *
 *  @tparam T The type used to store the coefficent and exponent. Assumed to be
 *            `float` or `double`-like.
 */
template<typename T>
class Gaussian {
public:
    /// The floating-point type used to hold the parameters
    using value_type = T;

    /** @brief Makes a Gaussian that is mathematically equal to 0
     *
     *  The Gaussian instance resulting from the default ctor has a contraction
     *  coefficient of 0, an exponent parameter of 0, and has no center.
     *
     *  @throw none No throw guarantee.
     */
    constexpr Gaussian() = default;

    /** @brief Makes a Gaussian with no center, but a coefficient and exponent
     *
     *  The Gaussian instance resulting from this ctor has a contraction
     *  coefficient equal to @p c, an exponent parameter of @p a, and has no
     *  center.
     *
     *  @tparam CoefType The type of the initial contraction coefficient.
     *                   Must be implicitly convertible to @p T.
     *  @tparam ExpType The type of the initial exponent. Must be implicitly
     *                  convertible to @p T.
     *  @param[in] c The initial value of the contraction coefficient.
     *  @param[in] a The initial value of the exponent.
     *
     *  @throw none No throw guarantee.
     */
    template<typename CoefType, typename ExpType>
    constexpr Gaussian(CoefType&& c, ExpType&& a) noexcept;

    /** @brief Makes a fully specified Gaussian
     *
     *  The Gaussian instance resulting from this ctor has a contraction
     *  coefficient equal to @p c, an exponent parameter of @p a, and a center
     *  equal to @p r.
     *
     *  @tparam CoefType The type of the initial contraction coefficient.
     *                   Must be implicitly convertible to @p T.
     *  @tparam ExpType The type of the initial exponent. Must be implicitly
     *                  convertible to @p T.
     *  @tparam CoordType The input type of the coordinate's initial value. Must
     *                    be implicitly convertible to type::coord<T>.
     *  @param[in] c The initial value of the contraction coefficient.
     *  @param[in] a The initial value of the exponent.
     *  @param[in] r Where the Gaussian is centered in Cartesian space (in a.u.)
     *
     *  @throw none No throw guarantee.
     */
    template<typename CoefType, typename ExpType, typename CoordType>
    constexpr Gaussian(CoefType&& c, ExpType&& a, CoordType&& r) noexcept;

    /** @brief Accessor for the contraction coefficient
     *
     *  The contraction coefficient is @f$c@f$ in:
     *   *  @f[
     *     G(\vec{r};c, \vec{\ell}, \alpha) =
     *     c  g\left(\vec{\ell}\right)\exp{-\alpha \vec{r}^2}.
     *  @f]
     *
     *  @return The contraction coefficient of the Gaussian in a read/write
     *          state.
     *  @throw none No throw guarantee.
     */
    constexpr auto& coefficient() noexcept { return m_coefs_; }

    /** @brief Accessor for the contraction coefficient
     *
     *  The contraction coefficient is @f$c@f$ in:
     *   *  @f[
     *     G(\vec{r};c, \vec{\ell}, \alpha) =
     *     c  g\left(\vec{\ell}\right)\exp{-\alpha \vec{r}^2}.
     *  @f]
     *
     *  @return The contraction coefficient of the Gaussian in a read-only
     *          state.
     *  @throw none No throw guarantee.
     */
    constexpr const auto& coefficient() const noexcept { return m_coefs_; }

    /** @brief Accessor for the Gaussian's exponent
     *
     *  The exponent is @f$\alpha@f$ in:
     *  @f[
     *     G(\vec{r};c, \vec{\ell}, \alpha) =
     *     c  g\left(\vec{\ell}\right)\exp{-\alpha \vec{r}^2}.
     *  @f]
     *
     *  @return The exponent of the Gaussian in a read/write state.
     *
     *  @throw none No throw guarantee.
     */
    constexpr auto& exponent() noexcept { return m_alpha_; }

    /** @brief Accessor for the Gaussian's exponent
     *
     *  The exponent is @f$\alpha@f$ in:
     *  @f[
     *     G(\vec{r};c, \vec{\ell}, \alpha) =
     *     c  g\left(\vec{\ell}\right)\exp{-\alpha \vec{r}^2}.
     *  @f]
     *
     *  @return The exponent of the Gaussian in a read-only state.
     *
     *  @throw none No throw guarantee.
     */
    constexpr const auto& exponent() const noexcept { return m_alpha_; }

    /** @brief Whether this Gaussian instance's coordinates have been set.
     *
     *  Strictly speaking a Gaussian primitive's parameters require knowledge
     *  of the coordinates of the center; however, it is very common to ignore
     *  this aspect. This function allows users to determine if the center has
     *  been set.
     *
     *  @return True if this Gaussian's instance's coordinates have been set.
     *  @throw none No throw guarantee.
     */
    constexpr bool has_center() const noexcept { return m_coord_.has_value(); }

    /** @brief Sets the coordinate to the provided value
     *
     *  If the Gaussian was provided with coordinates at construction than it is
     *  possible to modify the coordinates by calling `center`; however, if the
     *  center was not set during construction users must initially set it with
     *  this function.
     *
     *  @tparam CoordType The input type of the coordinates. Must be implicitly
     *                    convertible to `type::coord<T>`.
     *  @param[in] coord Where the Gaussian will be centered.
     *
     *  @throw none No throw guarantee.
     */
    template<typename CoordType>
    void set_center(CoordType&& coord) noexcept;

    /** @brief Accessor for the Gaussian's center
     *
     *  The center is @f$\vec{r}@f$ in:
     *  @f[
     *     G(\vec{r};c, \vec{\ell}, \alpha) =
     *     c  g\left(\vec{\ell}\right)\exp{-\alpha \vec{r}^2}.
     *  @f]
     *
     *  @return The Cartesian coordinates of the Gaussian, in a read/write
     *          state. Coordinate values are in atomic units.
     *
     *  @throw std::bad_optional_access if the coordinates have not been set.
     *                                  Strong throw guarantee.
     */
    constexpr auto& center() { return m_coord_.value(); }

    /** @brief Accessor for the Gaussian's center
     *
     *  The center is @f$\vec{r}@f$ in:
     *  @f[
     *     G(\vec{r};c, \vec{\ell}, \alpha) =
     *     c  g\left(\vec{\ell}\right)\exp{-\alpha \vec{r}^2}.
     *  @f]
     *
     *  @return The Cartesian coordinates of the Gaussian, in a read-only state.
     *          Coordinate values are in atomic units.
     *
     *  @throw std::bad_optional_access if the coordinates have not been set.
     *                                  Strong throw guarantee.
     */
    constexpr const auto& center() const { return m_coord_.value(); }

private:
    /// The contraction coefficient of the Gaussian
    T m_coefs_ = 0.0;
    /// The exponent parameter of the Gaussian
    T m_alpha_ = 0.0;
    /// Where in Cartesian space the Gaussian is centered
    std::optional<type::coord<T>> m_coord_;
};
/// Allow no-center ctor to deduce template parameter
template<typename CoefType, typename ExpType>
Gaussian(CoefType&&,
         ExpType&&) noexcept->Gaussian<std::common_type_t<CoefType, ExpType>>;

/// Allow center ctor to deduce template parameter
template<typename CoefType, typename ExpType, typename CoordType>
Gaussian(CoefType&&, ExpType&&,
         CoordType&&) noexcept->Gaussian<std::common_type_t<CoefType, ExpType>>;

/** @brief Determines if a Gaussian instance is the same as another object.
 *  @relates Gaussian
 *
 *  Compares a `Gaussian<LHS>` instance to an instance of type @p RHS. The two
 *  instances are the same if the results of their:
 *  - `coefficient`,
 *  - `exponent`, and
 *  - `center`
 *  functions are the same.
 *
 *  @tparam LHS The type of the coefficients and exponents in @p lhs. Assumed to
 *              be either `float` or `double`.
 * @tparam RHS  The type of @p rhs. @p RHS is expected to be a class that
 *              defines the same interface as the `Gaussian` class, but need not
 *              be a `Gaussian` instance.
 * @param lhs The instance on the left side of the equality operator.
 * @param rhs The instance on the right side of the equality operator.
 * @return True if the two instances are equal and false otherwise.
 * @throw None no throw guarantee.
 */
template<typename LHS, typename RHS>
constexpr bool operator==(const Gaussian<LHS>& lhs, const RHS& rhs) noexcept;

/** @brief Determines if a Gaussian instance differs from another object.
 *  @relates Gaussian
 *
 *  Compares a `Gaussian<LHS>` instance to an instance of type @p RHS. The two
 *  instances are the same if the results of their:
 *  - `coefficient`,
 *  - `exponent`, and
 *  - `center`
 *  functions are the same.
 *
 *  @tparam LHS The type of the coefficients and exponents in @p lhs. Assumed to
 *              be either `float` or `double`.
 * @tparam RHS  The type of @p rhs. @p RHS is expected to be a class that
 *              defines the same interface as the `Gaussian` class, but need not
 *              be a `Gaussian` instance.
 * @param lhs The instance on the left side of the inequality operator.
 * @param rhs The instance on the right side of the inequality operator.
 * @return False if the two instances are equal and true otherwise.
 * @throw None no throw guarantee.
 */
template<typename LHS, typename RHS>
constexpr bool operator!=(const Gaussian<LHS>& lhs, const RHS& rhs) noexcept;

//-------------------------Implementations--------------------------------------
template<typename T>
template<typename CoefType, typename ExpType>
constexpr Gaussian<T>::Gaussian(CoefType&& c, ExpType&& a) noexcept :
  Gaussian(std::forward<CoefType>(c), std::forward<ExpType>(a), std::nullopt) {}

template<typename T>
template<typename CoefType, typename ExpType, typename CoordType>
constexpr Gaussian<T>::Gaussian(CoefType&& c, ExpType&& a,
                                CoordType&& r) noexcept :
  m_coefs_(std::forward<CoefType>(c)),
  m_alpha_(std::forward<ExpType>(a)),
  m_coord_(std::forward<CoordType>(r)) {}

template<typename T>
template<typename CoordType>
void Gaussian<T>::set_center(CoordType&& coord) noexcept {
    m_coord_.emplace(std::forward<CoordType>(coord));
}

template<typename LHS, typename RHS>
constexpr bool operator==(const Gaussian<LHS>& lhs, const RHS& rhs) noexcept {
    return detail_::compare_gaussians(lhs, rhs);
}

template<typename LHS, typename RHS>
constexpr bool operator!=(const Gaussian<LHS>& lhs, const RHS& rhs) noexcept {
    return !(lhs == rhs);
}
} // namespace libchemist::basis_set
