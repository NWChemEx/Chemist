#pragma once
#include "libchemist/basis_set/gaussian_alias.hpp"
#include <numeric>

namespace libchemist::basis_set {

/** @brief An atomic orbital (AO) created from a linear combination of
 *         Gaussian primitives.
 *
 *  Atomic orbitals (AOs), @f$\mu@f$ in electronic structure theory are
 *  typically linear combinations of primitives @f$g@f$. This class models that
 *  relation. Since it is common to work in terms of shells a common scenario is
 *  to not provide a center for the AO, or the primitives. This class supports
 *  this by wrapping the center in an std::optional.
 *
 * @note Instances of this class are capable of being used as compile-time
 *       literals.
 *
 * @tparam T The type of a primitive's exponent or coefficient
 * @tparam ContainerType The type of the container that should be used to hold
 *                       the primitive's exponents and coefficients. Defaults to
 *                       std::vector<T>.
 */
template<typename ContainerType>
class GaussianAO {
public:
    /// The floating-point type used for the parameters
    using value_type = typename ContainerType::value_type;

    /** @brief Creates an AO with no center and no primitives.
     *
     *  This ctor creates an AO that is mathematically equal to the zero
     *  function. The resulting AO is not centered anywhere in space nor does it
     *  have any primitives. Primitives can be added by calling `add_primitive`
     *  and a center can be added by calling `set_center`.
     *
     *  @throw none No throw guarantee.
     */
    constexpr GaussianAO() = default;

    /** @brief Constructs an AO given a list of primitives
     *
     * @tparam PrimType The type of the first primitive. Must satisfy the
     *                  concept of Gaussian (have a coefficient, exponent,
     *                  has_center, and center method).
     * @tparam Args     The types of the remaining primitives.
     * @param[in] prim1 The first primitive to add to the AO
     * @param[in] prims The remaining primitives to add to the AO
     * @throw std::logic_error if some primitives have a center and others
     *                         don't or if the centers are not the same. Strong
     *                         throw guarantee.
     */
    template<typename PrimType, typename... Args,
             detail_::enable_if_gaussian_t<std::decay_t<PrimType>> = 0>
    explicit constexpr GaussianAO(PrimType&& prim1, Args&&... prims);

    /** @brief Creates an AO given the primitive's parameters, but no center.
     *
     *
     *  @tparam CoefsType The type of the container holding the coefficients.
     *                        Must be implicitly convertable to ContainerType.
     *  @tparam ExpsType The type of the container holding the exponents.
     *                   Must be implicitly convertable to ContainerType.
     *
     *  @param[in] cs A container of the contraction coefficients for the
     *                primitives forming this AO. Should be the same length as
     *                @p as and the coefficients must have the same type as the
     *                exponents in @p as.
     *  @param[in] as A container of the exponents for the primitives forming
     *                this AO. Should be the same length as @p cs and the
     *                exponents must have the same type as the
     *                coefficients in @p cs.
     *
     * @throw none No throw guarantee.
     */
    template<typename CoefsType, typename ExpsType,
             detail_::disable_if_gaussian_t<std::decay_t<CoefsType>> = 0>
    constexpr GaussianAO(CoefsType&& cs, ExpsType&& as) noexcept;

    /** @brief Creates an AO given the primitive's parameters.
     *
     *
     *  @tparam CoefsType The type of the container holding the coefficients.
     *                    Must be implicitly convertable to ContainerType.
     *  @tparam ExpsType The type of the container holding the exponents.
     *                   Must be implicitly convertable to ContainerType.
     *  @tparam CoordType The type of center's coordinate. Must be implicitly
     *                   convertible to std::optional<type::coord<T>> (or just
     *                   type::coord<T>).
     *
     *  @param[in] cs A container of the contraction coefficients for the
     *                primitives forming this AO. Should be the same length as
     *                @p as and the coefficients must have the same type as the
     *                exponents in @p as.
     *  @param[in] as A container of the exponents for the primitives forming
     *                this AO. Should be the same length as @p cs and the
     *                exponents must have the same type as the
     *                coefficients in @p cs.
     * @param[in] r   Where the AO is centered in Cartesian space. Components
     *                should be in atomic units.
     * @throw none No throw guarantee.
     */
    template<typename CoefsType, typename ExpsType, typename CoordType,
             detail_::disable_if_gaussian_t<std::decay_t<CoefsType>> = 0>
    constexpr GaussianAO(CoefsType&& cs, ExpsType&& as, CoordType&& r) noexcept;

    /** @brief Returns the @p i-th primitive contributing to this AO.
     *
     *  The returned primitive is an alias of the primitive inside this class.
     *  It is valid as long as this GaussianAO instance is in scope although
     *  references returned from it are only valid as long as `add_primitive`
     *  is not called on this GaussianAO instance. The resulting primtive may
     *  have its coefficient and exponent modified, but not its center. The
     *  latter restriction follows from it necessarilly being centered on this
     *  AO.
     *
     * @param[in] i Which primitive to return.
     * @return A Gaussian primitive that aliases the primitive stored
     *         internally.
     *
     * @throws std::out_of_range If @p i is not in the range [0,nprims()).
     *                           Strong throw guarantee.
     */
    constexpr auto primitive(type::size i);

    /** @brief Returns the @p i-th primitive contributing to this AO.
     *
     *  The returned primitive is an alias of the primitive inside this class.
     *  It is valid as long as this GaussianAO instance is in scope although
     *  references returned from it are only valid as long as `add_primitive`
     *  is not called on this GaussianAO instance. The resulting primtive is
     *  read-only.
     *
     * @param[in] i Which primitive to return.
     * @return A Gaussian primitive that aliases the primitive stored
     *         internally. The primitive is read-only.
     *
     * @throws std::out_of_range If @p i is not in the range [0,nprims()).
     *                           Strong throw guarantee.
     */
    constexpr auto primitive(type::size i) const;

    /** @brief Returns the number of primitives in the AO.
     *
     *  @return The number of primitives in the AO.
     *
     *  @throw none No throw guarantee.
     */
    constexpr auto nprims() const noexcept { return m_coefs_.size(); }

    /** @brief Whether or not each primitive has a center.
     *
     *  Typically when hard-coding AO basis sets it is common to only specify
     *  the contraction coefficients and the exponents for the primitives. This
     *  function will tell you if that's the case or if the Cartesian
     *  coordinates of the center have been set.
     *
     *  @return True if the point where all primitives are centered has been
     *          set and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    constexpr bool has_center() const noexcept { return m_coord_.has_value(); }

    /** @brief Allows one to set the Cartesian coordinates of the AO's center.
     *
     *  It is necessary to set the center's coordinates using this function if
     *  `has_center` is false. If `has_center` is true using this function to
     *  set the coordinates is equivalent to setting it through the `center`
     *  function.
     *
     * @tparam CoordType The type of the input coordinate. Must be implicitly
     *                   convertable to the type::coord<T>.
     *
     * @param[in] r Where in Cartesian space this AO is to be centered.
     *              Componets should be in atomic units.
     *
     * @throw none No throw gurantee.
     */
    template<typename CoordType>
    void set_center(CoordType&& r) noexcept;

    /** @brief Returns the Cartesian coordinates where this AO is centered, in
     *         a read/write state.
     *
     * This function can be used to retrieve the Cartesian coordinates of the
     * AO assuming they have been set either in the ctor or by calling
     * `set_center`. An error will occur if this is not the case.
     *
     * @return The Cartesian coordinates, in atomic units, where this AO is
     *         centered.
     * @throw std::bad_optional_access if the coordinates have not been set.
     *                                 Strong throw guarantee.
     */
    constexpr auto& center() { return m_coord_.value(); }

    /** @brief Returns the Cartesian coordinates where this AO is centered, in
     *         a read-only state
     *
     * This function can be used to retrieve the Cartesian coordinates of the
     * AO assuming they have been set either in the ctor or by calling
     * `set_center`. An error will occur if this is not the case.
     *
     * @return The Cartesian coordinates, in atomic units, where this AO is
     *         centered.
     * @throw std::bad_optional_access if the coordinates have not been set.
     *                                 Strong throw guarantee.
     */
    constexpr auto& center() const { return m_coord_.value(); }

    /** @brief Adds the primitive to the AO.
     *
     *  This function can be used to add an additional primitive to the AO. This
     *  assumes that @p ContainerType has a `push_back` method. If it does not
     *  you will get a compile-time error.
     *
     * @tparam GaussianType Type of the input primitive. Must define a
     *                      `coefficient` and `exponent` function which return
     *                      their namesakes.
     *
     * @param[in] g The primitive to add to the AO.
     *
     * @throw ??? Throws if the underlying containers' `push_back` method
     *            throws. Weak throw guarantee.
     *
     * @note This function may invalidate references obtained from primitives
     *       returned from the `primitive` function. The returned primitives
     *       remain valid.
     *
     */
    template<typename GaussianType>
    void add_primitive(GaussianType&& g);

    /** @brief Creates a primitive with the provided characteristics and adds it
     *         to the AO.
     *
     *  This function can be used to add an additional primitive to the AO. This
     *  overload can be used to bypass creating a primitive instance. Like the
     *  other `add_primitive` overload this function assumes that
     *  @p ContainerType has a `push_back` method. If it does not you will get a
     *  compile-time error.
     *
     * @param[in] c The contraction coefficient for the primitive.
     *
     * @param[in] a The exponent for the primitive.
     *
     * @throw ??? Throws if the underlying containers' `push_back` method
     *            throws. Weak throw guarantee.
     *
     * @note This function may invalidate references obtained from primitives
     *       returned from the `primitive` function. The returned primitives
     *       remain valid.
     */
    void add_primitive(value_type c, value_type a);

private:
    // Ensure views can get the data
    friend class detail_::CallMemberX;

    /// Retrieves primitive @p i's coefficient in a modifiable state
    constexpr auto& coefficient(type::size i) noexcept { return m_coefs_[i]; }

    /// Retrieves primitive @p i's coefficient in a read-only state
    constexpr auto& coefficient(type::size i) const noexcept;

    /// Retrieves primitive @p i's exponent in a modifiable stte
    constexpr auto& exponent(type::size i) noexcept { return m_alpha_[i]; }

    /// Retrieves primitive @p i's exponent in a read-only state
    constexpr auto& exponent(type::size i) const noexcept;

    /// Determines if primitive @p i has a center
    constexpr bool has_center(type::size) const { return has_center(); }

    /// Returns primitive @p i's center, throwing if it's not set
    constexpr auto& center(type::size) const { return m_coord_.value(); }

    /// The contraction coefficients
    ContainerType m_coefs_;

    /// The primitives' exponents
    ContainerType m_alpha_;

    ///(possibly) the Cartesian coordinates of the Gaussian, in atomic units
    std::optional<type::coord<value_type>> m_coord_;
};

/** @brief Determines if a GaussianAO is the same as another object.
 *  @relates GaussianAO
 *
 *  Two AOs are equal if they have the same number of primitives and the
 *  i-th primitive of each instance is identical. Hence order matters.
 *
 * @tparam LHST The type of the parameters stored in @p lhs
 * @tparam LHSCont The type of the container storing the @p lhs's state.
 * @tparam RHS The type of the object on the right of the equality sign. Must
 *             define a `nprims` function and a `primitive(type::size)`
 *             function.
 *
 * @param[in] lhs The AO on the left of the equality operator.
 * @param[in] rhs The AO on the right of the equality operator.
 * @return True if @p lhs and @p rhs are equal and false otherwise.
 */
template<typename LHS, typename RHS>
constexpr bool operator==(const GaussianAO<LHS>& lhs, const RHS& rhs) noexcept;

/** @brief Determines if a GaussianAO is different from another object.
 *  @relates GaussianAO
 *
 *  Two AOs are equal if they have the same number of primitives and the
 *  i-th primitive of each instance is identical. Hence order matters.
 *
 * @tparam LHST The type of the parameters stored in @p lhs
 * @tparam LHSCont The type of the container storing the @p lhs's state.
 * @tparam RHS The type of the object on the right of the inequality sign. Must
 *             define a `nprims` function and a `primitive(type::size)`
 *             function.
 *
 * @param[in] lhs The AO on the left of the inequality operator.
 * @param[in] rhs The AO on the right of the inequality operator.
 * @return False if @p lhs and @p rhs are equal and true otherwise.
 */
template<typename LHS, typename RHS>
constexpr bool operator!=(const GaussianAO<LHS>& lhs, const RHS& rhs) noexcept;

//----------------------------------Implementations-----------------------------
#define CLASS_PARAM template<typename ContainerType>
#define CLASS_TYPE GaussianAO<ContainerType>

template<typename PrimType, typename... Prims>
GaussianAO(PrimType&&, Prims&&...)
  ->GaussianAO<std::enable_if_t<
    detail_::is_gaussian_v<std::decay_t<PrimType>>,
    detail_::ao_container_t<std::decay_t<PrimType>, std::decay_t<Prims>...>>>;

template<typename CoefsType, typename ExpsType>
GaussianAO(CoefsType&&, ExpsType&&,
           detail_::disable_if_gaussian_t<std::decay_t<CoefsType>> =
             0) noexcept->GaussianAO<std::common_type_t<CoefsType, ExpsType>>;

template<typename CoefsType, typename ExpsType, typename CoordType>
GaussianAO(CoefsType&&, ExpsType&&, CoordType&&,
           detail_::disable_if_gaussian_t<std::decay_t<CoefsType>> =
             0) noexcept->GaussianAO<std::common_type_t<CoefsType, ExpsType>>;

CLASS_PARAM
template<typename PrimType, typename... Args,
         detail_::enable_if_gaussian_t<std::decay_t<PrimType>>>
constexpr CLASS_TYPE::GaussianAO(PrimType&& prim0, Args&&... prims) :
  m_coefs_({prim0.coefficient(), prims.coefficient()...}),
  m_alpha_({prim0.exponent(), prims.exponent()...}),
  m_coord_(prim0.has_center() ? std::optional{prim0.center()} : std::nullopt) {
    std::array<bool, sizeof...(Args)> has_c{prims.has_center()...};
    for(auto x : has_c)
        if(x != has_center())
            throw std::logic_error("Primitives have different centers");
    if(!has_center()) return;
    std::array<type::coord<value_type>, sizeof...(Args)> rs{prims.center()...};
    const auto r0 = std::tie((*m_coord_)[0], (*m_coord_)[1], (*m_coord_)[2]);
    for(auto x : rs)
        if(std::tie(x[0], x[1], x[2]) != r0)
            throw std::logic_error("Primitives have different centers");
}

CLASS_PARAM
template<typename CoefsType, typename ExpsType,
         detail_::disable_if_gaussian_t<std::decay_t<CoefsType>>>
constexpr CLASS_TYPE::GaussianAO(CoefsType&& cs, ExpsType&& as) noexcept :
  GaussianAO(std::forward<CoefsType>(cs), std::forward<ExpsType>(as),
             std::nullopt) {}

CLASS_PARAM
template<typename CoefsType, typename ExpsType, typename CoordType,
         detail_::disable_if_gaussian_t<std::decay_t<CoefsType>>>
constexpr CLASS_TYPE::GaussianAO(CoefsType&& cs, ExpsType&& as,
                                 CoordType&& r) noexcept :
  m_coefs_(std::forward<CoefsType>(cs)),
  m_alpha_(std::forward<ExpsType>(as)),
  m_coord_(std::forward<CoordType>(r)) {}

CLASS_PARAM
constexpr auto CLASS_TYPE::primitive(type::size i) {
    if(i >= nprims())
        throw std::out_of_range("Requested primitive is out of range");
    return GaussianAlias{this, std::array{i}};
}

CLASS_PARAM
constexpr auto CLASS_TYPE::primitive(type::size i) const {
    if(i >= nprims())
        throw std::out_of_range("Requested primitive is out of range");
    return GaussianAlias{this, std::array{i}};
}

CLASS_PARAM
template<typename GaussianType>
void CLASS_TYPE::add_primitive(GaussianType&& g) {
    add_primitive(g.coefficient(), g.exponent());
}

CLASS_PARAM
void CLASS_TYPE::add_primitive(typename CLASS_TYPE::value_type c,
                               typename CLASS_TYPE::value_type a) {
    m_coefs_.push_back(c);
    m_alpha_.push_back(a);
}

CLASS_PARAM
template<typename CoordType>
void CLASS_TYPE::set_center(CoordType&& r) noexcept {
    m_coord_.emplace(std::forward<CoordType>(r));
}

CLASS_PARAM
constexpr auto& CLASS_TYPE::coefficient(type::size i) const noexcept {
    return m_coefs_[i];
}

CLASS_PARAM
constexpr auto& CLASS_TYPE::exponent(type::size i) const noexcept {
    return m_alpha_[i];
}

template<typename LHS, typename RHS>
constexpr bool operator==(const GaussianAO<LHS>& lhs, const RHS& rhs) noexcept {
    return detail_::compare_ao(lhs, rhs);
}

template<typename LHS, typename RHS>
constexpr bool operator!=(const GaussianAO<LHS>& lhs, const RHS& rhs) noexcept {
    return !(lhs == rhs);
}

#undef CLASS_TYPE
#undef CLASS_PARAM
} // namespace libchemist::basis_set
