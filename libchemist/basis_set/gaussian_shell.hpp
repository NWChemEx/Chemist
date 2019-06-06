#pragma once
#include "libchemist/basis_set/gaussian_ao_alias.hpp"

namespace libchemist::basis_set {

/** @brief A shell of GaussianAO instances.
 *
 *  A spherical (Cartesian) Gaussian shell has @f$2\ell +1@f$
 *  (@f$\ell + 2 \choose 2@f$) GaussianAO instances. All of these AOs are
 *  centered in the same location in Cartesian space, have the same total
 *  angular momentum @f$\ell@f$, and are formed from the same primitives.
 *
 * @tparam Container The type of the container used to store the primitives'
 *                   parameters. Must be random access.
 */
template<typename Container>
class GaussianShell {
public:
    /// The floating-point type used to store the parameters
    using value_type = typename Container::value_type;

    /** @brief Constructs a spherical s-shell with no center.
     *
     *  This ctor is mainly intended to be used to create a place-holder
     *  GaussianShell instance.
     *
     *  @throw none No throw guarantee.
     */
    constexpr GaussianShell() = default;

    /** @brief Builds a GaussianShell given the explicit AO to use.
     *
     * @tparam AOType The type of the AO. Must satisfy the concept of an AO.
     * @param[in] pure Is the shell spherical?
     * @param[in] l The magnitude of the total angular momentum of the shell in
     *              atomic units.
     * @param[in] ao The AO from which the parameters will be pulled.
     *
     * @throw ??? if the container throws during filling.
     *            Strong throw guarantee.
     */
    template<typename AOType>
    constexpr GaussianShell(bool pure, type::angular_momentum l, AOType&& ao);

    /** @brief Constructs a GaussianShell holding only the shell's parameters
     *
     *  A Gaussian shell has @f$2\ell +1@f$ Gaussian AOs
     *
     *  @tparam CoefType The type of the container holding the input contraction
     *                   coefficients. Must be implicitly convertable to
     *                   @p Container.
     *  @tparam ExpType  The type of the container holding the input exponents.
     *                   Must be implicitly convertible to @p Container.
     * @param[in] pure   Whether the shell is Spherical or not.
     * @param[in] l      The magnitude of the total angular momentum.
     * @param[in] cs     The contraction coefficients for the primitives.
     * @param as
     */
    template<typename CoefType, typename ExpType>
    constexpr GaussianShell(bool pure, type::angular_momentum l, CoefType&& cs,
                            ExpType&& as) noexcept;

    template<typename CoefType, typename ExpType, typename CoordType>
    constexpr GaussianShell(bool pure, type::angular_momentum l, CoefType&& cs,
                            ExpType&& as, CoordType&& r) noexcept;

    /** @brief Returns an alias to the @p i-th AO stored in the shell.
     *
     *  This class only explicitly stores a single AO. Consequentially, the
     *  AO returned by this function is read-only because it is not aliasing an
     *  existing AO.
     *
     * @param[in] i The AO to return an alias to. Should be in the range
     *              [0, nao()).
     * @return A read-only alias to the @p i-th AO.
     * @throw std::out_of_range if @p i is not in the range [0, nao()). Strong
     *                          throw guarantee.
     */
    constexpr auto ao(type::size i) const;

    /** @brief The total number of atomic orbitals in this shell.
     *
     *  @return @f$2\ell+1@f$ for spherical shells and @f$\ell+2 \choose 2@f$
     *          for Cartesian shells.
     *
     *  @throw none No throw guarantee.
     */
    constexpr auto naos() const noexcept;

    /** @brief Returns whether the shell is pure in a read/write
     *         state.
     *
     *  @return Whether the shell is pure or not in a read/write state
     *  @throw none No throw guarantee.
     */
    constexpr auto& pure() noexcept { return m_is_pure_; }

    /** @brief Returns whether the shell is pure in a read-only
     *         state.
     *
     *  @return Whether the shell is pure or not in a read-only state
     *  @throw none No throw guarantee.
     */
    constexpr auto& pure() const noexcept { return m_is_pure_; }

    /** @brief Returns the magnitude of the angular momentum in a read/write
     *         state.
     *
     *  @return The magnitude of the total angular momentum of the shell, in
     *          atomic units.
     *  @throw none No throw guarantee.
     */
    constexpr auto& l() noexcept { return m_l_; }

    /** @brief Returns the magnitude of the angular momentum in a read-only
     *         state.
     *
     *  @return The magnitude of the total angular momentum of the shell, in
     *          atomic units.
     *  @throw none No throw guarantee.
     */
    constexpr auto& l() const noexcept { return m_l_; }

    /** @brief Whether or not each AO has a center.
     *
     *  Typically when hard-coding AO basis set it is common to only specify
     *  the contraction coefficients, the exponents, the purity, and the angular
     *  momentum for the shells. This function will tell you if that's the case
     *  or if the Cartesian coordinates of the center have been set.
     *
     *  @return True if the point where all AOs are centered has been set and
     *          false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    constexpr bool has_center() const noexcept { return has_center(0ul); }

    /** @brief Allows one to set the Cartesian coordinates of the shell.
     *
     *  It is necessary to set the center's coordinates using this function if
     *  `has_center` is false. If `has_center` is true using this function to
     *  set the coordinates is equivalent to setting it through the `center`
     *  function.
     *
     * @tparam CoordType The type of the input coordinate. Must be implicitly
     *                   convertable to the type::coord<T>.
     *
     * @param[in] r Where in Cartesian space this shell is to be centered.
     *              Components should be in atomic units.
     *
     * @throw none No throw guarantee.
     */
    template<typename CoordType>
    void set_center(CoordType&& r) noexcept;

    /** @brief Returns the Cartesian coordinates where this shell is centered,
     *         in a read/write state.
     *
     * This function can be used to retrieve the Cartesian coordinates of the
     * shell assuming they have been set either in the ctor or by calling
     * `set_center`. An error will occur if this is not the case.
     *
     * @return The Cartesian coordinates, in atomic units, where this shell is
     *         centered.
     * @throw std::bad_optional_access if the coordinates have not been set.
     *                                 Strong throw guarantee.
     */
    constexpr auto& center() { return m_coord_.value(); }

    /** @brief Returns the Cartesian coordinates where this shell is centered,
     *         in a read-only state
     *
     * This function can be used to retrieve the Cartesian coordinates of the
     * shell assuming they have been set either in the ctor or by calling
     * `set_center`. An error will occur if this is not the case.
     *
     * @return The Cartesian coordinates, in atomic units, where this shell is
     *         centered.
     * @throw std::bad_optional_access if the coordinates have not been set.
     *                                 Strong throw guarantee.
     */
    constexpr auto& center() const { return m_coord_.value(); }

    /** @name Class Specific Accessors
     *
     *  Functions in this section are not part of the concept of shell, but can
     *  be used to modify the state of this instance.
     */
    ///@{

    /** @brief Provides read/write access to the @p i-th coefficient.
     *
     *  This class only stores the primitive coefficients for a single AO
     *  because it assumes that each AO uses the same parameters.
     *  Consequentially, modifying the coefficient through this API will
     *  change its value for all AOs. Since this function is not part of the
     *  concept of a shell, use of it requires the user to understand how this
     *  class stores data.
     *
     * @param[in] i The primitive's coefficient you want. Should be in the range
     *            [0, nprims()/naos()).
     * @return The requested coefficient in a read/write state.
     * @throw std::out_of_range if @p i is not in the range
     *                         [0, nprims()/naos()). Strong throw guarantee.
     */
    auto& coefficient(type::size i);

    auto& coefficient(type::size i) const;

    /** @brief Provides read/write access to the @p i-th exponent.
     *
     *  This class only stores the primitive exponents for a single AO
     *  because it assumes that each AO uses the same parameters.
     *  Consequentially, modifying the exponent through this API will
     *  change its value for all AOs. Since this function is not part of the
     *  concept of a shell, use of it requires the user to understand how this
     *  class stores data.
     *
     * @param[in] i The primitive's exponent you want. Should be in the range
     *            [0, nprims()/naos()).
     * @return The requested cexponent in a read/write state.
     * @throw std::out_of_range if @p i is not in the range
     *                          [0, nprims()/naos()). Strong throw guarantee.
     */
    auto& exponent(type::size i);

    auto& exponent(type::size i) const;
    ///@}
private:
    /// Allows views to get at data
    friend class detail_::CallMemberX;

    /// Binomial coefficient @p m choose @p n
    constexpr auto bc(type::size m, type::size n) const noexcept;

    /// The number of primitives in AO @p i
    constexpr auto nprims(type::size) const noexcept { return m_coefs_.size(); }

    /// The @p j-th primitive in AO @p i
    constexpr auto primitive(type::size i, type::size j) const noexcept;

    /// The @p j-th contraction coefficient in AO @p i
    constexpr auto& coefficient(type::size, type::size j) const noexcept;

    /// The @p j-th exponent in AO @p i
    constexpr auto& exponent(type::size, type::size j) const noexcept;

    /// Whether a particular primitive or AO has a center
    template<typename... Idxs>
    constexpr bool has_center(type::size, Idxs...) const noexcept;

    /// The center in a read-only state
    template<typename... Idxs>
    constexpr auto& center(type::size, Idxs...) const;

    /// Is this a spherical shell?
    bool m_is_pure_ = true;

    /// The magnitude of the angular momentum
    type::angular_momentum m_l_ = 0;

    /// The contraction coefficients for the primitives
    Container m_coefs_;

    /// The exponents for the primitives
    Container m_alpha_;

    ///(possibly) the Cartesian coordinates of the shell, in atomic units
    std::optional<type::coord<value_type>> m_coord_;
};

/** @brief Determines if a GaussianShell is equal to another object.
 *
 *  @relates GaussianShell
 *
 *  Two shells are equal if they are both spherical (or Cartesian), the
 *  magnitude of their total angular momentum are the same, and the underlying
 *  AOs are the same.
 *
 * @tparam LHS The type of the container in the @p lhs.
 * @tparam RHS The type of the shell on the right side of the equality operator.
 * @param[in] lhs The shell on the left of the operator.
 * @param[in] rhs The shell on the right of the operator.
 * @return True if the shells are equal and false otherwise.
 * @throw none No throw guarantee.
 */
template<typename LHS, typename RHS>
constexpr bool operator==(const GaussianShell<LHS>& lhs, RHS&& rhs) noexcept;

/** @brief Determines if a GaussianShell differs from another object.
 *
 *  @relates GaussianShell
 *
 *  Two shells are equal if they are both spherical (or Cartesian), the
 *  magnitude of their total angular momentum are the same, and the underlying
 *  AOs are the same.
 *
 * @tparam LHS The type of the container in the @p lhs.
 * @tparam RHS The type of the shell on the right side of the operator.
 * @param[in] lhs The shell on the left of the operator.
 * @param[in] rhs The shell on the right of the operator.
 * @return False if the shells are equal and true otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename LHS, typename RHS>
constexpr bool operator!=(const GaussianShell<LHS>& lhs, RHS&& rhs) noexcept;

//------------------------------Implementations---------------------------------

#define CLASS_PARAMS template<typename Container>
#define CLASS_TYPE GaussianShell<Container>

template<typename T>
GaussianShell(bool, type::angular_momentum, T&&) noexcept
  ->GaussianShell<detail_::gaussian_ao_container_t<std::decay_t<T>>>;

template<typename CoefType, typename ExpType>
GaussianShell(bool, type::angular_momentum, CoefType&&, ExpType&&) noexcept
  ->GaussianShell<std::common_type_t<CoefType, ExpType>>;

template<typename CoefType, typename ExpType, typename CoordType>
GaussianShell(
  bool, type::angular_momentum, CoefType&&, ExpType&&,
  CoordType&&) noexcept->GaussianShell<std::common_type_t<CoefType, ExpType>>;

CLASS_PARAMS
template<typename AOType>
constexpr CLASS_TYPE::GaussianShell(bool pure, type::angular_momentum l,
                                    AOType&& ao) :
  GaussianShell(pure, l, detail_::get_ao_coefs(ao), detail_::get_ao_exps(ao),
                ao.has_center() ? std::optional{ao.center()} : std::nullopt) {}

CLASS_PARAMS
template<typename CoefType, typename ExpType>
constexpr CLASS_TYPE::GaussianShell(bool pure, type::angular_momentum l,
                                    CoefType&& cs, ExpType&& as) noexcept :
  GaussianShell(pure, l, std::forward<CoefType>(cs), std::forward<ExpType>(as),
                std::nullopt) {}

CLASS_PARAMS
template<typename CoefType, typename ExpType, typename CoordType>
constexpr CLASS_TYPE::GaussianShell(bool pure, type::angular_momentum l,
                                    CoefType&& cs, ExpType&& as,
                                    CoordType&& r) noexcept :
  m_is_pure_(pure),
  m_l_(l),
  m_coefs_(std::forward<CoefType>(cs)),
  m_alpha_(std::forward<ExpType>(as)),
  m_coord_(std::forward<CoordType>(r)) {}

CLASS_PARAMS
constexpr auto CLASS_TYPE::ao(type::size i) const {
    if(i >= naos())
        throw std::out_of_range("Requested AO is outside range [0, nao())");
    return GaussianAOAlias{this, std::array{i}};
}

CLASS_PARAMS
constexpr auto CLASS_TYPE::naos() const noexcept {
    return m_is_pure_ ? 2 * m_l_ + 1 : bc(m_l_ + 2, 2);
}

CLASS_PARAMS
template<typename... Idxs>
constexpr bool CLASS_TYPE::has_center(type::size, Idxs...) const noexcept {
    return m_coord_.has_value();
}

CLASS_PARAMS
template<typename CoordValue>
void CLASS_TYPE::set_center(CoordValue&& r) noexcept {
    m_coord_.emplace(std::forward<CoordValue>(r));
}

CLASS_PARAMS
auto& CLASS_TYPE::coefficient(type::size i) {
    if(i >= m_coefs_.size())
        throw std::out_of_range("Requested primitive is out of range");
    return m_coefs_[i];
}

CLASS_PARAMS
auto& CLASS_TYPE::exponent(type::size i) {
    if(i >= m_alpha_.size())
        throw std::out_of_range("Requested primitive is out of range");
    return m_alpha_[i];
}

CLASS_PARAMS
auto& CLASS_TYPE::coefficient(type::size i) const {
    if(i >= m_coefs_.size())
        throw std::out_of_range("Requested primitive is out of range");
    return m_coefs_[i];
}

CLASS_PARAMS
auto& CLASS_TYPE::exponent(type::size i) const {
    if(i >= m_alpha_.size())
        throw std::out_of_range("Requested primitive is out of range");
    return m_alpha_[i];
}

CLASS_PARAMS
constexpr auto CLASS_TYPE::bc(type::size m, type::size n) const noexcept {
    if(n == 0 || n == m) return 1;
    return bc(m - 1, n - 1) + bc(m - 1, n);
}

CLASS_PARAMS
constexpr auto CLASS_TYPE::primitive(type::size i, type::size j) const
  noexcept {
    return GaussianAlias{this, std::array{i, j}};
}

CLASS_PARAMS
constexpr auto& CLASS_TYPE::coefficient(type::size, type::size j) const
  noexcept {
    return m_coefs_[j];
}

CLASS_PARAMS
constexpr auto& CLASS_TYPE::exponent(type::size, type::size j) const noexcept {
    return m_alpha_[j];
}

CLASS_PARAMS
template<typename... Idxs>
constexpr auto& CLASS_TYPE::center(type::size, Idxs...) const {
    return m_coord_.value();
}

template<typename LHS, typename RHS>
constexpr bool operator==(const GaussianShell<LHS>& lhs, RHS&& rhs) noexcept {
    return detail_::compare_shell(lhs, rhs);
}

template<typename LHS, typename RHS>
constexpr bool operator!=(const GaussianShell<LHS>& lhs, RHS&& rhs) noexcept {
    return !(lhs == rhs);
}

#undef CLASS_TYPE
#undef CLASS_PARAMS
} // namespace libchemist::basis_set
