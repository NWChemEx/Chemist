#pragma once
#include "libchemist/basis_set/gaussian_alias.hpp"

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
template<typename T, typename ContainerType = std::vector<T>>
class GaussianAO {
public:
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

    /** @brief Creates an AO given the primitive's parameters.
     *
     *  It is strongly suggested that you create a GaussianAO instance using
     *  the free-functions. These free functions provide a more user-friendly
     *  API for calling this ctor given a variety of inputs.
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
    template<typename CoefsType, typename ExpsType, typename CoordType>
    constexpr GaussianAO(CoefsType&& cs, ExpsType&& as, CoordType&& r) noexcept;

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
    void add_primitive(T c, T a);

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
    std::optional<type::coord<T>> m_coord_;
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
template<typename LHST, typename LHSCont, typename RHS>
constexpr bool operator==(const GaussianAO<LHST, LHSCont>& lhs,
                          const RHS& rhs) noexcept;

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
template<typename LHST, typename LHSCont, typename RHS>
constexpr bool operator!=(const GaussianAO<LHST, LHSCont>& lhs,
                          const RHS& rhs) noexcept;

/** @brief Creates an AO given a list of primitives
 *
 *  @note This function only participates in overload resolution if
 *        @p PrimType is a Gaussian or GaussianAlias instance.
 *
 * @tparam PrimType The type of the first primitive
 * @tparam Args The types of the remaining primitives
 * @param[in] prim The first primitive.
 * @param[in] args The remaining primitives
 * @return An AO with the provided primitives.
 */
template<typename PrimType, typename... Args,
         detail_::enable_if_gaussian_t<std::decay_t<PrimType>> = 0>
constexpr auto make_gaussian_ao(PrimType&& prim, Args&&... args);

/** @brief Creates an AO with no center, but the provided primtives
 *
 *  @note This function only participates in overload resolution if
 *        @p Coefype is not a Gaussian or GaussianAlias instance.
 *
 * @tparam CoefType The type of the container holding the coefficients.
 * @tparam ExpType The type of the container holding the exponents.
 *
 * @param[in] cs The coefficients for the primitives forming the AO.
 * @param[in] as The exponents for the primitives forming the AO. Must be the
 *               same number of exponents as coefficients.
 *
 * @return An AO with no center, but the provided primitives.
 * @throw none No throw guarantee.
 */
template<typename CoefType, typename ExpType,
         detail_::disable_if_gaussian_t<CoefType> = 0>
constexpr auto make_gaussian_ao(CoefType&& cs, ExpType&& as) noexcept;

/** @brief Creates an AO with a center and the provided primtives
 *
 *  @note This function only participates in overload resolution if
 *        @p Coefype is not a Gaussian or GaussianAlias instance.
 *
 * @tparam CoefType The type of the container holding the coefficients.
 * @tparam ExpType The type of the container holding the exponents.
 * @tparam CoordType The type of the center's coordinates.
 *
 * @param[in] cs The coefficients for the primitives forming the AO.
 * @param[in] as The exponents for the primitives forming the AO. Must be the
 *               same number of exponents as coefficients.
 * @param[in] r  The Cartesian coordinates of the center, in atomic units.
 *
 * @return An AO with the provided center and primitives.
 * @throw none No throw guarantee.
 */
template<typename CoefType, typename ExpType, typename CoordType,
         detail_::disable_if_gaussian_t<CoefType> = 0>
constexpr auto make_gaussian_ao(CoefType&& cs, ExpType&& as,
                                CoordType&& r) noexcept;

//----------------------------------Implementations-----------------------------
#define CLASS_TYPE GaussianAO<T, ContainerType>

template<typename T, typename ContainerType>
template<typename CoefsType, typename ExpsType, typename CoordType>
constexpr CLASS_TYPE::GaussianAO(CoefsType&& cs, ExpsType&& as,
                                 CoordType&& r) noexcept :
  m_coefs_(std::forward<CoefsType>(cs)),
  m_alpha_(std::forward<ExpsType>(as)),
  m_coord_(std::forward<CoordType>(r)) {}

template<typename T, typename ContainerType>
constexpr auto CLASS_TYPE::primitive(type::size i) {
    if(i >= nprims())
        throw std::out_of_range("Requested primitive is out of range");
    return GaussianAlias{this, std::array{i}};
}

template<typename T, typename ContainerType>
constexpr auto CLASS_TYPE::primitive(type::size i) const {
    if(i >= nprims())
        throw std::out_of_range("Requested primitive is out of range");
    return GaussianAlias{this, std::array{i}};
}

template<typename T, typename ContainerType>
template<typename GaussianType>
void CLASS_TYPE::add_primitive(GaussianType&& g) {
    add_primitive(g.coefficient(), g.exponent());
}

template<typename T, typename ContainerType>
void CLASS_TYPE::add_primitive(T c, T a) {
    m_coefs_.push_back(c);
    m_alpha_.push_back(a);
}

template<typename T, typename ContainerType>
template<typename CoordType>
void CLASS_TYPE::set_center(CoordType&& r) noexcept {
    m_coord_.emplace(std::forward<CoordType>(r));
}

template<typename T, typename ContainerType>
constexpr auto& CLASS_TYPE::coefficient(type::size i) const noexcept {
    return m_coefs_[i];
}

template<typename T, typename ContainerType>
constexpr auto& CLASS_TYPE::exponent(type::size i) const noexcept {
    return m_alpha_[i];
}

template<typename LHST, typename LHSCont, typename RHS>
constexpr bool operator==(const GaussianAO<LHST, LHSCont>& lhs,
                          const RHS& rhs) noexcept {
    return detail_::compare_ao(lhs, rhs);
}

template<typename LHST, typename LHSCont, typename RHS>
constexpr bool operator!=(const GaussianAO<LHST, LHSCont>& lhs,
                          const RHS& rhs) noexcept {
    return !(lhs == rhs);
}

template<typename PrimType, typename... Args,
         detail_::enable_if_gaussian_t<std::decay_t<PrimType>>>
constexpr auto make_gaussian_ao(PrimType&& prim, Args&&... args) {
    std::array cs{prim.coefficient(), args.coefficient()...};
    std::array as{prim.exponent(), args.exponent()...};
    return make_gaussian_ao(
      std::move(cs), std::move(as),
      prim.has_center() ? std::make_optional(prim.center()) : std::nullopt);
}

template<typename CoefType, typename ExpType,
         detail_::disable_if_gaussian_t<CoefType>>
constexpr auto make_gaussian_ao(CoefType&& c, ExpType&& a) noexcept {
    return make_gaussian_ao(std::forward<CoefType>(c), std::forward<ExpType>(a),
                            std::nullopt);
}

template<typename CoefType, typename ExpType, typename CoordType,
         detail_::disable_if_gaussian_t<CoefType>>
constexpr auto make_gaussian_ao(CoefType&& cs, ExpType&& as,
                                CoordType&& r) noexcept {
    using clean_coef = std::decay_t<CoefType>;
    using clean_exp  = std::decay_t<ExpType>;
    using value_type = typename clean_coef::value_type;
    static_assert(std::is_same_v<clean_coef, clean_exp>,
                  "Containers must be same type");

    return GaussianAO<value_type, clean_coef>(std::forward<CoefType>(cs),
                                              std::forward<ExpType>(as),
                                              std::forward<CoordType>(r));
}

#undef CLASS_TYPE
} // namespace libchemist::basis_set
