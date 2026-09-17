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
#include <chemist/concepts.hpp>
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/experimental/detail_/float_serialization.hpp>
#include <chemist/experimental/detail_/gaussian_arithmetic.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <chemist/experimental/point/point_common.hpp>
#include <chemist/experimental/point/point_set_common.hpp>
#include <chemist/experimental/traits/primitive_traits.hpp>
#include <cstddef>
#include <utility>
#include <vector>

namespace chemist::experimental {

/** @brief Implements the API shared by Primitive and PrimitiveView.
 *
 *  A Gaussian primitive is the function
 *
 *  @f[
 *    \chi(\vec{r}; \vec{r_0}, c, \zeta, \ell) =
 *      c\, e^{-\zeta \left(\vec{r}-\vec{r_0}\right)^2},
 *  @f]
 *
 *  parameterized by a coefficient @f$c@f$, an exponent @f$\zeta@f$, the point
 *  @f$\vec{r_0}@f$ it is centered on, and its total angular momentum
 *  @f$\ell@f$. @f$\ell@f$ does not appear in the function above --- it is not
 *  needed to *evaluate* the primitive --- but it is needed to *normalize* it
 *  (see normalized_evaluate), and per
 *  docs/source/developer/design/basis_set/normalization.rst that is reason
 *  enough for the primitive to store it.
 *
 *  A primitive can either own its parameters (Primitive) or alias parameters
 *  owned by something else (PrimitiveView). This class, exactly like
 *  PointCommon for Point/PointView, implements the API shared by both exactly
 *  once. The derived class supplies only the six accessors named below and is
 *  passed to *this as a template parameter (the curiously recurring template
 *  pattern), so none of this costs a virtual call.
 *
 *  @tparam DerivedType The class deriving from *this. Must define
 *                      `coefficient_()`, `exponent_()`, `l_()`, and
 *                      `center_()`, each with a const overload, and must
 *                      declare *this a friend.
 *  @tparam PrimitiveType The, possibly const-qualified, Primitive type the
 *                        derived class models. This is what decides whether
 *                        the parameters handed out are mutable, and thus
 *                        whether the setters exist at all.
 */
template<typename DerivedType, typename PrimitiveType>
class PrimitiveCommon {
private:
    /// Struct defining the types for the primitive *this acts like
    using traits_type = ChemistClassTraits<PrimitiveType>;

    /// Enables a method only when *this can mutate the aliased parameters
    template<typename U, typename ReturnType = void>
    using enable_if_mutable_t =
      std::enable_if_t<!std::is_const_v<U>, ReturnType>;

public:
    /// Type *this uses to model the coefficient and the exponent
    using coefficient_type = typename traits_type::coefficient_type;
    using exponent_type    = typename traits_type::exponent_type;

    /// Type of a, possibly mutable, aliasing view of the coefficient/exponent
    using coefficient_reference = typename traits_type::coefficient_reference;
    using exponent_reference    = typename traits_type::exponent_reference;

    /// Type of a read-only, aliasing view of the coefficient/exponent
    using const_coefficient_reference =
      typename traits_type::const_coefficient_reference;
    using const_exponent_reference =
      typename traits_type::const_exponent_reference;

    /// Type *this uses to model the total angular momentum
    using angular_momentum_type = typename traits_type::angular_momentum_type;

    /// Type of a, possibly mutable, reference to the angular momentum
    using angular_momentum_reference =
      typename traits_type::angular_momentum_reference;

    /// Type of a read-only reference to the angular momentum
    using const_angular_momentum_reference =
      typename traits_type::const_angular_momentum_reference;

    /// Type used to model the point *this is centered on
    using center_type = typename traits_type::center_type;

    /// Type of a, possibly mutable, aliasing view of the center
    using center_reference = typename traits_type::center_reference;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference = typename traits_type::const_center_reference;

    /// Type *this uses to model a single Cartesian coordinate
    using coord_type = typename center_type::coord_type;

    /// Type resulting from evaluating *this at a single point
    using numerical_value = coord_type;

    /// Type resulting from evaluating *this at a set of points
    using numerical_vector = std::vector<numerical_value>;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the coefficient of *this.
     *
     *  @return A read-only view of the coefficient.
     *
     *  @throw None No throw guarantee.
     */
    const_coefficient_reference get_coefficient() const {
        return downcast_().coefficient_();
    }

    /** @brief Returns the exponent of *this.
     *
     *  @return A read-only view of the exponent.
     *
     *  @throw None No throw guarantee.
     */
    const_exponent_reference get_exponent() const {
        return downcast_().exponent_();
    }

    /** @brief Returns the total angular momentum of *this.
     *
     *  @return A read-only reference to the angular momentum.
     *
     *  @throw None No throw guarantee.
     */
    const_angular_momentum_reference get_l() const { return downcast_().l_(); }

    /** @brief Returns the point *this is centered on.
     *
     *  @return A read-only view of the center.
     *
     *  @throw None No throw guarantee.
     */
    const_center_reference get_center() const { return downcast_().center_(); }

    // -------------------------------------------------------------------------
    // -- Setters
    // -------------------------------------------------------------------------

    /** @brief Sets the coefficient of *this to @p value.
     *
     *  This method mutates the coefficient in place, so it does not
     *  invalidate any view which may currently be aliasing *this. When *this
     *  aliases read-only state this method does not participate in overload
     *  resolution.
     *
     *  @tparam T The concrete floating-point type of @p value.
     *
     *  @param[in] value The new value of the coefficient.
     *
     *  @throw std::runtime_error if @p T does not match the concrete
     *                            floating-point type the coefficient is
     *                            currently holding. Strong throw guarantee.
     */
    template<concepts::FloatingPoint T, typename U = PrimitiveType>
    enable_if_mutable_t<U> set_coefficient(T value) {
        downcast_().coefficient_() = std::move(value);
    }

    /** @brief Sets the exponent of *this to @p value.
     *
     *  See set_coefficient for the throw and mutation guarantees.
     */
    template<concepts::FloatingPoint T, typename U = PrimitiveType>
    enable_if_mutable_t<U> set_exponent(T value) {
        downcast_().exponent_() = std::move(value);
    }

    /** @brief Sets the total angular momentum of *this to @p value.
     *
     *  When *this aliases read-only state this method does not participate
     *  in overload resolution.
     *
     *  @param[in] value The new angular momentum.
     *
     *  @throw None No throw guarantee.
     */
    template<typename U = PrimitiveType>
    enable_if_mutable_t<U> set_l(angular_momentum_type value) {
        downcast_().l_() = value;
    }

    /** @brief Sets the point *this is centered on to @p r0.
     *
     *  This writes @p r0's coordinates into the center *this already owns or
     *  aliases, rather than rebinding what *this is centered on; that is
     *  consistent with set_coefficient/set_exponent/set_l, all of which
     *  mutate in place. When *this aliases read-only state this method does
     *  not participate in overload resolution.
     *
     *  @tparam OtherDerived The derived type of @p r0.
     *  @tparam OtherPoint The point type @p r0 models.
     *
     *  @param[in] r0 The new center.
     *
     *  @throw std::runtime_error if @p r0's coordinates are not holding the
     *                            same concrete floating-point type as the
     *                            center *this already owns or aliases. Weak
     *                            throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint,
             typename U = PrimitiveType>
    enable_if_mutable_t<U> set_center(
      const PointCommon<OtherDerived, OtherPoint>& r0) {
        downcast_().center_() = r0;
    }

    // -------------------------------------------------------------------------
    // -- Evaluation
    // -------------------------------------------------------------------------

    /** @brief Computes the unnormalized value of *this at the point @p r.
     *
     *  @f[
     *    \chi(\vec{r}) = c\, e^{-\zeta \left(\vec{r}-\vec{r_0}\right)^2}
     *  @f]
     *
     *  This is the raw Gaussian: it does not include the normalization
     *  constant. Use normalized_evaluate for that.
     *
     *  @tparam OtherDerived The derived type of @p r.
     *  @tparam OtherPoint The point type @p r models.
     *
     *  @param[in] r The point where *this should be evaluated.
     *
     *  @return The value of *this at @p r.
     *
     *  @throw std::runtime_error if the coefficient, exponent, center, and
     *                            @p r are not all holding the same concrete
     *                            floating-point type. Strong throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    numerical_value evaluate(
      const PointCommon<OtherDerived, OtherPoint>& r) const {
        auto dr             = r - get_center();
        const coord_type r2 = dr.inner_product(dr);
        auto exponent_term  = detail_::multiply(get_exponent(), as_view_(r2));
        auto neg_term       = detail_::negate(as_view_(exponent_term));
        auto gaussian       = detail_::exp(as_view_(neg_term));
        return detail_::multiply(get_coefficient(), as_view_(gaussian));
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

    /** @brief Computes the primitive normalization constant, @f$N^{\chi}@f$.
     *
     *  See docs/source/developer/design/basis_set/normalization.rst for the
     *  formula and for why this stops at the primitive factor: the
     *  contracted-Gaussian factor @f$N^{G}@f$ and the Cartesian-AO factor
     *  @f$N^{AO}_{ijk}@f$ are not computable from a single primitive.
     *
     *  @return The normalization constant.
     *
     *  @throw std::runtime_error if the exponent is not holding a
     *                            floating-point type chemist knows about.
     *                            Strong throw guarantee.
     */
    numerical_value normalization_constant() const {
        return detail_::primitive_normalization(get_exponent(), get_l());
    }

    /** @brief Computes the normalized value of *this at the point @p r.
     *
     *  Equivalent to `normalization_constant() * evaluate(r)`. See evaluate
     *  and normalization_constant.
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
        auto n_chi = normalization_constant();
        auto value = evaluate(r);
        return detail_::multiply(as_view_(n_chi), as_view_(value));
    }

    /** @brief Computes the normalized value of *this at a series of points.
     *
     *  Equivalent to calling normalized_evaluate on each of @p points. See
     *  the single-point overload and evaluate(point_set).
     */
    template<typename OtherDerived, typename OtherPointSet>
    numerical_vector normalized_evaluate(
      const PointSetCommon<OtherDerived, OtherPointSet>& points) const {
        auto n_chi = normalization_constant();
        numerical_vector rv;
        const auto n = points.size();
        rv.reserve(n);
        for(size_type i = 0; i < n; ++i) {
            auto value = evaluate(points[i]);
            rv.push_back(detail_::multiply(as_view_(n_chi), as_view_(value)));
        }
        return rv;
    }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two primitives are value equal if their coefficients, exponents,
     *  angular momenta, and centers are respectively value equal. Whether
     *  either primitive owns or aliases its state is NOT considered.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherPrimitive The primitive type @p rhs models.
     *
     *  @param[in] rhs The primitive to compare to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename OtherDerived, typename OtherPrimitive>
    bool operator==(
      const PrimitiveCommon<OtherDerived, OtherPrimitive>& rhs) const noexcept {
        return get_coefficient() == rhs.get_coefficient() &&
               get_exponent() == rhs.get_exponent() && get_l() == rhs.get_l() &&
               get_center() == rhs.get_center();
    }

    /** @brief Determines if *this differs from @p rhs.
     *
     *  This method defines "different" as not value equal. See operator== for
     *  the definition of value equal.
     */
    template<typename OtherDerived, typename OtherPrimitive>
    bool operator!=(
      const PrimitiveCommon<OtherDerived, OtherPrimitive>& rhs) const noexcept {
        return !((*this) == rhs);
    }

    /** @brief Serializes *this into @p ar.
     *
     *  Writes the coefficient, the exponent, the angular momentum, and the
     *  center, in that order. Only save is implemented here; loading requires
     *  replacing the state outright, which only a primitive which owns it can
     *  do.
     *
     *  @tparam Archive The type of the cereal output archive.
     *
     *  @param[in,out] ar The archive to write to.
     *
     *  @throw std::runtime_error if the coefficient, exponent, or center can
     *                            not be serialized. Weak throw guarantee.
     */
    template<typename Archive>
    void save(Archive& ar) const {
        detail_::save_float(ar, get_coefficient());
        detail_::save_float(ar, get_exponent());
        ar(get_l());
        get_center().save(ar);
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    PrimitiveCommon() noexcept                                  = default;
    PrimitiveCommon(const PrimitiveCommon&) noexcept            = default;
    PrimitiveCommon(PrimitiveCommon&&) noexcept                 = default;
    PrimitiveCommon& operator=(const PrimitiveCommon&) noexcept = default;
    PrimitiveCommon& operator=(PrimitiveCommon&&) noexcept      = default;
    ~PrimitiveCommon() noexcept                                 = default;
    ///@}

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
     *  The arithmetic helpers return owning Float objects, which then need to
     *  be fed back in as read-only views. Calling as_view() directly on the
     *  temporary would alias an object which is about to die, so the value
     *  has to be named first; this helper is the reminder to do so.
     */
    static detail_::const_float_reference as_view_(const coord_type& value) {
        return value.as_view();
    }
};

} // namespace chemist::experimental
