#pragma once
#include "libchemist/basis_set/detail_/gaussian_traits.hpp"
#include "libchemist/basis_set/detail_/holder.hpp"
#include <tuple>

namespace libchemist::basis_set::detail_ {

/** @brief Gaussian primitive that assumes a primitive has an exponent, weight,
 *         and a Cartesian center.
 */
template<typename Traits>
class GaussianPIMPL {
public:
    /** @brief Constructs a Gaussian_ object with the provided state
     *
     *  Users of this class are expected to go through the `make_gaussian`
     *  factory function. Whether this
     *
     *  @tparam CoefType The type of the provided coefficient, must be
     *                   implicitly convertable to the type `Traits::coef_type`.
     *  @tparam ExpType The type of the provided exponent, must be
     *                  implicitly convertable to the type `Traits::exp_type`.
     *  @tparam CenterType The type of the provided center, must be
     *                     implicitly convertable to the type
     *                    `Traits::center_type`.
     *
     *  @param[in] c The contraction coefficient for this Gaussian.
     *  @param[in] a The exponent for this Gaussian
     *  @param[in] r0 The Cartesian coordinates for this Gaussian.
     *
     *  @throw none No throw guarantee.
     */
    template<typename CoefType, typename ExpType, typename CenterType>
    constexpr GaussianPIMPL(CoefType&& c, ExpType&& a,
                            CenterType&& r0) noexcept;

    template<typename Traits2>
    Gaussian_

      /** @brief Provides (possibly) read/write access to the contraction
       *         coefficient.
       *
       *  This member function returns the contraction coefficient of the
       *  Gaussian. If `Traits::coef_type` is non-const then the coefficient is
       *  read-/write-able, if it is const then the coefficient is only
       * readable.
       *
       * @return The coefficient of the Gaussian.
       *
       * @throw none No throw guarantee.
       */
      constexpr auto&
      coef() noexcept {
        return *m_coef_;
    }

    /** @brief Provides read-only access to the contraction
     *         coefficient.
     *
     *  This member function returns the contraction coefficient of the
     *  Gaussian. The coefficient is read-only regardless of the type of
     *  `Traits::coef_type`.
     *
     * @return The coefficient of the Gaussian.
     *
     * @throw none No throw guarantee.
     */
    constexpr const auto& coef() const noexcept { return *m_coef_; }

    /** @brief Provides (possibly) read/write access to the exponent.
     *
     *  This member function returns the exponent of the Gaussian.
     *  If `Traits::exp_type` is non-const then the exponent is
     *  read-/write-able, if it is const then the exponent is only readable.
     *
     * @return The exponent of the Gaussian.
     *
     * @throw none No throw guarantee.
     */
    constexpr auto& exp() noexcept { return *m_exp_; }

    /** @brief Provides read-only access to the exponent.
     *
     *  This member function returns the exponent of the Gaussian. The exponent
     *  is read-only regardless of the type of  `Traits::exp_type`.
     *
     * @return The exponent of the Gaussian.
     *
     * @throw none No throw guarantee.
     */
    constexpr const auto& exp() const noexcept { return *m_exp_; }

    /** @brief Provides (possibly) read/write access to the coordinates.
     *
     *  This member function returns the Cartesian coordinates of the Gaussian.
     *  If `Traits::center_type` is non-const then the coordinates are
     *  read-/write-able, if it is const then the coordinates are only readable.
     *
     *
     * @return The Cartesian coordinates of the Gaussian's center (in a.u.).
     *
     * @throw none No throw guarantee.
     */
    constexpr auto& center() noexcept { return *m_center_; }

    /** @brief Provides read-only access to the coordinates.
     *
     *  This member function returns the Cartesian coordinates of the Gaussian.
     *  Regardless of the type `Traits::center_type`, the returned coordinates
     *  are read-only.
     *
     * @return The Cartesian coordinates of the Gaussian's center (in a.u.).
     *
     * @throw none No throw guarantee.
     */
    constexpr const auto& center() const noexcept { return *m_center_; }

private:
    using coef_holder = detail_::holder<typename Traits::coef_type>;
    using exp_holder  = detail_::holder<typename Traits::exp_type>;

    /// The instance holding the contraction coefficient
    m_coef_;
    /// The instance holding the exponent
    m_exp_;
    /// The instance holding the Cartesian coordinates
    detail_::holder<typename Traits::center_type> m_center_;
}; // class Gaussian_

/// Typedef of Gaussian_ that sets all types off of a floating-point type @p T
template<typename T>
using Gaussian = Gaussian_<detail_::GaussianTraits<T>>;

/** @brief Factory function for creating a Gaussian_ instance.
 *
 *  @relates Gaussian_
 *
 *  This function will analyze the types of the provided arguments and determine
 *  if the Gaussian should own the values or alias them based on the types of
 *  the arguments. Specifically, a value will be aliased if it is pointer-like
 *  and owned if it is value-like. The resulting value/alias will be read-only
 *  if the provided value is const.
 *
 *
 * @tparam CoefType The type to use to store the coefficient.
 * @tparam ExpType The type to use to store the exponent
 * @tparam CenterType The type to use to store the center
 *
 * @param[in] c The value of the coefficient
 * @param[in] a The value of the exponent
 * @param[in] r0 The value of the center
 * @return[in] The created Gaussian_ instance
 *
 * @throw none No throw guarantee.
 */
template<typename CoefType, typename ExpType, typename CenterType>
constexpr auto make_gaussian(CoefType&& c, ExpType&& a,
                             CenterType&& r0) noexcept;

/** @brief Compares two Gaussian instances for equality.
 *
 *  @relates Gaussian_
 *
 *  Two Gaussians are equal if the values of their contraction coefficient,
 *  exponent, and Cartesian coordinates compare equal. Thus no consideration is
 *  given to whether or not the Gaussians alias their values.
 *
 *  @tparam Traits1 The GaussianTraits_ type associated with the Gaussian_ on
 *                  the left side of the equality operator.
 * @tparam Traits2  The GaussianTraits_ type associated with the Gaussian_ on
 *                  the right side of the equality operator.
 * @param[in] lhs The Gaussian_ instance on the left side of the equality
 *                operator.
 * @param[in] rhs The Gaussian_ instance on the right side of the equality
 *                operator.
 * @return True if the two instances compare equal and false otherwise.
 * @throw none No throw guarantee.
 */
template<typename Traits1, typename Traits2>
constexpr auto operator==(const Gaussian_<Traits1>& lhs,
                          const Gaussian_<Traits2>& rhs) noexcept;

/** @brief Compares two Gaussian instances for inequality.
 *
 *  @relates Gaussian_
 *
 *  Two Gaussians are equal if the values of their contraction coefficient,
 *  exponent, and Cartesian coordinates compare equal. Thus no consideration is
 *  given to whether or not the Gaussians alias their values.
 *
 *  @tparam Traits1 The GaussianTraits_ type associated with the Gaussian_ on
 *                  the left side of the inequality operator.
 *  @tparam Traits2 The GaussianTraits_ type associated with the Gaussian_ on
 *                  the right side of the inequality operator.
 *  @param[in] lhs  The Gaussian_ instance on the left side of the inequality
 *                  operator.
 *  @param[in] rhs  The Gaussian_ instance on the right side of the inequality
 *                  operator.
 * @return True if the two instances do not compare equal and false otherwise.
 * @throw none No throw guarantee.
 */
template<typename Traits1, typename Traits2>
constexpr auto operator!=(const Gaussian_<Traits1>& lhs,
                          const Gaussian_<Traits2>& rhs) noexcept {
    return !(lhs == rhs);
}

//-------------------------------Implementations--------------------------------

template<typename Traits>
template<typename CoefType, typename ExpType, typename CenterType>
constexpr Gaussian_<Traits>::Gaussian_(CoefType&& c, ExpType&& a,
                                       CenterType&& r0) noexcept :
  m_coef_(std::forward<CoefType>(c)),
  m_exp_(std::forward<ExpType>(a)),
  m_center_(std::forward<CenterType>(r0)) {}

template<typename CoefType, typename ExpType, typename CenterType>
constexpr auto make_gaussian(CoefType&& c, ExpType&& a,
                             CenterType&& r0) noexcept {
    using clean_coef   = std::remove_reference_t<CoefType>;
    using clean_exp    = std::remove_reference_t<ExpType>;
    using clean_center = std::remove_reference_t<CenterType>;
    using traits =
      detail_::GaussianTraits_<clean_coef, clean_exp, clean_center>;

    return Gaussian_<traits>(std::forward<CoefType>(c),
                             std::forward<ExpType>(a),
                             std::forward<CenterType>(r0));
}

template<typename Traits1, typename Traits2>
constexpr auto operator==(const Gaussian_<Traits1>& lhs,
                          const Gaussian_<Traits2>& rhs) noexcept {
    return std::tie(lhs.coef(), lhs.exp(), lhs.center()[0], lhs.center()[1],
                    lhs.center()[2]) ==
           std::tie(rhs.coef(), rhs.exp(), rhs.center()[0], rhs.center()[1],
                    rhs.center()[2]);
}

} // namespace libchemist::basis_set::detail_
