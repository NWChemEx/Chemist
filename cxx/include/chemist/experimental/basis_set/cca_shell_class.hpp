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
#include <chemist/experimental/basis_set/cca_shell_common.hpp>
#include <chemist/experimental/basis_set/contracted_gaussian_class.hpp>
#include <chemist/experimental/basis_set/detail_/ao_shell_impl.hpp>
#include <memory>
#include <utility>

namespace chemist::experimental {

/** @brief Models a shell, in CCA order, by value.
 *
 *  See CCAShellCommon for the ordering and the shared API, and AOShell for the
 *  interface *this satisfies as one of the shell orderings.
 *
 *  *this owns the one contracted Gaussian all of its AOs are built on, and
 *  with it the shell's center and angular momentum. Indexing *this hands out
 *  views, never copies. See CCAShellView for a class with the same API which
 *  aliases a contracted Gaussian owned by something else.
 *
 *  A default-constructed instance is an s shell built on a contraction with
 *  no primitives.
 *
 *  @tparam AOType The kind of AO *this holds. CCAShell<CartesianAO> is a
 *                 Cartesian shell and CCAShell<SphericalAO> is a pure one.
 */
template<typename AOType>
class CCAShell : public CCAShellCommon<CCAShell<AOType>, CCAShell<AOType>>,
                 public detail_::AOShellImpl<CCAShell<AOType>> {
private:
    /// Type implementing the API shared with CCAShellView
    using common_type = CCAShellCommon<CCAShell<AOType>, CCAShell<AOType>>;

    /// Type implementing the AOShell interface in terms of *this
    using impl_type = detail_::AOShellImpl<CCAShell<AOType>>;

    /// Lets the CRTP base reach contracted_gaussian_()
    friend common_type;

    /// Lets a view alias *this's contracted Gaussian directly
    template<typename CCAShellType>
    friend class CCAShellView;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename common_type::angular_index_type;
    using typename common_type::angular_momentum_type;
    using typename common_type::ao_type;
    using typename common_type::cartesian_powers_type;
    using typename common_type::center_type;
    using typename common_type::const_ao_reference;
    using typename common_type::const_cartesian_ao_reference;
    using typename common_type::const_cartesian_shell_reference;
    using typename common_type::const_center_reference;
    using typename common_type::const_contracted_gaussian_reference;
    using typename common_type::contracted_gaussian_reference;
    using typename common_type::contracted_gaussian_type;
    using typename common_type::coord_type;
    using typename common_type::magnetic_index_type;
    using typename common_type::numerical_value;
    using typename common_type::size_type;
    using typename common_type::value_type;
    ///@}

    /// Pull the AOShell interface's types into *this's API
    ///@{
    using typename AOShell::base_pointer;
    using typename AOShell::const_ao_view_reference;
    using typename AOShell::const_base_reference;
    using typename AOShell::view_pointer;
    ///@}

    /// Type of a read-only view of *this
    using const_view_type =
      typename ChemistClassTraits<CCAShell<AOType>>::const_view_type;

    /** @brief Resolves the shared API against the AOShell interface.
     *
     *  See the corresponding block in CartesianAO: *this inherits each of
     *  these from both CCAShellCommon and AOShell, and these declarations pick
     *  the non-virtual implementations for a caller holding a CCAShell. Note
     *  that for at/operator[] the two differ in return type: the AOShell
     *  versions return a reference to a polymorphic AOView, while these return
     *  a reference to the concrete view, since a caller holding a CCAShell
     *  knows which it is. Either way it is the same, stored, view.
     */
    ///@{
    using common_type::at;
    using common_type::cartesian_powers;
    using common_type::get_center;
    using common_type::get_contracted_gaussian;
    using common_type::get_l;
    using common_type::is_cartesian;
    using common_type::is_pure;
    using common_type::magnetic_index;
    using common_type::normalization_constant;
    using common_type::operator[];
    using common_type::size;
    ///@}

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates an s shell built on an empty contraction.
     *
     *  Default constructed shells contain a single AO which is identical to
     *  the zero function.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    CCAShell() = default;

    /** @brief Creates a shell built on @p cg.
     *
     *  A shell is exactly one contracted Gaussian, so *this can be made from
     *  one directly; its angular momentum and center are @p cg's.
     *
     *  @param[in] cg The contracted Gaussian *this will own.
     *
     *  @throw None No throw guarantee.
     */
    explicit CCAShell(contracted_gaussian_type cg) noexcept :
      m_cg_(std::move(cg)) {}

    /** @brief Constructs a CCAShell from the provided parameters.
     *
     *  The arguments are forwarded, in order, to ContractedGaussian's ctor.
     *
     *  @param[in] cbegin Iterator to the beginning of the container holding
     *                    the coefficients.
     *  @param[in] cend Iterator to the end of the container holding the
     *                  coefficients.
     *  @param[in] ebegin Iterator to the beginning of the container holding
     *                    the exponents.
     *  @param[in] eend Iterator to the end of the container holding the
     *                  exponents.
     *  @param[in] l The total angular momentum of the shell.
     *  @param[in] x The x-coordinate where *this will be centered.
     *  @param[in] y The y-coordinate where *this will be centered.
     *  @param[in] z The z-coordinate where *this will be centered.
     *
     *  @throw std::invalid_argument if the coefficient and exponent ranges do
     *                               not have the same length. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        contraction. Strong throw guarantee.
     */
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    CCAShell(CoefBeginItr&& cbegin, CoefEndItr&& cend, ExpBeginItr&& ebegin,
             ExpEndItr&& eend, angular_momentum_type l, coord_type x,
             coord_type y, coord_type z) :
      CCAShell(std::forward<CoefBeginItr>(cbegin),
               std::forward<CoefEndItr>(cend),
               std::forward<ExpBeginItr>(ebegin), std::forward<ExpEndItr>(eend),
               l, center_type(std::move(x), std::move(y), std::move(z))) {}

    /** @brief Constructs a CCAShell from the provided parameters.
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
     *  @param[in] l The total angular momentum of the shell.
     *  @param[in] center Where *this is centered in Cartesian space.
     *
     *  @throw std::invalid_argument if the coefficient and exponent ranges do
     *                               not have the same length. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        contraction. Strong throw guarantee.
     */
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    CCAShell(CoefBeginItr&& cbegin, CoefEndItr&& cend, ExpBeginItr&& ebegin,
             ExpEndItr&& eend, angular_momentum_type l, center_type center) :
      m_cg_(std::forward<CoefBeginItr>(cbegin), std::forward<CoefEndItr>(cend),
            std::forward<ExpBeginItr>(ebegin), std::forward<ExpEndItr>(eend), l,
            std::move(center)) {}

    /** @brief Creates a deep copy of @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    CCAShell(const CCAShell& other) = default;

    /** @brief Takes ownership of @p other's state.
     *
     *  @throw None No throw guarantee.
     */
    CCAShell(CCAShell&& other) noexcept = default;

    /// Overwrites *this with a deep copy of @p other.
    CCAShell& operator=(const CCAShell& other) = default;

    /// Overwrites *this with @p other's state.
    CCAShell& operator=(CCAShell&& other) noexcept = default;

    /// Default, no-throw dtor
    ~CCAShell() noexcept override = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  Invalidates every AO reference previously obtained from either shell.
     *
     *  @throw None No throw guarantee.
     */
    void swap(CCAShell& other) noexcept {
        m_cg_.swap(other.m_cg_);
        this->invalidate_aos_();
        other.invalidate_aos_();
    }

    /** @brief Serializes *this into @p ar.
     *
     *  All of a shell's state is its contracted Gaussian, so this writes
     *  exactly what ContractedGaussian::save does. The purity is part of the
     *  type, so it is not written.
     *
     *  @tparam Archive The type of the cereal output archive.
     *
     *  @param[in,out] ar The archive to write to.
     *
     *  @throw std::runtime_error if the contraction can not be serialized.
     *                            Weak throw guarantee.
     */
    template<typename Archive>
    void save(Archive& ar) const {
        m_cg_.save(ar);
    }

    /** @brief Deserializes *this from @p ar.
     *
     *  Reads back what save wrote.
     *
     *  @tparam Archive The type of the cereal input archive.
     *
     *  @param[in,out] ar The archive to read from.
     *
     *  @throw std::runtime_error if the archive names a floating-point type
     *                            chemist does not know about or can not
     *                            deserialize. Weak throw guarantee.
     */
    template<typename Archive>
    void load(Archive& ar) {
        m_cg_.load(ar);
        this->invalidate_aos_();
    }

private:
    /** @brief Implements the CRTP base's state access.
     *
     *  The contracted Gaussian is handed out as a view aliasing m_cg_, so that
     *  the type a caller sees is the same one CCAShellView hands out.
     */
    ///@{
    contracted_gaussian_reference contracted_gaussian_() {
        return contracted_gaussian_reference(m_cg_);
    }
    const_contracted_gaussian_reference contracted_gaussian_() const {
        return const_contracted_gaussian_reference(m_cg_);
    }
    ///@}

    /** @brief Implements the AOShell interface in terms of the shared API.
     *
     *  clone_ and are_equal_ are not here; AOShellImpl implements those from
     *  *this's copy ctor and operator==.
     */
    ///@{
    typename AOShell::angular_momentum_type get_l_() const noexcept override {
        return common_type::get_l();
    }
    bool is_pure_() const noexcept override { return common_type::is_pure(); }
    typename AOShell::const_center_reference get_center_() const override {
        return common_type::get_center();
    }
    typename AOShell::const_contracted_gaussian_reference
    get_contracted_gaussian_() const override {
        return common_type::get_contracted_gaussian();
    }
    const_ao_view_reference at_(size_type i) const override {
        return common_type::at(i);
    }
    view_pointer as_view_() const override {
        return std::make_unique<const_view_type>(*this);
    }
    ///@}

    /// The contracted Gaussian every AO in *this is built on
    ContractedGaussian m_cg_;
};

/// Type of a Cartesian shell in CCA order
using cartesian_cca_shell = CCAShell<CartesianAO>;

/// Type of a pure shell in CCA order
using spherical_cca_shell = CCAShell<SphericalAO>;

} // namespace chemist::experimental

// CCAShell::as_view_ and get_cartesian_shell need CCAShellView to be complete
// by the time they are instantiated. CCAShellView's header needs *this one, so
// it can not be included at the top; including it here, after CCAShell is
// defined, means including either header gets both. See AOShellImpl for why
// as_view_ can not simply live somewhere else.
#include <chemist/experimental/basis_set/cca_shell_view.hpp>

namespace chemist::experimental {

extern template class CCAShell<CartesianAO>;
extern template class CCAShell<SphericalAO>;

} // namespace chemist::experimental
