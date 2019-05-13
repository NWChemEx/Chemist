#pragma once
#include "libchemist/basis_set/detail_/gaussian_pimpl.hpp"
#include <tuple>

namespace libchemist::basis_set {

/** @brief The common API for all implementations of a Gaussian primitive
 *
 *  In its simplest form this class is used like `Gaussian<double>{coef, exp}`.
 *  This produces a Gaussian primitive with the specified coefficient and
 *  exponent. Slightly more complicated is `GaussianAlias<double>{&coef, &exp}`,
 *  which provides the same API, but aliases existing values (likely in a
 *  contiguous array somewhere). Additional storage options can be added by
 *  implementing other PIMPLs. For example, in the most general scenario, one
 *  could have the PIMPL actually store the coordinates, pure-ness, exponent,
 *  coefficient, magnitude of the angular momentum, and projection of the
 *  angular momentum. Consequentially it would be possible to modify the
 *  aforementioned values on a per primitive basis instead of on a per shell
 *  basis as is more commonly done.
 *
 *  @note Despite how nasty this class looks it's a compile-time literal meaning
 *        if you hard code the values for the parameters, like is done for most,
 *        common basis sets, there's no runtime overhead for using this class.
 *
 * @tparam T The type used to store the coefficients and exponents. Should be
 *         an unqualified type or a const-qualified type.
 * @tparam PIMPLType The backend actually implementing this class. Despite
 *         being portrayed as a PIMPL, it's used more in a CRTP mechanism.
 */
template<typename T, typename PIMPLType = detail_::GaussianPIMPL<T>>
class Gaussian {
public:
    /// The type used to store the exponent and coefficient
    using value_type = typename PIMPLType::value_type;
    /// Type of a (possibly) read/write reference to a parameter
    using reference = value_type&;
    /// Type of a read-only reference to a parameter
    using const_reference = const value_type&;

    /** @brief
     *
     * @param c
     * @param a
     */
    template<typename U, typename V>
    constexpr Gaussian(U&& c, V&& a);

    /** @brief Provides read/write access to the
     *
     * @return
     */
    constexpr reference coef() noexcept { return m_pimpl_.coef(); }
    constexpr const_reference coef() const noexcept { return m_pimpl_.coef(); }

    constexpr reference exp() noexcept { return m_pimpl_.exp(); }
    constexpr const_reference exp() const noexcept { return m_pimpl_.exp(); }

    template<typename U, typename V>
    constexpr bool operator==(const Gaussian<U, V>& rhs) const noexcept;

    template<typename U, typename V>
    constexpr bool operator!=(const Gaussian<U, V>& rhs) const noexcept;

private:
    PIMPLType m_pimpl_;
};

template<typename T>
using GaussianAlias = Gaussian<T, detail_::GaussianAliasPIMPL<T>>;

//--------------------------------Implementations-------------------------------

template<typename T, typename PIMPLType>
template<typename U, typename V>
constexpr Gaussian<T, PIMPLType>::Gaussian(U&& c, V&& a) :
  m_pimpl_(std::forward<U>(c), std::forward<V>(a)) {}

template<typename T, typename PIMPLType>
template<typename U, typename V>
constexpr bool Gaussian<T, PIMPLType>::operator==(
  const Gaussian<U, V>& rhs) const noexcept {
    return std::tie(coef(), exp()) == std::tie(rhs.coef(), rhs.exp());
}

template<typename T, typename PIMPLType>
template<typename U, typename V>
constexpr bool Gaussian<T, PIMPLType>::operator!=(
  const Gaussian<U, V>& rhs) const noexcept {
    return !((*this) == rhs);
}

} // namespace libchemist::basis_set
