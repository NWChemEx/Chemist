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
#include <chemist/experimental/basis_set/cartesian_ao_class.hpp>
#include <chemist/experimental/basis_set/detail_/ao_view_impl.hpp>
#include <stdexcept>
#include <utility>
#include <vector>

namespace chemist::experimental {

/** @brief An object which behaves like a CartesianAO, but aliases the
 *         contracted Gaussian it is built on.
 *
 *  *this is the Cartesian-AO-level counterpart of ContractedGaussianView and
 *  PrimitiveView, and exists for the same reason: to give a non-owning handle
 *  the exact same API as the owning class. It is also what makes a shell of
 *  AOs possible: every AO in a shell shares one contracted Gaussian, and
 *  indexing that shell hands out views of it rather than
 *  @f$(\ell+1)(\ell+2)/2@f$ copies which could drift apart.
 *
 *  Note that the Cartesian powers are stored by value here, while the
 *  contracted Gaussian is aliased. The asymmetry is deliberate. The powers are
 *  what selects one AO out of the shared radial part, so a shell materializes
 *  them per offset from its own ordering rather than storing them anywhere a
 *  view could alias, and a shell hands out only
 *  CartesianAOView<const CartesianAO>, so there is no case in this design of
 *  writing new powers through a shell-issued view and needing that write
 *  observed elsewhere. Should aliased powers ever be needed, the way to add
 *  them is a PIMPL holding them, not a change to this class's layout.
 *
 *  @tparam CAOType A, possibly const-qualified, CartesianAO.
 *                  CartesianAOView<CartesianAO> aliases a mutable contracted
 *                  Gaussian and has setters;
 *                  CartesianAOView<const CartesianAO> aliases a read-only one
 *                  and has none.
 */
template<typename CAOType>
class CartesianAOView
  : public CartesianAOCommon<CartesianAOView<CAOType>, CAOType>,
    public detail_::AOViewImpl<CartesianAOView<CAOType>> {
private:
    /// Type implementing the API shared with CartesianAO
    using common_type = CartesianAOCommon<CartesianAOView<CAOType>, CAOType>;

    /// Type implementing the AOView interface in terms of *this
    using impl_type = detail_::AOViewImpl<CartesianAOView<CAOType>>;

    /// Lets the CRTP base reach contracted_gaussian_(), i_(), j_(), and k_()
    friend common_type;

    /// Lets the AOView implementation reach as_ao_value_()
    friend impl_type;

    /// Lets the other const-qualification of *this reach its members
    template<typename OtherCAOType>
    friend class CartesianAOView;

    /// Enables the mutable-to-const converting ctor, and only that direction
    template<typename T>
    using enable_mutable_to_const_t =
      std::enable_if_t<std::is_const_v<CAOType> && !std::is_const_v<T>>;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename common_type::angular_index_type;
    using typename common_type::angular_momentum_type;
    using typename common_type::center_type;
    using typename common_type::const_center_reference;
    using typename common_type::const_contracted_gaussian_reference;
    using typename common_type::contracted_gaussian_reference;
    using typename common_type::contracted_gaussian_type;
    using typename common_type::coord_type;
    using typename common_type::numerical_value;
    using typename common_type::numerical_vector;
    using typename common_type::size_type;
    using typename common_type::value_type;
    ///@}

    /// Pull the AOView interface's types into *this's API
    ///@{
    using typename AOView::ao_pointer;
    using typename AOView::base_pointer;
    using typename AOView::const_base_reference;
    ///@}

    /// Type of the owning Cartesian AO *this materializes into. AOViewImpl
    /// needs this to implement as_ao().
    using ao_type = typename ChemistClassTraits<CAOType>::value_type;

    /// Type of a, possibly read-only, reference to the aliased CartesianAO
    using cartesian_ao_reference =
      typename ChemistClassTraits<CAOType>::reference;

    /** @brief Resolves the shared API against the AOView interface.
     *
     *  See the corresponding block in CartesianAO for why these are needed:
     *  *this inherits each name twice, from CartesianAOCommon and from AOView,
     *  and these declarations pick the non-virtual implementations for a caller
     *  who holds the derived type.
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

    /** @brief Creates a view aliasing nothing.
     *
     *  @note This ctor is defaulted for symmetry with ContractedGaussianView,
     *        and, exactly like that class's, is in practice implicitly deleted:
     *        the aliasing types it would have to default-construct (ultimately
     *        a PointView) have no default ctor of their own, because a view
     *        aliasing nothing has nothing to point at. It is declared so that
     *        *this gains the ctor if they ever do.
     */
    CartesianAOView() noexcept = default;

    /** @brief Creates a view of @p cao.
     *
     *  This ctor is deliberately implicit so that a CartesianAO can be passed
     *  wherever a CartesianAOView is expected. The result aliases @p cao's
     *  contracted Gaussian; it does not copy it, so writing through *this
     *  writes into @p cao.
     *
     *  @param[in] cao The Cartesian AO *this will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    CartesianAOView(cartesian_ao_reference cao) :
      m_cg_(contracted_gaussian_reference(cao.m_cg_)),
      m_i_(cao.m_i_),
      m_j_(cao.m_j_),
      m_k_(cao.m_k_) {}

    /** @brief Creates a view of an already-aliased contracted Gaussian.
     *
     *  This ctor is what lets *this alias a contracted Gaussian which is not
     *  owned by a CartesianAO at all, which is how a shell hands out its AOs.
     *
     *  The argument order mirrors CartesianAO's ctor, with @p cg standing in
     *  for the coefficients, exponents, and center that ctor builds its own
     *  contraction from. Unlike that ctor, this one does have something to
     *  check: @p cg was built by someone else and already has an angular
     *  momentum, which the powers given here have to agree with.
     *
     *  @param[in] cg A view aliasing the contracted Gaussian *this is built on.
     *  @param[in] i The power of @f$x@f$ in the monomial.
     *  @param[in] j The power of @f$y@f$ in the monomial.
     *  @param[in] k The power of @f$z@f$ in the monomial.
     *
     *  @throw std::invalid_argument if @p i + @p j + @p k is not @p cg's total
     *                               angular momentum. Strong throw guarantee.
     */
    CartesianAOView(contracted_gaussian_reference cg, angular_index_type i,
                    angular_index_type j, angular_index_type k) :
      m_cg_(std::move(cg)), m_i_(i), m_j_(j), m_k_(k) {
        if(i + j + k == m_cg_.get_l()) return;
        throw std::invalid_argument(
          "chemist::experimental::CartesianAOView: i + j + k must equal the "
          "contracted Gaussian's total angular momentum.");
    }

    /** @brief Converts a mutable view into a read-only one.
     *
     *  This ctor is deliberately implicit, so that a mutable view can be
     *  passed wherever a read-only one is expected. The reverse direction is
     *  not provided.
     *
     *  @tparam OtherCAOType The Cartesian AO type @p other models.
     *
     *  @param[in] other The mutable view to convert.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    template<typename OtherCAOType,
             typename = enable_mutable_to_const_t<OtherCAOType>>
    CartesianAOView(const CartesianAOView<OtherCAOType>& other) :
      m_cg_(other.m_cg_),
      m_i_(other.m_i_),
      m_j_(other.m_j_),
      m_k_(other.m_k_) {}

    /** @brief Creates a view aliasing the same contracted Gaussian as @p other.
     *
     *  The copy is shallow: the resulting view aliases the same contracted
     *  Gaussian @p other does.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    CartesianAOView(const CartesianAOView& other) = default;

    /** @brief Takes the aliased state from @p other.
     *
     *  @throw None No throw guarantee.
     */
    CartesianAOView(CartesianAOView&& other) noexcept = default;

    /** @brief Overwrites the contracted Gaussian *this aliases with @p rhs's.
     *
     *  @note This does NOT make *this alias what @p rhs aliases. It writes
     *        @p rhs's contraction through into whatever *this is already
     *        aliasing. Use swap if you want to rebind *this.
     *
     *  Only the contracted Gaussian is written, because only the contracted
     *  Gaussian is aliased: the Cartesian powers belong to *this (see the note
     *  on *this). Assigning a Cartesian AO with different powers therefore
     *  throws rather than writing them, because doing so would leave the
     *  Cartesian AO *this aliases disagreeing with *this. This is the same
     *  contract ContractedGaussianView::operator= has for the one piece of its
     *  state which is likewise not writable through an alias, the number of
     *  primitives.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherCAO The Cartesian AO type @p rhs models.
     *
     *  @param[in] rhs The Cartesian AO whose contraction should be copied into
     *                 the state *this aliases.
     *
     *  @return *this, after overwriting the aliased contraction.
     *
     *  @throw std::runtime_error if @p rhs's powers differ from *this's, if its
     *                            contraction does not hold the same number of
     *                            primitives as *this's, or if the concrete
     *                            floating-point types do not match. Weak throw
     *                            guarantee.
     */
    template<typename OtherDerived, typename OtherCAO>
    CartesianAOView& operator=(
      const CartesianAOCommon<OtherDerived, OtherCAO>& rhs) {
        static_assert(!std::is_const_v<CAOType>,
                      "Can not assign through a read-only CartesianAOView.");
        assign_(rhs);
        return *this;
    }

    /** @brief Overwrites the Cartesian AO *this aliases with @p rhs's state.
     *
     *  See the templated assignment operator; this is the copy assignment
     *  operator and behaves identically.
     */
    CartesianAOView& operator=(const CartesianAOView& rhs) {
        static_assert(!std::is_const_v<CAOType>,
                      "Can not assign through a read-only CartesianAOView.");
        if(this != &rhs) assign_(rhs);
        return *this;
    }

    /** @brief Overwrites the Cartesian AO *this aliases with @p rhs's state.
     *
     *  This is identical to the copy assignment operator; there is nothing to
     *  steal, because *this does not own what it aliases.
     */
    CartesianAOView& operator=(CartesianAOView&& rhs) {
        static_assert(!std::is_const_v<CAOType>,
                      "Can not assign through a read-only CartesianAOView.");
        if(this != &rhs) assign_(rhs);
        return *this;
    }

    /// Default, no-throw dtor. Does not affect the aliased state.
    ~CartesianAOView() noexcept override = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a CartesianAO holding a copy of the aliased state.
     *
     *  @return An owning Cartesian AO, independent of what *this aliases.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    ao_type as_cartesian_ao() const {
        const auto n = m_cg_.size();
        std::vector<typename ao_type::coord_type> cs, es;
        cs.reserve(n);
        es.reserve(n);
        for(size_type p = 0; p < n; ++p) {
            cs.push_back(detail_::copy(m_cg_[p].get_coefficient()));
            es.push_back(detail_::copy(m_cg_[p].get_exponent()));
        }
        return ao_type(cs.begin(), cs.end(), es.begin(), es.end(), m_i_, m_j_,
                       m_k_, m_cg_.get_center().as_point());
    }

    /** @brief Makes *this alias what @p other aliases, and vice versa.
     *
     *  Unlike assignment, this rebinds the views; it does not touch the
     *  aliased values.
     *
     *  @throw None No throw guarantee.
     */
    void swap(CartesianAOView& other) noexcept {
        m_cg_.swap(other.m_cg_);
        std::swap(m_i_, other.m_i_);
        std::swap(m_j_, other.m_j_);
        std::swap(m_k_, other.m_k_);
    }

private:
    /// Implements every assignment operator
    template<typename OtherDerived, typename OtherCAO>
    void assign_(const CartesianAOCommon<OtherDerived, OtherCAO>& rhs) {
        if(m_i_ != rhs.get_i() || m_j_ != rhs.get_j() || m_k_ != rhs.get_k())
            throw std::runtime_error(
              "chemist::experimental::CartesianAOView: can not assign a "
              "Cartesian AO with different powers through a view. The powers "
              "are not aliased, so writing them would leave the Cartesian AO "
              "*this aliases disagreeing with *this.");
        m_cg_ = rhs.get_contracted_gaussian();
    }

    /// Implements AOViewImpl::as_ao_
    ao_type as_ao_value_() const { return as_cartesian_ao(); }

    /** @brief Implements the CRTP base's state access.
     *
     *  Unlike CartesianAO, *this only needs a single, const-qualified overload
     *  of the contracted-Gaussian hook: whether the returned view is itself
     *  mutable is decided by CAOType, via the reference types, not by the
     *  const-qualification of *this. The power hooks still need both, because
     *  the powers are owned by *this rather than aliased.
     */
    ///@{
    contracted_gaussian_reference contracted_gaussian_() const { return m_cg_; }
    angular_index_type& i_() noexcept { return m_i_; }
    angular_index_type i_() const noexcept { return m_i_; }
    angular_index_type& j_() noexcept { return m_j_; }
    angular_index_type j_() const noexcept { return m_j_; }
    angular_index_type& k_() noexcept { return m_k_; }
    angular_index_type k_() const noexcept { return m_k_; }
    ///@}

    /** @brief Implements the AOView interface in terms of the shared API.
     *
     *  clone_, as_ao_, and are_equal_ are not here; AOViewImpl implements
     *  those from *this's copy ctor, as_ao_value_(), and operator==.
     */
    ///@{
    typename AOView::angular_momentum_type get_l_() const noexcept override {
        return common_type::get_l();
    }
    typename AOView::const_center_reference get_center_() const override {
        return common_type::get_center();
    }
    typename AOView::const_contracted_gaussian_reference
    get_contracted_gaussian_() const override {
        return common_type::get_contracted_gaussian();
    }
    typename AOView::numerical_value normalization_constant_() const override {
        return common_type::normalization_constant();
    }
    typename AOView::numerical_value evaluate_(
      typename AOView::const_point_reference r) const override {
        return common_type::evaluate(r);
    }
    typename AOView::numerical_value normalized_evaluate_(
      typename AOView::const_point_reference r) const override {
        return common_type::normalized_evaluate(r);
    }
    ///@}

    /// Aliases the contracted Gaussian *this pairs its monomial with
    contracted_gaussian_reference m_cg_;

    /// The powers of x, y, and z in the monomial of *this. Owned, not aliased;
    /// see the note on *this for why.
    ///@{
    angular_index_type m_i_ = 0;
    angular_index_type m_j_ = 0;
    angular_index_type m_k_ = 0;
    ///@}
};

/// Type of a view of a mutable CartesianAO
using cartesian_ao_view = CartesianAOView<CartesianAO>;

/// Type of a view of a read-only CartesianAO
using const_cartesian_ao_view = CartesianAOView<const CartesianAO>;

/** @note Only the mutable instantiation is declared here, for the same reason
 *        ContractedGaussianView<const ContractedGaussian> is not: explicitly
 *        instantiating CartesianAOView<const CartesianAO> would force the
 *        compiler to instantiate its copy-assignment operator, which
 *        deliberately does not compile for a read-only view.
 */
extern template class CartesianAOView<CartesianAO>;

} // namespace chemist::experimental
