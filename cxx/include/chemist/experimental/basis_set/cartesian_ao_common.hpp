/*
 * Copyright 2026 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <chemist/experimental/basis_set/contracted_gaussian_view.hpp>
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/experimental/detail_/gaussian_arithmetic.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <chemist/experimental/point/point_common.hpp>
#include <chemist/experimental/point/point_set_common.hpp>
#include <chemist/experimental/traits/cartesian_ao_traits.hpp>
#include <cstddef>
#include <type_traits>
#include <vector>

namespace chemist::experimental {

/** @brief Implements the API shared by CartesianAO and CartesianAOView.
 *
 *  A Cartesian atomic orbital is a contracted Gaussian paired with a monomial:
 *
 *  @f[
 *    \mu_{ijk}(\vec{r}) = N^{AO}_{ijk}\, x^i y^j z^k\,
 *                         G(\vec{r}; \vec{r_0}, \vec{d}, \vec{\zeta}, \ell),
 *    \qquad \ell = i + j + k,
 *  @f]
 *
 *  where @f$(x, y, z)@f$ are measured from the center the contracted Gaussian
 *  is on. Per
 *  docs/source/developer/design/basis_set/ao_hierarchy.rst the pairing is
 *  constrained: @f$i + j + k@f$ must equal the contracted Gaussian's angular
 *  momentum, and it is that constraint which lets *this store @f$(i,j,k)@f$
 *  only, and report @f$\ell@f$ as their sum, rather than storing @f$\ell@f$ a
 *  second time.
 *
 *  A Cartesian AO can either own its contracted Gaussian (CartesianAO) or
 *  alias one owned by something else (CartesianAOView). *this, exactly like
 *  ContractedGaussianCommon for ContractedGaussian/ContractedGaussianView,
 *  implements the API shared by both exactly once. Note that *this does not
 *  derive from AO or AOView --- it is a plain CRTP mixin, and it is the
 *  concrete classes which pick up one of those two bases. The two kinds of
 *  polymorphism are orthogonal: the value/view distinction is resolved
 *  statically, here, and the Cartesian/spherical distinction dynamically, by
 *  the abstract base.
 *
 *  @tparam DerivedType The class deriving from *this. Must define
 *                      `contracted_gaussian_()`, `i_()`, `j_()`, and `k_()`,
 *                      and must declare *this a friend.
 *  @tparam CAOType The, possibly const-qualified, CartesianAO type the derived
 *                  class models. This is what decides whether the state
 *                  handed out is mutable, and thus whether the setters exist
 *                  at all.
 */
template<typename DerivedType, typename CAOType>
class CartesianAOCommon {
private:
    /// Struct defining the types for the Cartesian AO *this acts like
    using traits_type = ChemistClassTraits<CAOType>;

    /// Enables a method only when *this can mutate the aliased state
    template<typename U, typename ReturnType = void>
    using enable_if_mutable_t =
      std::enable_if_t<!std::is_const_v<U>, ReturnType>;

public:
    /// Type of the Cartesian AO *this acts like
    using value_type = typename traits_type::value_type;

    /// Type *this uses to model one Cartesian power
    using angular_index_type = typename traits_type::angular_index_type;

    /// Type *this uses to model the total angular momentum
    using angular_momentum_type = typename traits_type::angular_momentum_type;

    /// Type of the contracted Gaussian *this is built on
    using contracted_gaussian_type =
      typename traits_type::contracted_gaussian_type;

    /// Type of a, possibly mutable, aliasing view of the contracted Gaussian
    using contracted_gaussian_reference =
      typename traits_type::contracted_gaussian_reference;

    /// Type of a read-only, aliasing view of the contracted Gaussian
    using const_contracted_gaussian_reference =
      typename traits_type::const_contracted_gaussian_reference;

    /// Type used to model the point *this is centered on
    using center_type = typename traits_type::center_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference = typename traits_type::const_center_reference;

    /// Type *this uses to model a single Cartesian coordinate
    using coord_type = typename traits_type::coord_type;

    /// Type resulting from evaluating *this at a single point
    using numerical_value = coord_type;

    /// Type resulting from evaluating *this at a set of points
    using numerical_vector = std::vector<numerical_value>;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the power of @f$x@f$ in the monomial of *this.
     *
     *  @throw None No throw guarantee.
     */
    angular_index_type get_i() const noexcept { return downcast_().i_(); }

    /** @brief Returns the power of @f$y@f$ in the monomial of *this.
     *
     *  @throw None No throw guarantee.
     */
    angular_index_type get_j() const noexcept { return downcast_().j_(); }

    /** @brief Returns the power of @f$z@f$ in the monomial of *this.
     *
     *  @throw None No throw guarantee.
     */
    angular_index_type get_k() const noexcept { return downcast_().k_(); }

    /** @brief Returns the total angular momentum of *this.
     *
     *  This is @f$i + j + k@f$, which by the constraint documented on *this is
     *  also the angular momentum of the contracted Gaussian underneath. It is
     *  computed rather than stored precisely so that the two can not disagree.
     *
     *  @throw None No throw guarantee.
     */
    angular_momentum_type get_l() const noexcept {
        return get_i() + get_j() + get_k();
    }

    /** @brief Returns the contracted Gaussian *this is built on.
     *
     *  The returned view aliases the contracted Gaussian *this owns or
     *  aliases; it is not a copy, so writing through it (when *this models a
     *  mutable Cartesian AO) writes into *this.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    contracted_gaussian_reference get_contracted_gaussian() {
        return downcast_().contracted_gaussian_();
    }

    const_contracted_gaussian_reference get_contracted_gaussian() const {
        return downcast_().contracted_gaussian_();
    }
    ///@}

    /** @brief Returns the point *this is centered on.
     *
     *  Per docs/source/developer/design/basis_set/ao_hierarchy.rst the center
     *  is owned further up the hierarchy and reached, not stored, by
     *  everything below; *this reaches it through its contracted Gaussian
     *  rather than keeping a second copy which could drift.
     *
     *  @throw None No throw guarantee.
     */
    const_center_reference get_center() const {
        return get_contracted_gaussian().get_center();
    }

    // -------------------------------------------------------------------------
    // -- Setters
    // -------------------------------------------------------------------------

    /** @brief Sets the power of @f$x@f$ in the monomial of *this.
     *
     *  Because @f$\ell@f$ is @f$i + j + k@f$, changing a power changes the
     *  total angular momentum, and the contracted Gaussian underneath needs
     *  @f$\ell@f$ in order to normalize itself. This method therefore also
     *  writes the new @f$\ell@f$ into that contracted Gaussian, keeping the
     *  constraint documented on *this satisfied. That write is in place, so
     *  every view aliasing the same contracted Gaussian observes it.
     *
     *  When *this models a read-only Cartesian AO this method does not
     *  participate in overload resolution.
     *
     *  @param[in] value The new power of @f$x@f$.
     *
     *  @throw None No throw guarantee.
     */
    template<typename U = CAOType>
    enable_if_mutable_t<U> set_i(angular_index_type value) {
        downcast_().i_() = value;
        sync_l_();
    }

    /** @brief Sets the power of @f$y@f$ in the monomial of *this.
     *
     *  See set_i for the effect on the contracted Gaussian's angular momentum.
     */
    template<typename U = CAOType>
    enable_if_mutable_t<U> set_j(angular_index_type value) {
        downcast_().j_() = value;
        sync_l_();
    }

    /** @brief Sets the power of @f$z@f$ in the monomial of *this.
     *
     *  See set_i for the effect on the contracted Gaussian's angular momentum.
     */
    template<typename U = CAOType>
    enable_if_mutable_t<U> set_k(angular_index_type value) {
        downcast_().k_() = value;
        sync_l_();
    }

    // -------------------------------------------------------------------------
    // -- Evaluation
    // -------------------------------------------------------------------------

    /** @brief Computes the unnormalized value of *this at the point @p r.
     *
     *  This is the raw contracted Gaussian times the raw monomial: it includes
     *  neither the Cartesian-AO factor @f$N^{AO}_{ijk}@f$ nor either of the
     *  factors belonging to the contraction. Use normalized_evaluate for
     *  those.
     *
     *  @tparam OtherDerived The derived type of @p r.
     *  @tparam OtherPoint The point type @p r models.
     *
     *  @param[in] r The point where *this should be evaluated.
     *
     *  @return The value of *this at @p r.
     *
     *  @throw std::runtime_error if the parameters of *this and @p r are not
     *                            all holding the same concrete floating-point
     *                            type. Strong throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    numerical_value evaluate(
      const PointCommon<OtherDerived, OtherPoint>& r) const {
        return apply_monomial_(get_contracted_gaussian().evaluate(r), r);
    }

    /** @brief Computes the unnormalized value of *this at a series of points.
     *
     *  @tparam OtherDerived The derived type of @p points.
     *  @tparam OtherPointSet The point-set type @p points models.
     *
     *  @param[in] points The points where *this should be evaluated.
     *
     *  @return The value of *this at each of @p points, in order.
     *
     *  @throw std::runtime_error under the same conditions as the single
     *                            point overload. Strong throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        return. Strong throw guarantee.
     */
    template<typename OtherDerived, typename OtherPointSet>
    numerical_vector evaluate(
      const PointSetCommon<OtherDerived, OtherPointSet>& points) const {
        numerical_vector rv;
        const auto n = points.size();
        rv.reserve(n);
        for(size_type i = 0; i < n; ++i) rv.push_back(evaluate(points[i]));
        return rv;
    }

    /** @brief Returns the normalization constant of *this.
     *
     *  This is @f$N^{AO}_{ijk} N^{G}@f$: the contracted Gaussian's own
     *  constant with the Cartesian-AO factor *this owns applied to it. Per
     *  docs/source/developer/design/basis_set/normalization.rst the remaining
     *  factor, @f$N^{\chi}@f$, is not part of this product because it differs
     *  from primitive to primitive and so sits inside the contraction sum
     *  rather than in front of it; normalized_evaluate is what applies it.
     *  This is the same division of labor ContractedGaussianCommon already
     *  makes between its own normalization_constant and normalized_evaluate.
     *
     *  @return The normalization constant.
     *
     *  @throw std::runtime_error if the coefficients and exponents of the
     *                            contraction are not all holding the same
     *                            concrete floating-point type. Strong throw
     *                            guarantee.
     */
    numerical_value normalization_constant() const {
        auto n_g = get_contracted_gaussian().normalization_constant();
        return detail_::scale(as_view_(n_g), n_ao_());
    }

    /** @brief Computes the value of *this at the point @p r, normalized only
     *         up to the contracted Gaussian.
     *
     *  @f[
     *    x^i y^j z^k\, N^{G} \sum_p d_p\, N^{\chi}_p\, \chi_p(\vec{r})
     *  @f]
     *
     *  i.e. the contracted Gaussian's own normalized value times the
     *  monomial, without the Cartesian-AO factor @f$N^{AO}_{ijk}@f$. Per
     *  docs/source/developer/design/basis_set/normalization.rst this is the
     *  convention integral libraries use, and it is what the
     *  Cartesian-to-spherical transformation coefficients assume, since those
     *  coefficients already carry @f$N^{AO}_{ijk}@f$. Use normalized_evaluate
     *  for the fully normalized value.
     *
     *  @tparam OtherDerived The derived type of @p r.
     *  @tparam OtherPoint The point type @p r models.
     *
     *  @param[in] r The point where *this should be evaluated.
     *
     *  @return The value of *this at @p r, normalized up to @f$N^{G}@f$.
     *
     *  @throw std::runtime_error under the same conditions as evaluate.
     *                            Strong throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    numerical_value cg_normalized_evaluate(
      const PointCommon<OtherDerived, OtherPoint>& r) const {
        return apply_monomial_(get_contracted_gaussian().normalized_evaluate(r),
                               r);
    }

    /** @brief Computes the value of *this at a series of points, normalized
     *         only up to the contracted Gaussian.
     *
     *  Equivalent to calling cg_normalized_evaluate on each of @p points. See
     *  the single-point overload and evaluate(point_set).
     */
    template<typename OtherDerived, typename OtherPointSet>
    numerical_vector cg_normalized_evaluate(
      const PointSetCommon<OtherDerived, OtherPointSet>& points) const {
        numerical_vector rv;
        const auto n = points.size();
        rv.reserve(n);
        for(size_type i = 0; i < n; ++i)
            rv.push_back(cg_normalized_evaluate(points[i]));
        return rv;
    }

    /** @brief Computes the normalized value of *this at the point @p r.
     *
     *  @f[
     *    \mu_{ijk}(\vec{r}) = N^{AO}_{ijk}\, x^i y^j z^k\, N^{G}
     *                         \sum_p d_p\, N^{\chi}_p\, \chi_p(\vec{r})
     *  @f]
     *
     *  i.e. cg_normalized_evaluate times the Cartesian-AO factor. Note this is
     *  NOT `normalization_constant() * evaluate(r)`, for the reason given on
     *  normalization_constant: each primitive's @f$N^{\chi}@f$ has to be
     *  applied before the contraction is summed.
     *
     *  @tparam OtherDerived The derived type of @p r.
     *  @tparam OtherPoint The point type @p r models.
     *
     *  @param[in] r The point where *this should be evaluated.
     *
     *  @return The normalized value of *this at @p r.
     *
     *  @throw std::runtime_error under the same conditions as evaluate.
     *                            Strong throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    numerical_value normalized_evaluate(
      const PointCommon<OtherDerived, OtherPoint>& r) const {
        return detail_::scale(as_view_(cg_normalized_evaluate(r)), n_ao_());
    }

    /** @brief Computes the normalized value of *this at a series of points.
     *
     *  Equivalent to calling normalized_evaluate on each of @p points. See
     *  the single-point overload and evaluate(point_set).
     */
    template<typename OtherDerived, typename OtherPointSet>
    numerical_vector normalized_evaluate(
      const PointSetCommon<OtherDerived, OtherPointSet>& points) const {
        numerical_vector rv;
        const auto n = points.size();
        rv.reserve(n);
        for(size_type i = 0; i < n; ++i)
            rv.push_back(normalized_evaluate(points[i]));
        return rv;
    }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two Cartesian AOs are value equal if their Cartesian powers are equal
     *  and their contracted Gaussians are value equal. Whether either owns or
     *  aliases its contracted Gaussian is NOT considered.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherCAO The Cartesian AO type @p rhs models.
     *
     *  @param[in] rhs The Cartesian AO to compare to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename OtherDerived, typename OtherCAO>
    bool operator==(
      const CartesianAOCommon<OtherDerived, OtherCAO>& rhs) const noexcept {
        return get_i() == rhs.get_i() && get_j() == rhs.get_j() &&
               get_k() == rhs.get_k() &&
               get_contracted_gaussian() == rhs.get_contracted_gaussian();
    }

    /** @brief Determines if *this differs from @p rhs.
     *
     *  This method defines "different" as not value equal. See operator== for
     *  the definition of value equal.
     */
    template<typename OtherDerived, typename OtherCAO>
    bool operator!=(
      const CartesianAOCommon<OtherDerived, OtherCAO>& rhs) const noexcept {
        return !((*this) == rhs);
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    CartesianAOCommon() noexcept                                    = default;
    CartesianAOCommon(const CartesianAOCommon&) noexcept            = default;
    CartesianAOCommon(CartesianAOCommon&&) noexcept                 = default;
    CartesianAOCommon& operator=(const CartesianAOCommon&) noexcept = default;
    CartesianAOCommon& operator=(CartesianAOCommon&&) noexcept      = default;
    ~CartesianAOCommon() noexcept                                   = default;
    ///@}

    /// Wraps computing the Cartesian-AO normalization factor of *this
    double n_ao_() const noexcept {
        return detail_::cartesian_ao_normalization(get_i(), get_j(), get_k());
    }

private:
    /// Wraps casting *this to the derived class
    DerivedType& downcast_() noexcept {
        return static_cast<DerivedType&>(*this);
    }

    /// Wraps casting *this to a read-only reference to the derived class
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }

    /** @brief Wraps taking a read-only view of an owning Float.
     *
     *  See PrimitiveCommon::as_view_ for why this reminder-helper exists: the
     *  arithmetic helpers return owning Float objects, which then need to be
     *  fed back in as read-only views, and calling as_view() on the temporary
     *  directly would alias an object which is about to die.
     */
    static detail_::const_float_reference as_view_(const coord_type& value) {
        return value.as_view();
    }

    /// Writes the current i+j+k into the contracted Gaussian *this is built on
    void sync_l_() { get_contracted_gaussian().set_l(get_l()); }

    /** @brief Multiplies @p value by the monomial of *this, evaluated at @p r.
     *
     *  The monomial is @f$x^i y^j z^k@f$ with @f$(x,y,z)@f$ measured from the
     *  center of *this. Each factor is multiplied in one at a time, starting
     *  from @p value rather than from a literal 1, so that the concrete
     *  floating-point type of the result is the one the contraction is already
     *  holding; a literal would be a double, which is not necessarily what
     *  the parameters are.
     */
    template<typename OtherDerived, typename OtherPoint>
    numerical_value apply_monomial_(
      numerical_value value,
      const PointCommon<OtherDerived, OtherPoint>& r) const {
        const Point dr   = r - get_center();
        auto multiply_in = [&value](detail_::const_float_reference coord,
                                    angular_index_type power) {
            for(angular_index_type p = 0; p < power; ++p)
                value = detail_::multiply(as_view_(value), coord);
        };
        multiply_in(dr.get_x(), get_i());
        multiply_in(dr.get_y(), get_j());
        multiply_in(dr.get_z(), get_k());
        return value;
    }
};

} // namespace chemist::experimental
