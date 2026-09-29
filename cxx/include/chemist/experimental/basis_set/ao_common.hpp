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
#include <chemist/experimental/point/point_common.hpp>
#include <chemist/experimental/point/point_set_common.hpp>
#include <chemist/experimental/point/point_view.hpp>
#include <chemist/experimental/traits/ao_traits.hpp>
#include <cstddef>
#include <vector>

namespace chemist::experimental {

/** @brief Implements the API shared by AO and AOView.
 *
 *  AO is the abstract base of the owning atomic orbitals (CartesianAO and,
 *  eventually, SphericalAO) and AOView is the abstract base of the aliasing
 *  ones. The two hierarchies are separate --- an AOView is guaranteed not to
 *  own what it describes, and that guarantee is worth having in the type
 *  system --- but the properties of an atomic orbital do not
 *  depend on ownership status. *this provides the common API for accessing the
 *  AO state, regardless of ownership.
 *
 *  Each public method here forwards to a same-named, trailing-underscore
 *  virtual method on @p DerivedType. That is the only indirection: which of
 *  AO/AOView a caller holds is known statically, so *this costs no virtual
 *  call of its own, while which *kind* of AO is underneath is not, so the
 *  forwarded-to method is virtual.
 *
 *  @tparam DerivedType The class deriving from *this, i.e. AO or AOView. Must
 *                      declare `get_l_()`, `get_center_()`,
 *                      `get_contracted_gaussian_()`,
 *                      `normalization_constant_()`, `evaluate_()`, and
 *                      `normalized_evaluate_()`, and must declare *this a
 *                      friend.
 */
template<typename DerivedType>
class AOCommon {
private:
    /// Struct defining the types for the AO *this acts like
    using traits_type = ChemistClassTraits<AO>;

public:
    /// Type *this uses to model the total angular momentum
    using angular_momentum_type = typename traits_type::angular_momentum_type;

    /// Type of the contracted Gaussian *this is built on
    using contracted_gaussian_type =
      typename traits_type::contracted_gaussian_type;

    /// Type of a read-only, aliasing view of the contracted Gaussian
    using const_contracted_gaussian_reference =
      typename traits_type::const_contracted_gaussian_reference;

    /// Type used to model the point *this is centered on
    using center_type = typename traits_type::center_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference = typename traits_type::const_center_reference;

    /// Type of a read-only view of a point to evaluate *this at
    using const_point_reference = typename traits_type::const_point_reference;

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

    /** @brief Returns the total angular momentum of *this.
     *
     *  For a Cartesian AO this is @f$i + j + k@f$; for a spherical AO it is
     *  the @f$\ell@f$ its component belongs to. Either way it is the
     *  angular momentum of the contracted Gaussian underneath.
     *
     *  @return The total angular momentum.
     *
     *  @throw None No throw guarantee.
     */
    angular_momentum_type get_l() const { return downcast_().get_l_(); }

    /** @brief Returns the point *this is centered on.
     *
     *  @return A read-only view of the center.
     *
     *  @throw None No throw guarantee.
     */
    const_center_reference get_center() const {
        return downcast_().get_center_();
    }

    /** @brief Returns the contracted Gaussian *this is built on.
     *
     *  Every AO is ultimately built on exactly one contracted Gaussian: a
     *  Cartesian AO pairs one with a monomial, and a spherical AO reaches the
     *  one shared by the Cartesian shell it transforms.
     *
     *  @return A read-only view of the contracted Gaussian.
     *
     *  @throw None No throw guarantee.
     */
    const_contracted_gaussian_reference get_contracted_gaussian() const {
        return downcast_().get_contracted_gaussian_();
    }

    // -------------------------------------------------------------------------
    // -- Evaluation
    // -------------------------------------------------------------------------

    /** @brief Returns the full normalization constant of *this.
     *
     *  This is the complete product --- for a Cartesian AO,
     *  @f$N^{\chi} N^{G} N^{AO}_{ijk}@f$ --- and not the truncated
     *  @f$N^{\chi} N^{G}@f$ which integral libraries expect. See
     *  docs/source/developer/design/basis_set/normalization.rst for why both
     *  truncations exist and which consumer wants which.
     *
     *  @return The normalization constant.
     *
     *  @throw std::runtime_error if the parameters of *this are not all
     *                            holding the same concrete floating-point
     *                            type. Strong throw guarantee.
     */
    numerical_value normalization_constant() const {
        return downcast_().normalization_constant_();
    }

    /** @brief Computes the unnormalized value of *this at the point @p r.
     *
     *  @tparam OtherDerived The derived type of @p r.
     *  @tparam OtherPoint The point type @p r models.
     *
     *  @param[in] r The point where *this should be evaluated.
     *
     *  @return The value of *this at @p r.
     *
     *  @throw std::runtime_error if the parameters of *this and @p r are not
     *                            all holding the same concrete
     *                            floating-point type. Strong throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    numerical_value evaluate(
      const PointCommon<OtherDerived, OtherPoint>& r) const {
        return downcast_().evaluate_(as_const_view_(r));
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

    /** @brief Computes the normalized value of *this at the point @p r.
     *
     *  This applies the full normalization constant, i.e. the one reported by
     *  normalization_constant. Note this is not in general
     *  `normalization_constant() * evaluate(r)`: the primitive factors sit
     *  inside the contraction sum and so must be applied before it is taken.
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
        return downcast_().normalized_evaluate_(as_const_view_(r));
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

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    AOCommon() noexcept                           = default;
    AOCommon(const AOCommon&) noexcept            = default;
    AOCommon(AOCommon&&) noexcept                 = default;
    AOCommon& operator=(const AOCommon&) noexcept = default;
    AOCommon& operator=(AOCommon&&) noexcept      = default;
    ~AOCommon() noexcept                          = default;
    ///@}

    /** @brief Wraps taking a read-only view of an arbitrary point.
     *
     *  The evaluation methods above are templated on the point type, but the
     *  virtual methods they forward to can not be (a virtual function can not
     *  be a template), so the point has to be narrowed to one concrete type
     *  first. Going through the coordinate views, rather than relying on a
     *  conversion to const_point_view, is what makes that work for any
     *  PointCommon and not just for a Point. The result aliases @p r, which
     *  outlives the call.
     */
    template<typename OtherDerived, typename OtherPoint>
    static const_point_reference as_const_view_(
      const PointCommon<OtherDerived, OtherPoint>& r) {
        return const_point_reference(r.get_x(), r.get_y(), r.get_z());
    }

private:
    /// Wraps casting *this to a read-only reference to the derived class
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }
};

} // namespace chemist::experimental
