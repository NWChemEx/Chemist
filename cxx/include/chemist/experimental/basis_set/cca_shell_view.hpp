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
#include <chemist/experimental/basis_set/cca_shell_class.hpp>
#include <chemist/experimental/basis_set/detail_/ao_shell_view_impl.hpp>
#include <memory>
#include <type_traits>
#include <utility>

namespace chemist::experimental {

/** @brief An object which behaves like a CCAShell, but aliases the
 *         contracted Gaussian it is built on.
 *
 *  *this is the shell-level counterpart of CartesianAOView and
 *  ContractedGaussianView. Since all of a shell's state is its contracted
 *  Gaussian, whatever its purity, *this is exactly a ContractedGaussianView
 *  with the CCA ordering on top; unlike CartesianAOView there is no by-value
 *  state at all.
 *
 *  @tparam CCAShellType A, possibly const-qualified, CCAShell<AOType>. The AO
 *                       type decides the purity, exactly as for CCAShell.
 *                       CCAShellView<CCAShell<T>> aliases a mutable contracted
 *                       Gaussian and has setters;
 *                       CCAShellView<const CCAShell<T>> aliases a read-only
 *                       one and has none.
 */
template<typename CCAShellType>
class CCAShellView
  : public CCAShellCommon<CCAShellView<CCAShellType>, CCAShellType>,
    public detail_::AOShellViewImpl<CCAShellView<CCAShellType>> {
private:
    /// Type implementing the API shared with CCAShell
    using common_type =
      CCAShellCommon<CCAShellView<CCAShellType>, CCAShellType>;

    /// Type implementing the AOShellView interface in terms of *this
    using impl_type = detail_::AOShellViewImpl<CCAShellView<CCAShellType>>;

    /// Lets the CRTP base reach contracted_gaussian_()
    friend common_type;

    /// Lets the AOShellView implementation reach as_shell_value_()
    friend impl_type;

    /// Lets the other const-qualification of *this reach its members
    template<typename OtherCCAShellType>
    friend class CCAShellView;

    /// Enables the mutable-to-const converting ctor, and only that direction
    template<typename T>
    using enable_mutable_to_const_t =
      std::enable_if_t<std::is_const_v<CCAShellType> && !std::is_const_v<T>>;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename common_type::angular_index_type;
    using typename common_type::angular_momentum_type;
    using typename common_type::ao_type;
    using typename common_type::cartesian_powers_type;
    using typename common_type::center_type;
    using typename common_type::const_ao_pointer;
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

    /// Pull the AOShellView interface's types into *this's API
    ///@{
    using typename AOShellView::ao_view_pointer;
    using typename AOShellView::base_pointer;
    using typename AOShellView::const_base_reference;
    using typename AOShellView::shell_pointer;
    ///@}

    /// Type of the owning shell *this materializes into. AOShellViewImpl
    /// needs this to implement as_shell().
    using shell_type = typename ChemistClassTraits<CCAShellType>::value_type;

    /// Type of a, possibly read-only, reference to the aliased CCAShell
    using cca_shell_reference =
      typename ChemistClassTraits<CCAShellType>::reference;

    /** @brief Resolves the shared API against the AOShellView interface.
     *
     *  See the corresponding block in CCAShell.
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

    /** @brief Creates a view aliasing nothing.
     *
     *  @note Defaulted for symmetry with CartesianAOView, and, like that
     *        class's, in practice implicitly deleted; see
     *        CartesianAOView::CartesianAOView().
     */
    CCAShellView() noexcept = default;

    /** @brief Creates a view of @p shell.
     *
     *  This ctor is deliberately implicit so that a CCAShell can be passed
     *  wherever a CCAShellView is expected. The result aliases @p shell's
     *  contracted Gaussian; writing through *this writes into @p shell.
     *
     *  @param[in] shell The shell *this will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    CCAShellView(cca_shell_reference shell) :
      m_cg_(contracted_gaussian_reference(shell.m_cg_)) {}

    /** @brief Creates a view of a shell built on an already-aliased
     *         contracted Gaussian.
     *
     *  This is what lets *this alias a contracted Gaussian which is not owned
     *  by a CCAShell at all. Nothing needs checking: every contracted Gaussian
     *  is a valid shell.
     *
     *  @param[in] cg A view aliasing the contracted Gaussian *this is built on.
     *
     *  @throw None No throw guarantee.
     */
    explicit CCAShellView(contracted_gaussian_reference cg) noexcept :
      m_cg_(std::move(cg)) {}

    /** @brief Converts a mutable view into a read-only one.
     *
     *  This ctor is deliberately implicit, so that a mutable view can be
     *  passed wherever a read-only one is expected. The reverse direction is
     *  not provided.
     *
     *  @tparam OtherCCAShellType The shell type @p other models.
     *
     *  @param[in] other The mutable view to convert.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    template<typename OtherCCAShellType,
             typename = enable_mutable_to_const_t<OtherCCAShellType>>
    CCAShellView(const CCAShellView<OtherCCAShellType>& other) :
      m_cg_(other.m_cg_) {}

    /** @brief Creates a view aliasing the same contracted Gaussian as @p other.
     *
     *  The copy is shallow.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    CCAShellView(const CCAShellView& other) = default;

    /** @brief Takes the aliased state from @p other.
     *
     *  @throw None No throw guarantee.
     */
    CCAShellView(CCAShellView&& other) noexcept = default;

    /** @brief Overwrites the contracted Gaussian *this aliases with @p rhs's.
     *
     *  @note This does NOT make *this alias what @p rhs aliases. It writes
     *        @p rhs's contraction through into whatever *this is already
     *        aliasing. Use swap if you want to rebind *this.
     *
     *  This inherits ContractedGaussianView::operator='s contract: the number
     *  of primitives can not be changed through a view. @p rhs must have the
     *  same purity as *this, since the purity is part of the type of what
     *  *this aliases and so can not be written through.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherShell The CCA shell type @p rhs models.
     *
     *  @param[in] rhs The shell whose contraction should be copied into the
     *                 state *this aliases.
     *
     *  @return *this, after overwriting the aliased contraction.
     *
     *  @throw std::runtime_error if @p rhs's contraction does not hold the same
     *                            number of primitives as *this's, or if the
     *                            concrete floating-point types do not match.
     *                            Weak throw guarantee.
     */
    template<typename OtherDerived, typename OtherShell>
    CCAShellView& operator=(
      const CCAShellCommon<OtherDerived, OtherShell>& rhs) {
        static_assert(!std::is_const_v<CCAShellType>,
                      "Can not assign through a read-only CCAShellView.");
        static_assert(ChemistClassTraits<OtherShell>::is_pure ==
                        ChemistClassTraits<CCAShellType>::is_pure,
                      "Can not assign a shell of a different purity.");
        m_cg_ = rhs.get_contracted_gaussian();
        return *this;
    }

    /** @brief Overwrites the shell *this aliases with @p rhs's state.
     *
     *  See the templated assignment operator; this is the copy assignment
     *  operator and behaves identically.
     */
    CCAShellView& operator=(const CCAShellView& rhs) {
        static_assert(!std::is_const_v<CCAShellType>,
                      "Can not assign through a read-only CCAShellView.");
        if(this != &rhs) m_cg_ = rhs.get_contracted_gaussian();
        return *this;
    }

    /** @brief Overwrites the shell *this aliases with @p rhs's state.
     *
     *  This is identical to the copy assignment operator; there is nothing to
     *  steal, because *this does not own what it aliases.
     */
    CCAShellView& operator=(CCAShellView&& rhs) {
        static_assert(!std::is_const_v<CCAShellType>,
                      "Can not assign through a read-only CCAShellView.");
        if(this != &rhs) m_cg_ = rhs.get_contracted_gaussian();
        return *this;
    }

    /// Default, no-throw dtor. Does not affect the aliased state.
    ~CCAShellView() noexcept override = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a CCAShell holding a copy of the aliased state.
     *
     *  @return An owning shell, independent of what *this aliases.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    shell_type as_cca_shell() const {
        return shell_type(m_cg_.as_contracted_gaussian());
    }

    /** @brief Makes *this alias what @p other aliases, and vice versa.
     *
     *  Unlike assignment, this rebinds the views; it does not touch the
     *  aliased values.
     *
     *  @throw None No throw guarantee.
     */
    void swap(CCAShellView& other) noexcept { m_cg_.swap(other.m_cg_); }

private:
    /// Implements AOShellViewImpl::as_shell_
    shell_type as_shell_value_() const { return as_cca_shell(); }

    /** @brief Implements the CRTP base's state access.
     *
     *  Only a const-qualified overload is needed: whether the returned view is
     *  mutable is decided by CCAShellType, not by the const-qualification of
     *  *this. See CartesianAOView for the same choice.
     */
    contracted_gaussian_reference contracted_gaussian_() const { return m_cg_; }

    /** @brief Implements the AOShellView interface in terms of the shared API.
     *
     *  clone_, as_shell_, and are_equal_ are not here; AOShellViewImpl
     *  implements those from *this's copy ctor, as_shell_value_(), and
     *  operator==.
     */
    ///@{
    typename AOShellView::angular_momentum_type get_l_()
      const noexcept override {
        return common_type::get_l();
    }
    bool is_pure_() const noexcept override { return common_type::is_pure(); }
    typename AOShellView::const_center_reference get_center_() const override {
        return common_type::get_center();
    }
    typename AOShellView::const_contracted_gaussian_reference
    get_contracted_gaussian_() const override {
        return common_type::get_contracted_gaussian();
    }
    ao_view_pointer at_(size_type i) const override {
        return common_type::at(i);
    }
    ///@}

    /// Aliases the contracted Gaussian every AO in *this is built on
    contracted_gaussian_reference m_cg_;
};

/// Type of a view of a mutable Cartesian CCA shell
using cartesian_cca_shell_view = CCAShellView<CCAShell<CartesianAO>>;

/// Type of a view of a read-only Cartesian CCA shell
using const_cartesian_cca_shell_view =
  CCAShellView<const CCAShell<CartesianAO>>;

/// Type of a view of a mutable pure CCA shell
using spherical_cca_shell_view = CCAShellView<CCAShell<SphericalAO>>;

/// Type of a view of a read-only pure CCA shell
using const_spherical_cca_shell_view =
  CCAShellView<const CCAShell<SphericalAO>>;

/** @note Only the mutable instantiations are declared here, for the reason
 *        given on CartesianAOView's: explicitly instantiating a read-only one
 *        would force its (deliberately non-compiling) copy-assignment
 *        operator.
 */
///@{
extern template class CCAShellView<CCAShell<CartesianAO>>;
extern template class CCAShellView<CCAShell<SphericalAO>>;
///@}

} // namespace chemist::experimental
