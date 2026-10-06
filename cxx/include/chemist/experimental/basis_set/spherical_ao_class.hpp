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
#include <chemist/experimental/basis_set/contracted_gaussian_class.hpp>
#include <chemist/experimental/basis_set/detail_/ao_impl.hpp>
#include <chemist/experimental/basis_set/spherical_ao_common.hpp>
#include <memory>
#include <utility>

namespace chemist::experimental {

/** @brief Models a spherical atomic orbital by value.
 *
 *  See SphericalAOCommon for the function *this models and for the shared
 *  API, and AO for the interface *this satisfies as one of the two kinds of
 *  atomic orbital.
 *
 *  *this owns the Cartesian shell it is built on, and with it that shell's
 *  contracted Gaussian, center, and angular momentum; the only state *this
 *  adds is @f$m_\ell@f$. The shell must be a Cartesian shell, and is held as
 *  an AOShell<CartesianAO>, so that being Cartesian is part of its type. It is
 *  otherwise held polymorphically, because the value of a spherical AO does
 *  not depend on the shell's ordering. The ctors which have no shell to copy
 *  build a CCAShell<CartesianAO>. See SphericalAOView for a class with the
 *  same API which aliases a shell owned by something else.
 *
 *  A default-constructed instance is an s function (@f$m_\ell = 0@f$) built on
 *  an empty CCAShell<CartesianAO>.
 *
 *  @note Every ctor which has to build a shell does so in the source file.
 *        CCAShell<SphericalAO> hands out SphericalAOView objects, so the
 *        shell headers include this one, and this one can not include them.
 *
 *  @note Unlike CartesianAO, *this is not (yet) serializable. Serializing it
 *        requires serializing a polymorphic shell, which needs a registration
 *        scheme that does not exist yet.
 */
class SphericalAO : public SphericalAOCommon<SphericalAO, SphericalAO>,
                    public detail_::AOImpl<SphericalAO> {
private:
    /// Type implementing the API shared with SphericalAOView
    using common_type = SphericalAOCommon<SphericalAO, SphericalAO>;

    /// Type implementing the AO interface in terms of *this
    using impl_type = detail_::AOImpl<SphericalAO>;

    /// Lets the CRTP base reach shell_() and m_()
    friend common_type;

public:
    /// Type of the (polymorphic) Cartesian shell *this is built on
    using shell_type = typename ChemistClassTraits<SphericalAO>::shell_type;

    /// Type of the pointer *this owns its shell through
    using shell_pointer = typename shell_type::pointer;

    /// Pull the shared API's types into *this's API
    ///@{
    using typename common_type::angular_momentum_type;
    using typename common_type::center_type;
    using typename common_type::const_center_reference;
    using typename common_type::const_contracted_gaussian_reference;
    using typename common_type::contracted_gaussian_type;
    using typename common_type::coord_type;
    using typename common_type::magnetic_index_type;
    using typename common_type::numerical_value;
    using typename common_type::numerical_vector;
    using typename common_type::size_type;
    using typename common_type::value_type;
    ///@}

    /// Pull the AO interface's types into *this's API
    ///@{
    using typename AO::base_pointer;
    using typename AO::const_base_reference;
    ///@}

    /// Type of a read-only reference to the Cartesian shell *this is built on
    using const_shell_reference = const shell_type&;

    /** @brief Resolves the shared API against the AO interface.
     *
     *  See the corresponding block in CartesianAO.
     */
    ///@{
    using common_type::evaluate;
    using common_type::get_center;
    using common_type::get_contracted_gaussian;
    using common_type::get_l;
    using common_type::normalization_constant;
    using common_type::normalized_evaluate;
    ///@}

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates an s function built on an empty CCAShell<CartesianAO>.
     *
     *  @throw std::bad_alloc if there is a problem allocating the shell.
     *                        Strong throw guarantee.
     */
    SphericalAO();

    /** @brief Creates the component @p m of the Cartesian shell built on
     *         @p cg.
     *
     *  The shell is built as a CCAShell<CartesianAO> owning @p cg.
     *
     *  @param[in] cg The contracted Gaussian to build *this on.
     *  @param[in] m The component. Must satisfy @f$|m| \le \ell@f$, where
     *               @f$\ell@f$ is @p cg's angular momentum.
     *
     *  @throw std::invalid_argument if @p m is out of range. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the shell.
     *                        Strong throw guarantee.
     */
    SphericalAO(contracted_gaussian_type cg, magnetic_index_type m);

    /** @brief Creates the component @p m of @p shell.
     *
     *  *this takes a deep copy of @p shell, keeping its ordering. See
     *  SphericalAOView for a spherical AO which aliases the shell instead.
     *
     *  @param[in] shell The Cartesian shell to build *this from.
     *  @param[in] m The component. Must satisfy @f$|m| \le \ell@f$, where
     *               @f$\ell@f$ is @p shell's angular momentum.
     *
     *  @throw std::invalid_argument if @p m is out of range. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is a problem copying @p shell. Strong
     *                        throw guarantee.
     */
    SphericalAO(const shell_type& shell, magnetic_index_type m) :
      m_shell_((check_shell(shell, m), shell.clone())), m_m_(m) {}

    /** @brief Constructs a SphericalAO from the provided parameters.
     *
     *  The argument order follows CartesianAO's ctor, with @f$(\ell, m)@f$ in
     *  the place of @f$(i, j, k)@f$. The Cartesian shell is built as a
     *  CCAShell<CartesianAO>.
     *
     *  @param[in] cbegin Iterator to the beginning of the container holding
     *                    the coefficients.
     *  @param[in] cend Iterator to the end of the container holding the
     *                  coefficients.
     *  @param[in] ebegin Iterator to the beginning of the container holding
     *                    the exponents.
     *  @param[in] eend Iterator to the end of the container holding the
     *                  exponents.
     *  @param[in] l The total angular momentum.
     *  @param[in] m The component. Must satisfy @f$|m| \le \ell@f$.
     *  @param[in] x The x-coordinate where *this will be centered.
     *  @param[in] y The y-coordinate where *this will be centered.
     *  @param[in] z The z-coordinate where *this will be centered.
     *
     *  @throw std::invalid_argument if @p m is out of range, or if the
     *                               coefficient and exponent ranges do not
     *                               have the same length. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        shell. Strong throw guarantee.
     */
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    SphericalAO(CoefBeginItr&& cbegin, CoefEndItr&& cend, ExpBeginItr&& ebegin,
                ExpEndItr&& eend, angular_momentum_type l,
                magnetic_index_type m, coord_type x, coord_type y,
                coord_type z) :
      SphericalAO(
        std::forward<CoefBeginItr>(cbegin), std::forward<CoefEndItr>(cend),
        std::forward<ExpBeginItr>(ebegin), std::forward<ExpEndItr>(eend), l, m,
        center_type(std::move(x), std::move(y), std::move(z))) {}

    /** @brief Constructs a SphericalAO from the provided parameters.
     *
     *  This ctor is the same as the other range ctor except that it takes an
     *  already existing point to center *this on.
     *
     *  @param[in] cbegin Iterator to the beginning of the container holding
     *                    the coefficients.
     *  @param[in] cend Iterator to the end of the container holding the
     *                  coefficients.
     *  @param[in] ebegin Iterator to the beginning of the container holding
     *                    the exponents.
     *  @param[in] eend Iterator to the end of the container holding the
     *                  exponents.
     *  @param[in] l The total angular momentum.
     *  @param[in] m The component. Must satisfy @f$|m| \le \ell@f$.
     *  @param[in] center Where *this is centered in Cartesian space.
     *
     *  @throw std::invalid_argument if @p m is out of range, or if the
     *                               coefficient and exponent ranges do not
     *                               have the same length. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        shell. Strong throw guarantee.
     */
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    SphericalAO(CoefBeginItr&& cbegin, CoefEndItr&& cend, ExpBeginItr&& ebegin,
                ExpEndItr&& eend, angular_momentum_type l,
                magnetic_index_type m, center_type center) :
      SphericalAO(contracted_gaussian_type(std::forward<CoefBeginItr>(cbegin),
                                           std::forward<CoefEndItr>(cend),
                                           std::forward<ExpBeginItr>(ebegin),
                                           std::forward<ExpEndItr>(eend), l,
                                           std::move(center)),
                  m) {}

    /** @brief Creates a deep copy of @p other.
     *
     *  The copy's shell has the same ordering as @p other's.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    SphericalAO(const SphericalAO& other);

    /** @brief Takes ownership of @p other's state.
     *
     *  @note @p other is left without a shell, and may only be assigned to or
     *        destroyed.
     *
     *  @throw None No throw guarantee.
     */
    SphericalAO(SphericalAO&& other) noexcept = default;

    /** @brief Overwrites *this with a deep copy of @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    SphericalAO& operator=(const SphericalAO& other);

    /// Overwrites *this with @p other's state. See the move ctor.
    SphericalAO& operator=(SphericalAO&& other) noexcept = default;

    /// Default, no-throw dtor
    ~SphericalAO() noexcept override = default;

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the Cartesian shell *this is built on.
     *
     *  @throw None No throw guarantee.
     */
    const_shell_reference get_cartesian_shell() const noexcept {
        return *m_shell_;
    }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @throw None No throw guarantee.
     */
    void swap(SphericalAO& other) noexcept;

private:
    /// Implements the CRTP base's state access
    ///@{
    const_shell_reference shell_() const noexcept { return *m_shell_; }
    magnetic_index_type& m_() noexcept { return m_m_; }
    magnetic_index_type m_() const noexcept { return m_m_; }
    ///@}

    /** @brief Implements the AO interface in terms of the shared API.
     *
     *  clone_ and are_equal_ are not here; AOImpl implements those from *this's
     *  copy ctor and operator==.
     */
    ///@{
    typename AO::angular_momentum_type get_l_() const noexcept override {
        return common_type::get_l();
    }
    typename AO::const_center_reference get_center_() const override {
        return common_type::get_center();
    }
    typename AO::const_contracted_gaussian_reference get_contracted_gaussian_()
      const override {
        return common_type::get_contracted_gaussian();
    }
    typename AO::numerical_value normalization_constant_() const override {
        return common_type::normalization_constant();
    }
    typename AO::numerical_value evaluate_(
      typename AO::const_point_reference r) const override {
        return common_type::evaluate(r);
    }
    typename AO::numerical_value normalized_evaluate_(
      typename AO::const_point_reference r) const override {
        return common_type::normalized_evaluate(r);
    }
    ///@}

    /// The Cartesian shell whose AOs *this is a linear combination of
    shell_pointer m_shell_;

    /// The component, @f$m_\ell@f$, of *this
    magnetic_index_type m_m_ = 0;
};

} // namespace chemist::experimental
