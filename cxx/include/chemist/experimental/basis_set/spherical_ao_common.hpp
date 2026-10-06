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
#include <chemist/experimental/basis_set/ao_shell.hpp>
#include <chemist/experimental/basis_set/ao_shell_view.hpp>
#include <chemist/experimental/basis_set/cartesian_ao_view.hpp>
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/experimental/detail_/gaussian_arithmetic.hpp>
#include <chemist/experimental/point/point_common.hpp>
#include <chemist/experimental/point/point_set_common.hpp>
#include <chemist/experimental/traits/spherical_ao_traits.hpp>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace chemist::experimental {

/** @brief Implements the API shared by SphericalAO and SphericalAOView.
 *
 *  Per docs/source/developer/design/basis_set/ao_hierarchy.rst, a spherical
 *  AO pairs a Cartesian shell with a component @f$m_\ell@f$, and is the linear
 *  combination
 *
 *  @f[
 *    \mu_{\ell m}(\vec{r}) = \sum_{i+j+k=\ell}
 *                            c^{(ijk)}_{\ell m}\, \mu_{ijk}(\vec{r})
 *  @f]
 *
 *  of that shell's AOs, with the coefficients computed by
 *  detail_::spherical_transform_coefficient rather than stored. The sum runs
 *  over every AO in the shell, so the result does not depend on the order the
 *  shell enumerates them in, and *this accepts a shell of any ordering.
 *
 *  The coefficients carry @f$N^{AO}_{ijk}@f$ (see
 *  docs/source/developer/design/basis_set/normalization.rst), i.e. they assume
 *  Cartesian AOs normalized only up to @f$N^{G}@f$. chemist's Cartesian AOs
 *  apply @f$N^{AO}_{ijk}@f$ themselves in normalized_evaluate, so *this
 *  combines their cg_normalized_evaluate instead, which stops at
 *  @f$N^{G}@f$. evaluate needs no such care, since the Cartesian evaluate does
 *  not apply @f$N^{AO}_{ijk}@f$ in the first place.
 *
 *  Exactly as with CartesianAOCommon, *this is a plain CRTP mixin, and it is
 *  the concrete classes which pick up AO or AOView.
 *
 *  @tparam DerivedType The class deriving from *this. Must define `shell_()`,
 *                      returning a read-only reference to a Cartesian shell
 *                      (an AOShell<CartesianAO> or an
 *                      AOShellView<CartesianAO>), and `m_()`, and must declare
 *                      *this a friend.
 *  @tparam SAOType The, possibly const-qualified, SphericalAO type the derived
 *                  class models. This decides whether set_m exists.
 */
template<typename DerivedType, typename SAOType>
class SphericalAOCommon {
private:
    /// Struct defining the types for the spherical AO *this acts like
    using traits_type = ChemistClassTraits<SAOType>;

    /// Enables a method only when *this can mutate its state
    template<typename U, typename ReturnType = void>
    using enable_if_mutable_t =
      std::enable_if_t<!std::is_const_v<U>, ReturnType>;

public:
    /// Type of the spherical AO *this acts like
    using value_type = typename traits_type::value_type;

    /// Type used to model the total angular momentum
    using angular_momentum_type = typename traits_type::angular_momentum_type;

    /// Type used to model the component, @f$m_\ell@f$
    using magnetic_index_type = typename traits_type::magnetic_index_type;

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

    /** @brief Returns the component, @f$m_\ell@f$, of *this.
     *
     *  @throw None No throw guarantee.
     */
    magnetic_index_type get_m() const noexcept { return downcast_().m_(); }

    /** @brief Returns the total angular momentum of *this.
     *
     *  This is the angular momentum of the Cartesian shell *this is built on;
     *  per the design, it is not stored a second time.
     *
     *  @throw None No throw guarantee.
     */
    angular_momentum_type get_l() const noexcept {
        return downcast_().shell_().get_l();
    }

    /** @brief Returns the point *this is centered on.
     *
     *  Reached through the Cartesian shell, not stored.
     *
     *  @throw None No throw guarantee.
     */
    const_center_reference get_center() const {
        return downcast_().shell_().get_center();
    }

    /** @brief Returns the contracted Gaussian *this is built on.
     *
     *  This is the Cartesian shell's contracted Gaussian, which all the
     *  Cartesian AOs being combined share. The returned view aliases it.
     *
     *  @throw None No throw guarantee.
     */
    const_contracted_gaussian_reference get_contracted_gaussian() const {
        return downcast_().shell_().get_contracted_gaussian();
    }

    // -------------------------------------------------------------------------
    // -- Setters
    // -------------------------------------------------------------------------

    /** @brief Sets the component, @f$m_\ell@f$, of *this.
     *
     *  When *this models a read-only spherical AO this method does not
     *  participate in overload resolution.
     *
     *  @param[in] m The new component. Must satisfy @f$|m| \le \ell@f$.
     *
     *  @throw std::invalid_argument if @p m is out of range for the angular
     *                               momentum of *this. Strong throw guarantee.
     */
    template<typename U = SAOType>
    enable_if_mutable_t<U> set_m(magnetic_index_type m) {
        check_m(get_l(), m);
        downcast_().m_() = m;
    }

    // -------------------------------------------------------------------------
    // -- Evaluation
    // -------------------------------------------------------------------------

    /** @brief Computes the unnormalized value of *this at the point @p r.
     *
     *  This is @f$\sum_{ijk} c^{(ijk)}_{\ell m}@f$ times the unnormalized
     *  value of the Cartesian AO @f$(i,j,k)@f$. The transformation
     *  coefficients are part of the angular function, not a normalization
     *  factor, so they are applied here; what is left out is exactly what the
     *  Cartesian AO's evaluate leaves out, the factors belonging to the
     *  contraction.
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
        return transform_([&r](const auto& ao) { return ao.evaluate(r); });
    }

    /** @brief Computes the unnormalized value of *this at a series of points.
     *
     *  Equivalent to calling evaluate on each of @p points.
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

    /** @brief Returns the normalization constant of *this, @f$N^{G}@f$.
     *
     *  This is the Cartesian shell's constant. The transformation coefficients
     *  normalize the solid harmonic and carry @f$N^{AO}_{ijk}@f$, but they are
     *  part of the angular function, so they are not part of this constant;
     *  normalized_evaluate is what gives the fully normalized value.
     *
     *  @throw std::runtime_error if the coefficients and exponents of the
     *                            contraction are not all holding the same
     *                            concrete floating-point type. Strong throw
     *                            guarantee.
     */
    numerical_value normalization_constant() const {
        return downcast_().shell_().normalization_constant();
    }

    /** @brief Computes the normalized value of *this at the point @p r.
     *
     *  @f[
     *    \mu_{\ell m}(\vec{r}) = \sum_{ijk}
     *      c^{(ijk)}_{\ell m}\, \frac{\mu_{ijk}(\vec{r})}{N^{AO}_{ijk}}
     *  @f]
     *
     *  where @f$\mu_{ijk}/N^{AO}_{ijk}@f$ is the Cartesian AO's
     *  cg_normalized_evaluate. See the description of *this for why
     *  @f$N^{AO}_{ijk}@f$ is left out.
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
        return transform_(
          [&r](const auto& ao) { return ao.cg_normalized_evaluate(r); });
    }

    /** @brief Computes the normalized value of *this at a series of points.
     *
     *  Equivalent to calling normalized_evaluate on each of @p points.
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
     *  Two spherical AOs are value equal if they have the same component and
     *  their contracted Gaussians (which include the angular momentum) are
     *  value equal. The ordering of the Cartesian shells underneath is NOT
     *  considered, since the value of a spherical AO does not depend on it,
     *  and neither is whether either owns or aliases its shell.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherSAO The spherical AO type @p rhs models.
     *
     *  @param[in] rhs The spherical AO to compare to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename OtherDerived, typename OtherSAO>
    bool operator==(
      const SphericalAOCommon<OtherDerived, OtherSAO>& rhs) const noexcept {
        return get_m() == rhs.get_m() &&
               get_contracted_gaussian() == rhs.get_contracted_gaussian();
    }

    /** @brief Determines if *this differs from @p rhs.
     *
     *  This method defines "different" as not value equal. See operator== for
     *  the definition of value equal.
     */
    template<typename OtherDerived, typename OtherSAO>
    bool operator!=(
      const SphericalAOCommon<OtherDerived, OtherSAO>& rhs) const noexcept {
        return !((*this) == rhs);
    }

    /** @brief Throws if @p m is not a valid component for @p l.
     *
     *  Public so that the derived classes' ctors can validate their arguments
     *  the same way set_m does.
     *
     *  @param[in] l The total angular momentum.
     *  @param[in] m The component to check.
     *
     *  @throw std::invalid_argument if @f$|m| > \ell@f$. Strong throw
     *                               guarantee.
     */
    static void check_m(angular_momentum_type l, magnetic_index_type m) {
        const auto abs_m = static_cast<angular_momentum_type>(m < 0 ? -m : m);
        if(abs_m <= l) return;
        throw std::invalid_argument(
          "chemist::experimental::SphericalAO: m_l must satisfy |m_l| <= l.");
    }

    /** @brief Throws if @p shell and @p m can not make a spherical AO.
     *
     *  A spherical AO is a linear combination of Cartesian AOs, so the shell
     *  it is built on must be a Cartesian shell, and @p m must be valid for
     *  that shell's angular momentum. The former is part of @p shell's type,
     *  so it is checked at compile time; only the latter can fail at runtime.
     *  Public for the same reason as check_m.
     *
     *  @tparam ShellType The type of @p shell: AOShell<CartesianAO>,
     *                    AOShellView<CartesianAO>, or a class deriving from
     *                    either.
     *
     *  @param[in] shell The shell to check.
     *  @param[in] m The component to check.
     *
     *  @throw std::invalid_argument if @f$|m| > \ell@f$. Strong throw
     *                               guarantee.
     */
    template<typename ShellType>
    static void check_shell(const ShellType& shell, magnetic_index_type m) {
        static_assert(
          !ChemistClassTraits<AOShell<typename ShellType::ao_type>>::is_pure,
          "A SphericalAO must be built on a Cartesian shell.");
        check_m(shell.get_l(), m);
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    SphericalAOCommon() noexcept                                    = default;
    SphericalAOCommon(const SphericalAOCommon&) noexcept            = default;
    SphericalAOCommon(SphericalAOCommon&&) noexcept                 = default;
    SphericalAOCommon& operator=(const SphericalAOCommon&) noexcept = default;
    SphericalAOCommon& operator=(SphericalAOCommon&&) noexcept      = default;
    ~SphericalAOCommon() noexcept                                   = default;
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

    /// Type of a read-only view of one Cartesian AO of the shell
    using const_cartesian_ao_reference =
      typename traits_type::const_cartesian_ao_reference;

    /** @brief Sums the Cartesian AOs of the shell with the transformation
     *         coefficients.
     *
     *  @param[in] value_of Returns the value (normalized or not) of one
     *                      Cartesian AO. Since the coefficients carry
     *                      @f$N^{AO}_{ijk}@f$, that value must not.
     *
     *  Terms with a zero coefficient are skipped, and the sum starts from the
     *  first term which is not, rather than from a literal zero, so that the
     *  concrete floating-point type of the result is the one the contraction
     *  holds. Every @f$(\ell, m)@f$ has at least one non-zero coefficient.
     */
    template<typename ValueFxn>
    numerical_value transform_(ValueFxn&& value_of) const {
        const auto& shell = downcast_().shell_();
        const auto l      = shell.get_l();
        const auto m      = get_m();

        // The sum is over every (i, j, k) with i + j + k = l, and does not
        // depend on the order they are visited in, so they are enumerated here
        // rather than asked of the shell, whose ordering *this does not know.
        // Each Cartesian AO is built directly on the shell's contracted
        // Gaussian for the same reason.
        std::optional<numerical_value> sum;
        for(size_type i = 0; i <= l; ++i) {
            for(size_type j = 0; i + j <= l; ++j) {
                const size_type k = l - i - j;
                const auto c =
                  detail_::spherical_transform_coefficient(l, m, i, j, k);
                if(c == 0.0) continue;

                const const_cartesian_ao_reference ao(
                  shell.get_contracted_gaussian(), i, j, k);
                const auto ao_value = value_of(ao);
                auto term           = detail_::scale(ao_value.as_view(), c);
                if(sum.has_value())
                    sum = detail_::add(sum->as_view(), term.as_view());
                else
                    sum = std::move(term);
            }
        }
        if(!sum.has_value())
            throw std::logic_error(
              "chemist::experimental::SphericalAO: no Cartesian AO contributes "
              "to this component. This should not be possible.");
        return std::move(*sum);
    }
};

} // namespace chemist::experimental
