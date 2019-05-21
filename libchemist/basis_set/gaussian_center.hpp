#include "libchemist/basis_set/detail_/gaussian_center_traits.hpp"
#include "libchemist/basis_set/detail_/holder.hpp"
#include "libchemist/basis_set/gaussian_shell.hpp"
#include <utilities/iter_tools/range_container.hpp>

namespace libchemist::basis_set {

/** @brief A collection of GaussianShell_ instances on the same center.
 *
 *  It is often advantageous to treat Gaussian shells that are on the same
 *  center together. This class factors out the common center.
 *
 *  @tparam Traits The type of the GaussianCenterTraits_ instance defining the
 *          member types.
 */
template<typename Traits>
class GaussianCenter_ {
public:
    /** @brief Makes a GaussianCenter_ from an array of shells
     *
     *
     *  @tparam CenterType The type of the Cartesian coefficient
     *  @tparam AMCont Type of a container holding the angular momentum of each
     *                 shell
     *  @tparam PureCont Type of a container holding the purity of each shell
     *  @tparam NPrimCont Type of a container holding the number of primitives
     *                    on each center
     *  @tparam CoefCont Type of a container holding all the coefficients for
     *                   the shells.
     *  @tparam ExpCont Type of a container holding all the exponents for the
     *                  shells.
     *  @param[in] r0 The Cartesian coordinates for this center.
     *  @param[in] am A container of the angular momenta of each shell
     *  @param[in] is_pure A container of whether each shell is pure or not
     *  @param[in] nprims The number of primitives associated with each shell
     *  @param[in] cs The contraction coefficients for each primitive
     *  @param[in] as The exponents for each primitive
     * @throw none No throw guarantee.
     */
    template<typename CenterType, typename AMCont, typename PureCont,
             typename NPrimCont, typename CoefCont, typename ExpCont>
    constexpr GaussianCenter_(CenterType&& r0, AMCont&& am, PureCont&& is_pure,
                              NPrimCont&& nprims, CoefCont&& cs,
                              ExpCont&& as) noexcept;

    /** @brief The number of shells centered on this center.
     *
     *  @return the number of shells centered on this center.
     *  @throw none No throw guarantee.
     */
    constexpr type::size size() const noexcept { return m_ls_->size(); }

    /** @brief The Cartesian coordinates of this center in a (possibly)
     *         read-/write-able state.
     *
     *
     * @return The Cartesian coordinates (in a.u.) of the center
     * @throw none No throw guarantee.
     */
    constexpr auto& center() noexcept { return *m_center_; }

    /** @brief The Cartesian coordinates of this center in a read-only state.
     *
     *
     * @return The Cartesian coordinates (in a.u.) of the center
     * @throw none No throw guarantee.
     */
    constexpr const auto& center() const noexcept { return *m_center_; }

    /** @brief Returns the requested shell in a (possibly) read-/write-able
     *         state.
     *
     * @param[in] i The shell to return. @p i should be in the range [0, size())
     * @return The requested shell.
     * @throw ??? If the container throws. Strong throw guarantee.
     */
    constexpr auto operator[](type::size i);

    /** @brief Returns the requested shell in a read-only state.
     *
     * @param[in] i The shell to return. @p i should be in the range [0, size())
     * @return The requested shell.
     * @throw ??? If the container throws. Strong throw guarantee.
     */
    constexpr const auto operator[](type::size i) const;

    auto begin() noexcept { return detail_::make_bs_iterator(0, this); }
    auto end() noexcept { return detail_::make_bs_iterator(size(), this); }
    auto begin() const noexcept { return detail_::make_bs_iterator(0, this); }
    auto end() const noexcept {
        return detail_::make_bs_iterator(size(), this);
    }
    auto cbegin() const noexcept { return begin(); }
    auto cend() const noexcept { return end(); }

private:
    /// The angular momentum of each shell
    detail_::holder<typename Traits::am_container> m_ls_;
    /// The purity of each shell
    detail_::holder<typename Traits::pure_container> m_pure_;
    /// element i/i+1 is the offset in cs_/as_ to shell i's first/last prim
    detail_::holder<typename Traits::nprim_container> m_offs_;
    detail_::holder<typename Traits::coef_container> m_cs_;
    detail_::holder<typename Traits::exp_container> m_as_;
    /// The Cartesian coordinate of the center
    detail_::holder<typename Traits::center_type> m_center_;
};

/** @brief Helper function for making a GaussianCenter_ instance
 *
 * @tparam CenterType The type of the incoming center.
 * @tparam AMCont Type of a container holding the angular momentum of each shell
 * @tparam PureCont Type of a container holding the purity of each shell
 * @tparam NPrimCont Type of a container holding the number of primitives on
 *                   each center
 * @tparam CoefCont Type of a container holding all the coefficients for the
 *                  shells.
 * @tparam ExpCont Type of a container holding all the exponents for the shells.
 * @param[in] r0 The Cartesian coordinates for this center.
 * @param[in] am A container of the angular momenta of each shell
 * @param[in] is_pure A container of whether each shell is pure or not
 * @param[in] nprims The number of primitives associated with each shell
 * @param[in] cs The contraction coefficients for each primitive
 * @param[in] as The exponents for each primitive
 * @return A GaussianCenter_ with the provided parameters
 */
template<typename CenterType, typename AMCont, typename PureCont,
         typename NPrimCont, typename CoefCont, typename ExpCont>
constexpr auto make_gaussian_center(CenterType&& r0, AMCont&& am,
                                    PureCont&& is_pure, NPrimCont&& nprims,
                                    CoefCont&& cs, ExpCont&& as) noexcept;

/** @brief Compares two GaussianCenter instances for equality
 *
 *  Two GaussianCenter_ instances are equal if they have the same center, and
 *  the same shells.
 *
 *  @tparam Traits1 The types of the members in @p lhs
 *  @tparam Traits2 The types of the members in @p rhs
 *  @param lhs The instance on the left of the equality operator
 *  @param rhs The instance on the right of the equality operator
 *  @return True if the two instances are equal and false otherwise.
 *  @throw none No throw guarantee.
 */
template<typename Traits1, typename Traits2>
constexpr bool operator==(const GaussianCenter_<Traits1>& lhs,
                          const GaussianCenter_<Traits2>& rhs) noexcept;

/** @brief Compares two GaussianCenter instances for inequality
 *
 *  Two GaussianCenter_ instances are equal if they have the same center, and
 *  the same shells.
 *
 *  @tparam Traits1 The types of the members in @p lhs
 *  @tparam Traits2 The types of the members in @p rhs
 *  @param lhs The instance on the left of the inequality operator
 *  @param rhs The instance on the right of the inequality operator
 *  @return True if the two instances are not equal and false otherwise.
 *  @throw none No throw guarantee.
 */
template<typename Traits1, typename Traits2>
constexpr bool operator!=(const GaussianCenter_<Traits1>& lhs,
                          const GaussianCenter_<Traits2>& rhs) noexcept;

//--------------------------------Implementations-------------------------------
/// makes the type of the center more readable
#define CENTER_TYPE GaussianCenter_<Traits>

template<typename Traits>
template<typename CenterType, typename AMCont, typename PureCont,
         typename NPrimCont, typename CoefCont, typename ExpCont>
constexpr CENTER_TYPE::GaussianCenter_(CenterType&& r0, AMCont&& am,
                                       PureCont&& is_pure, NPrimCont&& nprims,
                                       CoefCont&& cs, ExpCont&& as) noexcept :
  m_ls_(std::forward<AMCont>(am)),
  m_pure_(std::forward<PureCont>(is_pure)),
  m_offs_(std::forward<NPrimCont>(nprims)),
  m_cs_(std::forward<CoefCont>(cs)),
  m_as_(std::forward<ExpCont>(as)),
  m_center_(std::forward<CenterType>(r0)) {}

template<typename Traits>
constexpr auto CENTER_TYPE::operator[](type::size i) {
    const auto boff   = (*m_offs_)[i];
    const auto eoff   = (*m_offs_)[i + 1];
    const auto nprims = eoff - boff;
    using namespace utilities::detail_;
    const auto* r0 = &(*m_center_);
    return make_gaussian_shell(
      &(*m_ls_)[i],
      RangeContainer{m_cs_->cbegin() + boff, m_cs_->cbegin() + eoff, nprims},
      RangeContainer{m_as_->cbegin() + boff, m_as_->cbegin() + eoff, nprims},
      r0, (*m_pure_)[i]);
}

template<typename Traits>
constexpr const auto CENTER_TYPE::operator[](type::size i) const {
    const auto boff   = (*m_offs_)[i];
    const auto eoff   = (*m_offs_)[i + 1];
    const auto pure   = (*m_pure_)[i];
    const auto nprims = eoff - boff;
    using namespace utilities::detail_;
    return make_gaussian_shell(
      &(*m_ls_)[i],
      RangeContainer{m_cs_->begin() + boff, m_cs_->begin() + eoff, nprims},
      RangeContainer{m_as_->begin() + boff, m_as_->begin() + eoff, nprims},
      &(*m_center_), pure);
}

template<typename CenterType, typename AMCont, typename PureCont,
         typename NPrimCont, typename CoefCont, typename ExpCont>
constexpr auto make_gaussian_center(CenterType&& r0, AMCont&& am,
                                    PureCont&& is_pure, NPrimCont&& nprims,
                                    CoefCont&& cs, ExpCont&& as) noexcept {
    using clean_center = std::remove_reference_t<CenterType>;
    using clean_am     = std::remove_reference_t<AMCont>;
    using clean_pure   = std::remove_reference_t<PureCont>;
    using clean_nprim  = std::remove_reference_t<NPrimCont>;
    using clean_coef   = std::remove_reference_t<CoefCont>;
    using clean_exp    = std::remove_reference_t<ExpCont>;
    using traits =
      detail_::GaussianCenterTraits<clean_center, clean_am, clean_pure,
                                    clean_nprim, clean_coef, clean_exp>;
    return GaussianCenter_<traits>{
      std::forward<CenterType>(r0),    std::forward<AMCont>(am),
      std::forward<PureCont>(is_pure), std::forward<NPrimCont>(nprims),
      std::forward<CoefCont>(cs),      std::forward<ExpCont>(as)};
}

template<typename Traits1, typename Traits2>
constexpr bool operator==(const GaussianCenter_<Traits1>& lhs,
                          const GaussianCenter_<Traits2>& rhs) noexcept {
    if(lhs.size() != rhs.size()) return false;
    for(type::size i = 0; i < lhs.size(); ++i)
        if(lhs[i] != rhs[i]) return false;
    return true;
}

template<typename Traits1, typename Traits2>
constexpr bool operator!=(const GaussianCenter_<Traits1>& lhs,
                          const GaussianCenter_<Traits2>& rhs) noexcept {
    return !(lhs == rhs);
}

#undef SHELL_TYPE

} // namespace libchemist::basis_set
