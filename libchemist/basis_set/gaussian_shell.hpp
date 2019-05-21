#pragma once
#include "libchemist/basis_set/detail_/bs_iterator.hpp"
#include "libchemist/basis_set/detail_/gaussian_shell_traits.hpp"
#include "libchemist/basis_set/detail_/holder.hpp"
#include "libchemist/basis_set/gaussian_ao.hpp"
#include <utilities/iter_tools/range_container.hpp>

namespace libchemist::basis_set {
/** @brief A shell of Gaussian-based AOs.
 *
 *  A Gaussian shell is a set of Gaussian-based AOs that are all centered at the
 *  same point in space and have the same magnitude of angular momentum.
 *  Typically in electronic structure theory, each AO has the same radial part,
 *  but ad different angular parts. The angular part is not explicitly stored,
 *  rather what is stored is whether that angular part is spherical or
 *  Cartesian and it is up to the integrals library to account for it.
 *
 * @tparam Traits The type containing the types of this shell's members. It
 *                minimally must include: `pure_flag`, `am_type`,
 *                `coef_container`, `exp_container`, and `center_type, which
 *                respectively are the types of the toggle for the angular
 *                part, the type used to store the angular momentum, the type
 *                of the container holding the contraction coefficients, the
 *                type of the container holding the exponents, and the type of
 *                the Cartesian coordinates.
 *
 */
template<typename Traits>
class GaussianShell_ {
public:
    /** @brief Creates a Gaussian-based shell from the provided parameters
     *
     *  This class assumes that all AOs in the shell have the same contraction
     *  coefficients and exponents, hence users should only provide the
     *  the set of coefficients and exponents for one AO. Users are encouraged
     *  to use the `make_gaussian_shell` factory function to create a new shell.
     *
     * @tparam AMType The type of the angular momentum passed to this function.
     *                Must be implicitly convertible to `Traits::am_type`.
     * @tparam CoefCont The type of the container holding the contraction
     *                  coefficients. Must be implicitly convertible to
     *                  `Traits::coef_container`.
     * @tparam ExpCont  The type of the container holding the exponents. Must
     *                  be implicitly convertible to `Traits::exp_container`.
     * @tparam CenterType The type of the Cartesian coordinates. Must be
     *                    implicitly convertible to `Traits::center_type`
     * @tparam PureFlag The type of the purity flag. Must be implicitly
     *                  convertible to `Traits::pure_flag`.
     * @param[in] am The magnitude of the shell's angular momentum
     * @param[in] cs The contraction coefficients.
     * @param[in] as The exponents
     * @param[in] r0 The Cartesian point where the shell is centered.
     * @param[in] is_pure Whether this is spherical or Cartesian shell
     *
     * @throw none No throw guarantee.
     */
    template<typename AMType, typename CoefCont, typename ExpCont,
             typename CenterType, typename PureFlag>
    constexpr GaussianShell_(AMType&& am, CoefCont&& cs, ExpCont&& as,
                             CenterType&& r0, PureFlag&& is_pure) noexcept;

    /** @brief The number of basis functions in this shell.
     *
     *  The number of AOs in this shell is @f$2\ell + 1@f$ if the shell is pure
     *  and @f${\ell + 2 \choose 2}@f$ if it is Cartesian.
     *
     *  @return The number of basis functions in this shell.
     *  @throw none No throw guarantee.
     */
    constexpr type::size size() const noexcept;

    /** @brief The magnitude of the shell's angular momentum in a (possibly)
     *         read-/write-able state.
     *
     *
     *  @return The magnitude of the shell's angular momentum.
     *  @throw none No throw guarantee.
     */
    constexpr auto& l() noexcept { return *m_l_; }

    /** @brief The magnitude of the shell's angular momentum in a read-only
     *         state.
     *
     *  @return The magnitude of the shell's angular momentum.
     *  @throw none No throw guarantee.
     */
    constexpr const auto l() const noexcept { return *m_l_; }

    /** @brief The Cartesian coordinates of the shell's center in a (possibly)
     *         read-/write-able state.
     *
     *
     *  @return The Cartesian coordinates of the shell's center (in a.u.)
     *  @throw none No throw guarantee.
     */
    constexpr auto& center() noexcept { return *m_center_; }

    /** @brief The Cartesian coordinates of the shell's center in a read-only
     *         state.
     *
     *  @return The Cartesian coordinates of the shell's center (in a.u.)
     *  @throw none No throw guarantee.
     */
    constexpr const auto& center() const noexcept { return *m_center_; }

    /** @brief Whether this shell is spherical or Cartesian, in a (possibly)
     *         read-/write-able state.
     *
     *
     *  @return The purity of the shell.
     *  @throw none No throw guarantee.
     */
    constexpr auto& pure() noexcept { return *m_pure_; }

    /** @brief Whether this shell is spherical or Cartesian in a read-only
     *         state.
     *
     *  @return The purity of the shell.
     *  @throw none No throw guarantee.
     */
    constexpr const auto& pure() const noexcept { return *m_pure_; }

    /** @brief Returns a read-only AO from this shell.
     *
     * @param[in] i The AO to return. @p i must be in the range [0, size()).
     *
     * @return The requested AO.
     * @throw none No throw guarantee.
     */
    constexpr auto operator[](type::size) const noexcept;

    auto begin() const { return detail_::make_bs_iterator(0, this); }
    auto end() const { return detail_::make_bs_iterator(size(), this); }
    auto cbegin() const { return begin(); }
    auto cend() const { return end(); }

private:
    /** @brief Used in the ctor if the shell is pure to compute the binomial
     *         coefficient.
     *
     *  This function uses recursion to compute the binomial coefficient and
     *  will not overflow for any reasonable angular momentum
     *  (values up to @f$\ell \approx 6,000,000,000@f$ are valid).
     *
     * @param n The top number of the binomial coefficient
     * @param m The bottom number of the binomial coefficient
     * @return The number of ways of choosing @p m things from @p n things.
     * @throw none No throw guarantee (even if overflow occurs no throw is
     *             issued).
     */
    constexpr type::size binomial_coef_(type::size n, type::size m) const
      noexcept;

    /// Whether the shell is pure or spherical
    detail_::holder<typename Traits::pure_flag> m_pure_;
    /// The magnitude of the shell's angular momentum
    detail_::holder<typename Traits::am_type> m_l_;
    /// The contraction coefficients
    detail_::holder<typename Traits::coef_container> m_cs_;
    /// The exponents of the primitives
    detail_::holder<typename Traits::exp_container> m_as_;
    /// The point in space where this shell is centered
    detail_::holder<typename Traits::center_type> m_center_;
};

/// Type of a Gaussian shell with a dynamic number of primitives
template<typename T>
using GaussianShell = GaussianShell_<detail_::GaussianShellTraits<T>>;

/// Type of a Gaussian shell whose number of primitives is known at compile-time
template<typename T, type::size N>
using StaticGaussianShell =
  GaussianShell_<detail_::StaticGuassianShellTraits<T, N>>;

/** @brief Factory function for creating a Gaussian shell from parameters.
 *
 *  @relates GaussianShell_
 *
 *  This function deduces the types needed for the GaussianShellTraits_ class
 *  and returns a GaussianShell_ instance with the provided parameters.
 *  Parameters are aliased if they are pointer-like and read-only if they are
 *  const.
 *
 * @tparam AMType The type of the incoming angular momentum
 * @tparam CoefArray The type of container holding the incoming contraction
 *                   coefficients.
 * @tparam ExpArray  The type of the container holding the incoming exponents.
 * @tparam CenterType The type of the incoming Cartesian coordinates.
 * @tparam PureFlag The type of the incoming purity toggle
 * @param[in] l The initial angular momentum of the shell.
 * @param[in] cs The contraction coefficients for the primitives.
 * @param[in] as The expansion coefficients for the primitives.
 * @param[in] r0 The location of the shell in Cartesian space.
 * @param[in] is_pure Whether the shell is spherical or Cartesian.
 *
 * @return A GaussianShell_ instance with the provided parameters.
 * @throw none No throw guarantee.
 */
template<typename AMType, typename CoefArray, typename ExpArray,
         typename CenterType, typename PureFlag>
constexpr auto make_gaussian_shell(AMType&& l, CoefArray&& cs, ExpArray&& as,
                                   CenterType&& r0,
                                   PureFlag&& is_pure) noexcept;

/** @brief Compares two GaussianShell_ instances for equality
 *
 *  @relates GaussianShell_
 *
 *  Two GaussianShell_ instances are equal if they are the same purity, have
 *  the same magnitude of angular momentum, and the AOs are equal.
 *
 * @tparam Traits1 The types of the members in @p lhs
 * @tparam Traits2  The types of the members in @p rhs
 * @param[in] lhs The instance on the left side of the equality operator
 * @param[in] rhs The instance on the right side of the equality operator
 * @return True if the two instances are equal and false otherwise.
 * @throw none No throw guarantee.
 */
template<typename Traits1, typename Traits2>
constexpr bool operator==(const GaussianShell_<Traits1>& lhs,
                          const GaussianShell_<Traits2>& rhs) noexcept;

/** @brief Compares two GaussianShell_ instances for inequality
 *
 *  @relates GaussianShell_
 *
 *  Two GaussianShell_ instances are equal if they are the same purity, have
 *  the same magnitude of angular momentum, and the AOs are equal.
 *
 * @tparam Traits1 The types of the members in @p lhs
 * @tparam Traits2  The types of the members in @p rhs
 * @param[in] lhs The instance on the left side of the inequality operator
 * @param[in] rhs The instance on the right side of the inequality operator
 * @return True if the two instances are not equal and false otherwise.
 * @throw none No throw guarantee.
 */
template<typename Traits1, typename Traits2>
constexpr bool operator!=(const GaussianShell_<Traits1>& lhs,
                          const GaussianShell_<Traits2>& rhs) noexcept;

//----------------------------------Implementations-----------------------------
/// Macro to make shell type more readable
#define SHELL_TYPE GaussianShell_<Traits>

template<typename Traits>
template<typename AMType, typename CoefCont, typename ExpCont,
         typename CenterType, typename PureFlag>
constexpr SHELL_TYPE::GaussianShell_(AMType&& am, CoefCont&& cs, ExpCont&& as,
                                     CenterType&& r0,
                                     PureFlag&& is_pure) noexcept :
  m_pure_(std::forward<PureFlag>(is_pure)),
  m_l_(std::forward<AMType>(am)),
  m_cs_(std::forward<CoefCont>(cs)),
  m_as_(std::forward<ExpCont>(as)),
  m_center_(std::forward<CenterType>(r0)) {}

template<typename Traits>
constexpr type::size SHELL_TYPE::size() const noexcept {
    return pure() ? 2ul * l() + 1 : binomial_coef_(l() + 2ul, 2ul);
}

template<typename Traits>
constexpr auto SHELL_TYPE::operator[](type::size) const noexcept {
    using namespace utilities::detail_;

    return make_gaussian_ao(
      RangeContainer{m_cs_->begin(), m_cs_->end(), m_cs_->size()},
      RangeContainer{m_as_->begin(), m_as_->end(), m_as_->size()}, &center());
}

template<typename Traits>
constexpr type::size SHELL_TYPE::binomial_coef_(type::size n,
                                                type::size m) const noexcept {
    if(m == 0) return 1ul;
    if(n == 0) return 0ul;
    return binomial_coef_(n - 1, m) + binomial_coef_(n - 1, m - 1);
}

template<typename AMType, typename CoefCont, typename ExpCont,
         typename CenterType, typename PureFlag>
constexpr auto make_gaussian_shell(AMType&& am, CoefCont&& cs, ExpCont&& as,
                                   CenterType&& r0,
                                   PureFlag&& is_pure) noexcept {
    using clean_am     = std::remove_reference_t<AMType>;
    using clean_coefs  = std::remove_reference_t<CoefCont>;
    using clean_exps   = std::remove_reference_t<ExpCont>;
    using clean_center = std::remove_reference_t<CenterType>;
    using clean_flag   = std::remove_reference_t<PureFlag>;
    using traits =
      detail_::GaussianShellTraits_<clean_coefs, clean_exps, clean_center,
                                    clean_am, clean_flag>;
    return GaussianShell_<traits>(
      std::forward<AMType>(am), std::forward<CoefCont>(cs),
      std::forward<ExpCont>(as), std::forward<CenterType>(r0),
      std::forward<PureFlag>(is_pure));
}

template<typename Traits1, typename Traits2>
constexpr bool operator==(const GaussianShell_<Traits1>& lhs,
                          const GaussianShell_<Traits2>& rhs) noexcept {
    if(lhs.size() != rhs.size()) return false;
    if(lhs.pure() != rhs.pure()) return false;
    if(lhs.l() != rhs.l()) return false;
    return lhs[0] == rhs[0];
}

template<typename Traits1, typename Traits2>
constexpr bool operator!=(const GaussianShell_<Traits1>& lhs,
                          const GaussianShell_<Traits2>& rhs) noexcept {
    return !(lhs == rhs);
}

#undef SHELL_TYPE

} // namespace libchemist::basis_set
