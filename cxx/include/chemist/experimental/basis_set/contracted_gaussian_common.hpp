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
#include <chemist/experimental/basis_set/primitive_view.hpp>
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/experimental/detail_/float_serialization.hpp>
#include <chemist/experimental/detail_/gaussian_arithmetic.hpp>
#include <chemist/experimental/point/point_common.hpp>
#include <chemist/experimental/point/point_set_common.hpp>
#include <chemist/experimental/traits/contracted_gaussian_traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>
#include <vector>

namespace chemist::experimental {

/** @brief Implements the API shared by ContractedGaussian and
 *         ContractedGaussianView.
 *
 *  A (segmented) contracted Gaussian is a linear combination of @f$N@f$
 *  Gaussian primitives which all share one center and one total angular
 *  momentum:
 *
 *  @f[
 *    G(\vec{r}; \vec{r_0}, \vec{d}, \vec{\zeta}, \ell) =
 *      \sum_{p=1}^N d_p\, \chi_p(\vec{r}; \vec{r_0}, \zeta_p, \ell),
 *  @f]
 *
 *  where @f$\chi_p@f$ is the @f$p@f$-th primitive. *this stores the
 *  contraction as a structure of arrays --- one array of coefficients and one
 *  of exponents --- exactly the way PointSet stores a set of points as three
 *  coordinate arrays, plus the ONE shared center and ONE shared angular
 *  momentum every primitive in the contraction aliases. Indexing *this yields
 *  a PrimitiveView aliasing the @f$i@f$-th coefficient, the @f$i@f$-th
 *  exponent, and the shared angular momentum and center; nothing is copied
 *  out of the contraction.
 *
 *  A contracted Gaussian can either own its buffers (ContractedGaussian) or
 *  alias buffers owned by something else (ContractedGaussianView). This
 *  class, exactly like PrimitiveCommon for Primitive/PrimitiveView, implements
 *  the API shared by both exactly once.
 *
 *  @tparam DerivedType The class deriving from *this. Must define
 *                      `coefficient_buffer_()`, `exponent_buffer_()`,
 *                      `l_()`, and `center_()`, each with a const overload,
 *                      and must declare *this a friend.
 *  @tparam CGType The, possibly const-qualified, ContractedGaussian type the
 *                 derived class models.
 */
template<typename DerivedType, typename CGType>
class ContractedGaussianCommon
  : public utilities::IndexableContainerBase<DerivedType> {
private:
    /// Type *this inherits from
    using base_type = utilities::IndexableContainerBase<DerivedType>;

    /// Lets the container base reach at_ and size_
    friend base_type;

    /// Struct defining the types for the contracted Gaussian *this acts like
    using traits_type = ChemistClassTraits<CGType>;

    /// Enables a method only when *this can mutate the aliased parameters
    template<typename U, typename ReturnType = void>
    using enable_if_mutable_t =
      std::enable_if_t<!std::is_const_v<U>, ReturnType>;

public:
    /// Type of a primitive in *this
    using value_type = typename traits_type::primitive_type;

    /// Type acting like a, possibly mutable, reference to a primitive in
    /// *this. This is what indexing *this yields.
    using reference = typename traits_type::primitive_reference;

    /// Type acting like a read-only reference to a primitive in *this
    using const_reference = typename traits_type::const_primitive_reference;

    /// Type *this uses to model a coefficient/exponent
    using coefficient_type = typename traits_type::coefficient_type;
    using exponent_type    = typename traits_type::exponent_type;

    /// Type of a, possibly mutable, aliasing view of a coefficient/exponent
    using coefficient_reference = typename traits_type::coefficient_reference;
    using exponent_reference    = typename traits_type::exponent_reference;

    /// Type of a read-only, aliasing view of a coefficient/exponent
    using const_coefficient_reference =
      typename traits_type::const_coefficient_reference;
    using const_exponent_reference =
      typename traits_type::const_exponent_reference;

    /// Type *this uses to model the (shared) total angular momentum
    using angular_momentum_type = typename traits_type::angular_momentum_type;

    /// Type of a, possibly mutable, reference to the angular momentum
    using angular_momentum_reference =
      typename traits_type::angular_momentum_reference;

    /// Type of a read-only reference to the angular momentum
    using const_angular_momentum_reference =
      typename traits_type::const_angular_momentum_reference;

    /// Type used to model the (shared) point *this is centered on
    using center_type = typename traits_type::center_type;

    /// Type of a, possibly mutable, aliasing view of the center
    using center_reference = typename traits_type::center_reference;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference = typename traits_type::const_center_reference;

    /// Type used to own one parameter's worth of values
    using buffer_type = typename traits_type::buffer_type;

    /// Type of a, possibly mutable, aliasing view of one parameter's array
    using buffer_reference = typename traits_type::buffer_reference;

    /// Type of a read-only, aliasing view of one parameter's array
    using const_buffer_reference = typename traits_type::const_buffer_reference;

    /// Type *this uses to model a single Cartesian coordinate
    using coord_type = typename center_type::coord_type;

    /// Type resulting from evaluating *this at a single point
    using numerical_value = coord_type;

    /// Type resulting from evaluating *this at a set of points
    using numerical_vector = std::vector<numerical_value>;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    // -------------------------------------------------------------------------
    // -- Raw data access
    // -------------------------------------------------------------------------

    /** @brief Returns the array holding every primitive's coefficient.
     *
     *  Escape hatch for consumers which can not accept any C++ abstraction
     *  and need a pointer to contiguous coefficients, e.g., an external
     *  integral library. See PointSetCommon::get_buffer for the same
     *  reasoning about type-erased buffers.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    buffer_reference get_coefficient_buffer() {
        return downcast_().coefficient_buffer_();
    }

    const_buffer_reference get_coefficient_buffer() const {
        return downcast_().coefficient_buffer_();
    }
    ///@}

    /** @brief Returns the array holding every primitive's exponent.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    buffer_reference get_exponent_buffer() {
        return downcast_().exponent_buffer_();
    }

    const_buffer_reference get_exponent_buffer() const {
        return downcast_().exponent_buffer_();
    }
    ///@}

    // -------------------------------------------------------------------------
    // -- Shared (contraction-wide) accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the total angular momentum shared by every primitive in
     *         *this.
     *
     *  @throw None No throw guarantee.
     */
    const_angular_momentum_reference get_l() const { return downcast_().l_(); }

    /** @brief Sets the total angular momentum shared by every primitive in
     *         *this.
     *
     *  This method mutates the angular momentum in place, so every
     *  PrimitiveView previously handed out by *this observes the new value
     *  too. When *this aliases read-only state this method does not
     *  participate in overload resolution.
     *
     *  @throw None No throw guarantee.
     */
    template<typename U = CGType>
    enable_if_mutable_t<U> set_l(angular_momentum_type value) {
        downcast_().l_() = value;
    }

    /** @brief Returns the point shared by every primitive in *this.
     *
     *  @throw None No throw guarantee.
     */
    const_center_reference get_center() const { return downcast_().center_(); }

    /** @brief Sets the point shared by every primitive in *this.
     *
     *  This writes @p r0's coordinates into the center *this already owns or
     *  aliases, exactly like Primitive::set_center. When *this aliases
     *  read-only state this method does not participate in overload
     *  resolution.
     *
     *  @throw std::runtime_error if @p r0's coordinates are not holding the
     *                            same concrete floating-point type as the
     *                            center *this already owns or aliases. Weak
     *                            throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint, typename U = CGType>
    enable_if_mutable_t<U> set_center(
      const PointCommon<OtherDerived, OtherPoint>& r0) {
        downcast_().center_() = r0;
    }

    // -------------------------------------------------------------------------
    // -- Evaluation
    // -------------------------------------------------------------------------

    /** @brief Computes the unnormalized value of *this at the point @p r.
     *
     *  This is the raw sum of the primitives making up the contraction: each
     *  primitive's own coefficient is already applied by
     *  Primitive::evaluate, so this does not include the contracted-Gaussian
     *  normalization constant, N^G. Use normalized_evaluate for that.
     *
     *  A contraction with no primitives evaluates to 0.0.
     *
     *  @throw std::runtime_error if the primitives are not all holding the
     *                            same concrete floating-point type as @p r.
     *                            Strong throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    numerical_value evaluate(
      const PointCommon<OtherDerived, OtherPoint>& r) const {
        const auto n = this->size();
        if(n == 0) return numerical_value(0.0);
        auto acc = (*this)[0].evaluate(r);
        for(size_type i = 1; i < n; ++i) {
            auto term = (*this)[i].evaluate(r);
            acc       = detail_::add(as_view_(acc), as_view_(term));
        }
        return acc;
    }

    /** @brief Computes the unnormalized value of *this at a series of points.
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

    /** @brief Computes the contracted-Gaussian normalization constant, N^G.
     *
     *  See detail_::contracted_gaussian_normalization for the formula.
     *
     *  @throw std::runtime_error if the coefficients and exponents are not
     *                            all holding the same concrete
     *                            floating-point type. Strong throw
     *                            guarantee.
     */
    numerical_value normalization_constant() const {
        return detail_::contracted_gaussian_normalization(
          get_coefficient_buffer(), get_exponent_buffer(), get_l());
    }

    /** @brief Computes the normalized value of *this at the point @p r.
     *
     *  This is NOT `normalization_constant() * evaluate(r)`: evaluate() sums
     *  the RAW primitives (each already weighted by its own coefficient),
     *  while the normalized value must route each term through that
     *  primitive's own normalization constant, N^chi, first:
     *
     *  @f[
     *    N^G \sum_p d_p\, N^{\chi}_p\, \chi_p(\vec{r})
     *  @f]
     *
     *  i.e. `N^G * sum_p primitive_p.normalized_evaluate(r)`, with N^G
     *  applied once, contraction-wide, at the end.
     *
     *  A contraction with no primitives evaluates to 0.0.
     *
     *  @throw std::runtime_error under the same conditions as evaluate and
     *                            normalization_constant. Strong throw
     *                            guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    numerical_value normalized_evaluate(
      const PointCommon<OtherDerived, OtherPoint>& r) const {
        const auto n = this->size();
        if(n == 0) return numerical_value(0.0);
        auto n_g = normalization_constant();
        auto acc = (*this)[0].normalized_evaluate(r);
        for(size_type i = 1; i < n; ++i) {
            auto term = (*this)[i].normalized_evaluate(r);
            acc       = detail_::add(as_view_(acc), as_view_(term));
        }
        return detail_::multiply(as_view_(n_g), as_view_(acc));
    }

    /** @brief Computes the normalized value of *this at a series of points.
     *
     *  Equivalent to calling normalized_evaluate on each of @p points. See
     *  the single-point overload.
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
     *  Two contracted Gaussians are value equal if they hold the same number
     *  of primitives and if, for each @f$i@f$, `(*this)[i] == rhs[i]`. This
     *  reuses PrimitiveCommon::operator==, so it (redundantly) compares each
     *  primitive's shared center and angular momentum every time.
     *
     *  @throw None No throw guarantee.
     */
    template<typename OtherDerived, typename OtherCG>
    bool operator==(const ContractedGaussianCommon<OtherDerived, OtherCG>& rhs)
      const noexcept {
        const auto n = this->size();
        if(n != rhs.size()) return false;
        for(size_type i = 0; i < n; ++i) {
            if((*this)[i] != rhs[i]) return false;
        }
        return true;
    }

    /** @brief Determines if *this differs from @p rhs.
     *
     *  This method defines "different" as not value equal. See operator==
     *  for the definition of value equal.
     */
    template<typename OtherDerived, typename OtherCG>
    bool operator!=(const ContractedGaussianCommon<OtherDerived, OtherCG>& rhs)
      const noexcept {
        return !((*this) == rhs);
    }

    /** @brief Serializes *this into @p ar.
     *
     *  Writes the number of primitives, then the (shared) angular momentum
     *  and center EXACTLY ONCE, then each primitive's coefficient and
     *  exponent, in order. Only save is implemented here; loading requires
     *  replacing the state outright, which only a contracted Gaussian which
     *  owns it can do.
     *
     *  @throw std::runtime_error if the coefficients, exponents, or center
     *                            can not be serialized. Weak throw
     *                            guarantee.
     */
    template<typename Archive>
    void save(Archive& ar) const {
        const auto n = this->size();
        ar(n);
        ar(get_l());
        get_center().save(ar);
        for(size_type i = 0; i < n; ++i) {
            detail_::save_float(ar, (*this)[i].get_coefficient());
            detail_::save_float(ar, (*this)[i].get_exponent());
        }
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    ContractedGaussianCommon() noexcept = default;
    ContractedGaussianCommon(const ContractedGaussianCommon&) noexcept =
      default;
    ContractedGaussianCommon(ContractedGaussianCommon&&) noexcept = default;
    ContractedGaussianCommon& operator=(
      const ContractedGaussianCommon&) noexcept = default;
    ContractedGaussianCommon& operator=(ContractedGaussianCommon&&) noexcept =
      default;
    ~ContractedGaussianCommon() noexcept = default;
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
     *  See PrimitiveCommon::as_view_ for why this reminder-helper exists:
     *  the arithmetic helpers return owning Float objects, which then need
     *  to be fed back in as read-only views, and calling as_view() directly
     *  on the temporary would alias an object which is about to die.
     */
    static detail_::const_float_reference as_view_(const coord_type& value) {
        return value.as_view();
    }

    /** @brief Implements the container base's mutable element access.
     *
     *  The returned view aliases the i-th coefficient, the i-th exponent,
     *  and the SAME shared angular momentum and center every other index
     *  aliases too --- that sharing is intentional, not a bug.
     */
    reference at_(size_type i) {
        return reference(downcast_().coefficient_buffer_().at(i),
                         downcast_().exponent_buffer_().at(i), downcast_().l_(),
                         downcast_().center_());
    }

    /// Implements the container base's read-only element access
    const_reference at_(size_type i) const {
        return const_reference(downcast_().coefficient_buffer_().at(i),
                               downcast_().exponent_buffer_().at(i),
                               downcast_().l_(), downcast_().center_());
    }

    /// Implements the container base's size
    size_type size_() const noexcept {
        return downcast_().coefficient_buffer_().size();
    }
};

} // namespace chemist::experimental
