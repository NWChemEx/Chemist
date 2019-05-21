#pragma once
#include "libchemist/basis_set/detail_/bs_iterator.hpp"
#include "libchemist/basis_set/detail_/gaussian_ao_traits.hpp"
#include "libchemist/basis_set/detail_/holder.hpp"
#include <tuple>

namespace libchemist::basis_set {

/** @brief Models an atomic-orbital that is a linear combination of Gaussian
 *         primitives.
 *
 *  (The radial part of) each atomic orbital (AO) basis function is usually made
 *  up of a linear combination of Gaussian primitives. The Gaussian primitives
 *  necessarily have the same center and this class factors that out. For
 *  efficiency reasons this class also factors the coefficients and exponents
 *  out of each primitive and stores them contiguously.
 *
 *  This class is capable of being used as a compile-time constant.
 *
 *  @note The class's name has an underscore suffix because users should either
 *        use the `make_gaussian_ao` factory function, or the GaussianAO
 *        typedef.
 *
 *  @tparam Traits The GassianAOTraits_ type defining the types of the member
 *                 variables. It must include types: `coef_container`,
 *                 `exp_container`, and `center_type`, which respectively will
 *                 be used as the types of the container holding the contraction
 *                 coefficients, the exponents, and the center.
 */
template<typename Traits>
class GaussianAO_ {
public:
    /** @brief Constructs an AO that is a linear combination of the Gaussian
     *         primitives described by the provided parameters.
     *
     *  This ctor makes a GaussianAO with the provided state. Whether the state
     *  is owned or aliased, and read-/write-able or read-only is controlled by
     *  the types in `Traits`. Users are encouraged to use the
     *  `make_gaussian_ao` factory function to create a GaussianAO_.
     *
     *  @note This function does no error-checking. It is the responsability of
     *        the user to ensure all input is valid.
     *
     * @tparam CoefCont The type of the container holding the coefficients
     * @tparam ExpCont  The type of the container holding the exponents
     * @tparam CenterType The type holding the center
     * @param[in] cs The container of coefficients, such that `cs[i]` is the
     *               contraction coefficient associated with primitive `i`.
     *               Must be implicitly convertible to `Traits::coef_container`.
     * @param[in] as The container of exponents, such that `as[i]` is the
     *               contraction coefficient associated with primitive `i`.
     *               Must be implicitly convertible to `Traits::exp_container`.
     * @param[in] r0 The Cartesian coordinates of the center. Must be implicitly
     *               convertible to `Traits::center_type`.
     *
     * @throw none No throw guarantee.
     */
    template<typename CoefCont, typename ExpCont, typename CenterType>
    constexpr GaussianAO_(CoefCont&& cs, ExpCont&& as,
                          CenterType&& r0) noexcept;

    /** @brief Returns the number of primitives in this AO.
     *
     *  @return The number of primitives in this AO
     *
     *  @throw none No throw guarantee.
     */
    constexpr auto size() const noexcept { return (*m_cs_).size(); }

    /** @brief Provides (possibly) read/write access to the coordinates.
     *
     *  This member function returns the Cartesian coordinates of the AO.
     *  If `Traits::center_type` is non-const then the coordinates are
     *  read-/write-able, if it is const then the coordinates are only readable.
     *
     *
     * @return The Cartesian coordinates of the AO's center (in a.u.).
     *
     * @throw none No throw guarantee.
     */
    constexpr auto& center() noexcept { return *m_center_; }

    /** @brief Provides read-only access to the coordinates.
     *
     *  This member function returns the Cartesian coordinates of the AO.
     *  Regardless of the type `Traits::center_type`, the returned coordinates
     *  are read-only.
     *
     * @return The Cartesian coordinates of the AO's center (in a.u.).
     *
     * @throw none No throw guarantee.
     */
    constexpr const auto& center() const noexcept { return *m_center_; }

    /** @brief Returns a (possibly) read-/write-able view of primitive @p i
     *
     *  This function is used to create a view of primitive @p i. The resulting
     *  view will have write access to any non-const members.
     *
     *  @param[in] i Which primitive to return. @p i should be in the range
     *             [0, size()) and undefined behavior will result otherwise.
     *  @return A view of primitive @p i. The resulting instance is only valid
     *          as long as the present GaussianAO_ instance is in scope.
     *  @throw none No throw guarantee.
     */
    auto operator[](type::size i) noexcept;

    /** @brief Returns a read-only view of primitive @p i
     *
     *  This function is used to create a read-only view of the primitive @p i.
     *  The resulting view is read-only regardless of what the type of
     *  @p Traits.
     *
     *  @param[in] i Which primitive to return. @p i should be in the range
     *             [0, size()) and undefined behavior will result otherwise.
     *  @return A read-only view of primitive @p i. The resulting instance is
     *          only valid as long as the present GaussianAO_ instance is in
     *          scope .
     *  @throw none No throw guarantee.
     */
    constexpr auto operator[](type::size i) const noexcept;

    auto begin() noexcept { return detail_::make_bs_iterator(0, this); }
    auto end() noexcept { return detail_::make_bs_iterator(size(), this); }
    auto begin() const noexcept { return detail_::make_bs_iterator(0, this); }
    auto end() const noexcept {
        return detail_::make_bs_iterator(size(), this);
    }
    auto cbegin() const noexcept { return begin(); }
    auto cend() const noexcept { return end(); }

private:
    /// The instance holding the contraction coefficients
    detail_::holder<typename Traits::coef_container> m_cs_;
    /// The instance holding the exponents
    detail_::holder<typename Traits::exp_container> m_as_;
    /// The instance holding the center
    detail_::holder<typename Traits::center_type> m_center_;
}; // class GaussianAO_

/// A GaussianAO_ with a dynamic number of floating-point-based primitives
template<typename T>
using GaussianAO = GaussianAO_<detail_::GaussianAOTraits<T>>;

/// A GaussianAO_ with a static number of floating-point-based primitives
template<typename T, type::size N>
using StaticGaussianAO = GaussianAO_<detail_::StaticGaussianAOTraits<T, N>>;

/** @brief Factory function for creating a GaussianAO_ with the provided state.
 *
 *  @relates GaussianAO_
 *
 *  This function uses template meta-programming to work out the proper type of
 *  the Traits parameter for the returned GaussianAO_ instance. Specifically, if
 *  any of the parameters are pointers they will be aliased, and if they are
 *  const they will be read-only.
 *
 * @tparam CoefArray The type of the container holding the coefficients.
 * @tparam ExpArray  The type of the container holding the exponents.
 * @tparam CenterType The type of the center.
 * @param[in] cs The contraction coefficients for the primitives
 * @param[in] as The exponents for the primitives.
 * @param[in] r0 The Cartesian coordinate where the primitives are centered.
 * @return A GaussianAO_ instance consistent with the input.
 *
 * @throw none No throw guarantee.
 */
template<typename CoefArray, typename ExpArray, typename CenterType>
constexpr auto make_gaussian_ao(CoefArray&& cs, ExpArray&& as,
                                CenterType&& r0) noexcept;

/** @brief Compares two GaussianAO_ instances for equality.
 *
 *  Two GaussianAO_ instances are equal if their centers compare equal, they
 *  have the same number of primitives, and each of the primitives compares
 *  equal
 *
 * @tparam Traits1 The type traits associated with @p lhs
 * @tparam Traits2 The type traits associated with @p rhs
 * @param[in] lhs The instance on the left side of the equality operator
 * @param[in] rhs The instance on the right side of the equality operator
 * @return True if the instances are equal and false otherwise.
 * @throw none No throw guarantee.
 */
template<typename Traits1, typename Traits2>
constexpr bool operator==(const GaussianAO_<Traits1>& lhs,
                          const GaussianAO_<Traits2>& rhs) noexcept;

/** @brief Compares two GaussianAO_ instances for inequality.
 *
 *  Two GaussianAO_ instances are equal if their centers compare equal, they
 *  have the same number of primitives, and each of the primitives compares
 *  equal.
 *
 * @tparam Traits1 The type traits associated with @p lhs
 * @tparam Traits2 The type traits associated with @p rhs
 * @param[in] lhs The instance on the left side of the inequality operator
 * @param[in] rhs The instance on the right side of the inequality operator
 * @return True if the instances are not equal and false otherwise.
 * @throw none No throw guarantee.
 */
template<typename Traits1, typename Traits2>
constexpr bool operator!=(const GaussianAO_<Traits1>& lhs,
                          const GaussianAO_<Traits2>& rhs) noexcept;

//--------------------------------Implementations-------------------------------
// Makes the class's type more readable
#define AO_CLASS_TYPE GaussianAO_<Traits>

template<typename Traits>
template<typename CoefType, typename ExpType, typename CenterType>
constexpr AO_CLASS_TYPE::GaussianAO_(CoefType&& c, ExpType&& a,
                                     CenterType&& r0) noexcept :
  m_cs_(std::forward<CoefType>(c)),
  m_as_(std::forward<ExpType>(a)),
  m_center_(std::forward<CenterType>(r0)) {}

template<typename Traits>
auto AO_CLASS_TYPE::operator[](type::size i) noexcept {
    const auto* ptr = &center(); // Make sure it's const pointer
    return make_gaussian(&((*m_cs_)[i]), &((*m_as_)[i]), ptr);
}

template<typename Traits>
constexpr auto AO_CLASS_TYPE::operator[](type::size i) const noexcept {
    return make_gaussian(&(*m_cs_)[i], &(*m_as_)[i], &center());
}

template<typename CoefCont, typename ExpCont, typename CenterType>
constexpr auto make_gaussian_ao(CoefCont&& cs, ExpCont&& as,
                                CenterType&& r0) noexcept {
    using clean_coefs  = std::remove_reference_t<CoefCont>;
    using clean_exps   = std::remove_reference_t<ExpCont>;
    using clean_center = std::remove_reference_t<CenterType>;
    using traits =
      detail_::GaussianAOTraits_<clean_coefs, clean_exps, clean_center>;
    return GaussianAO_<traits>(std::forward<CoefCont>(cs),
                               std::forward<ExpCont>(as),
                               std::forward<CenterType>(r0));
}

// template<typename PrimType, typename...Args>
// constexpr auto make_gaussian_ao(PrimType&& p0, Args&&...ps) noexcept {
//    std::array cs{p0.coef(), ps.coef()...};
//    std::array as{p0.exp(), ps.exp()...};
//    auto r0 = p0.center();
//    return make_gaussian_ao(cs, as, r0);
//}

template<typename Traits1, typename Traits2>
constexpr bool operator==(const GaussianAO_<Traits1>& lhs,
                          const GaussianAO_<Traits2>& rhs) noexcept {
    if(lhs.size() != rhs.size()) return false;
    // Primitives all have the center stored in this instance
    for(std::size_t i = 0; i < lhs.size(); ++i)
        if(lhs[i] != rhs[i]) return false;
    return true;
}

template<typename Traits1, typename Traits2>
constexpr bool operator!=(const GaussianAO_<Traits1>& lhs,
                          const GaussianAO_<Traits2>& rhs) noexcept {
    return !(lhs == rhs);
}

#undef AO_CLASS_TYPE
} // namespace libchemist::basis_set
