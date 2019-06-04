#pragma once
#include "libchemist/basis_set/gaussian_ao.hpp"

namespace libchemist::basis_set {

/** @brief An alias of a GaussianAO
 */
template<typename BasisSet, typename IndexType>
class GaussianAOAlias {
public:
    constexpr GaussianAOAlias(BasisSet bs, IndexType idx) noexcept;

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
    constexpr auto nprims() const noexcept;

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
    constexpr bool has_center() const noexcept;

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
    constexpr auto& center() const;

private:
    detail_::holder<BasisSet> m_bs_;
    IndexType m_index_;
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
constexpr bool operator==(const GaussianAOAlias<LHST, LHSCont>& lhs,
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
constexpr bool operator!=(const GaussianAOAlias<LHST, LHSCont>& lhs,
                          const RHS& rhs) noexcept;

//----------------------------------Implementations-----------------------------
#define CLASS_TYPE GaussianAOAlias<BasisSet, IndexType>

template<typename BasisSet, typename IndexType>
constexpr CLASS_TYPE::GaussianAOAlias(BasisSet bs, IndexType idx) noexcept :
  m_bs_(bs),
  m_index_(idx) {}

template<typename BasisSet, typename IndexType>
constexpr auto CLASS_TYPE::primitive(type::size i) {
    return detail_::CallMemberX::primitive(m_bs_,
                                           detail_::cat_indices(m_index_, i));
}

template<typename BasisSet, typename IndexType>
constexpr auto CLASS_TYPE::primitive(type::size i) const {
    return detail_::CallMemberX::primitive(m_bs_,
                                           detail_::cat_indices(m_index_, i));
}

template<typename BasisSet, typename IndexType>
constexpr auto CLASS_TYPE::nprims() const noexcept {
    return detail_::CallMemberX::nprims(m_bs_, m_index_);
}

template<typename BasisSet, typename IndexType>
constexpr bool CLASS_TYPE::has_center() const noexcept {
    return detail_::CallMemberX::has_center(m_bs_, m_index_);
}

template<typename BasisSet, typename IndexType>
constexpr auto& CLASS_TYPE::center() const {
    return detail_::CallMemberX::center(m_bs_, m_index_);
}

template<typename LHST, typename LHSCont, typename RHS>
constexpr bool operator==(const GaussianAOAlias<LHST, LHSCont>& lhs,
                          const RHS& rhs) noexcept {
    return detail_::compare_ao(lhs, rhs);
}

template<typename LHST, typename LHSCont, typename RHS>
constexpr bool operator!=(const GaussianAOAlias<LHST, LHSCont>& lhs,
                          const RHS& rhs) noexcept {
    return !(lhs == rhs);
}

#undef CLASS_TYPE
} // namespace libchemist::basis_set
