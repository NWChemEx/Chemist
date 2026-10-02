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
#include <chemist/experimental/basis_set/atomic_basis_set_class.hpp>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace chemist::experimental {

/** @brief An object which behaves like an AtomicBasisSet, but aliases its
 *         state.
 *
 *  *this is the atomic-basis-set counterpart of CCAShellView and
 *  ContractedGaussianView: it has the same API as the owning class, but
 *  writing through it writes into whatever owns the state. Like those views,
 *  *this can not change the shape of what it aliases, so it has no
 *  add_shell or push_back, and assignment writes values through rather than
 *  rebinding.
 *
 *  Internally *this holds an implementation which aliases each piece of the
 *  state separately, so the state need not be owned by an AtomicBasisSet;
 *  it may, e.g., be a slice of the state of a larger container.
 *
 *  @tparam AtomicBasisSetType A, possibly const-qualified, AtomicBasisSet.
 *                             AtomicBasisSetView<AtomicBasisSet> aliases
 *                             mutable state and has setters;
 *                             AtomicBasisSetView<const AtomicBasisSet>
 *                             aliases read-only state and has none.
 */
template<typename AtomicBasisSetType>
class AtomicBasisSetView
  : public AtomicBasisSetCommon<AtomicBasisSetView<AtomicBasisSetType>,
                                AtomicBasisSetType> {
private:
    /// Type *this inherits from
    using base_type =
      AtomicBasisSetCommon<AtomicBasisSetView<AtomicBasisSetType>,
                           AtomicBasisSetType>;

    /// Lets the CRTP bases reach pimpl_()
    template<typename OtherDerived, typename OtherType>
    friend class AtomicBasisSetCommon;

    /// Lets the other const-qualification of *this reach its members
    template<typename OtherType>
    friend class AtomicBasisSetView;

    /// Is the aliased state read-only?
    static constexpr bool is_const = std::is_const_v<AtomicBasisSetType>;

    /// Enables the mutable-to-const converting ctor, and only that direction
    template<typename T>
    using enable_mutable_to_const_t =
      std::enable_if_t<is_const && !std::is_const_v<T>>;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::pimpl_pointer;
    using typename base_type::pimpl_type;
    using typename base_type::size_type;
    ///@}

    /// Type of the owning set *this materializes into
    using atomic_basis_set_type = AtomicBasisSet;

    /// Type of a, possibly read-only, reference to the aliased set
    using atomic_basis_set_reference =
      typename ChemistClassTraits<AtomicBasisSetType>::reference;

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a view of @p abs.
     *
     *  This ctor is deliberately implicit so that an AtomicBasisSet can be
     *  passed wherever an AtomicBasisSetView is expected. The result aliases
     *  @p abs's state; writing through *this writes into @p abs. It is
     *  invalidated by anything which reallocates that state, e.g. adding a
     *  shell to @p abs.
     *
     *  @param[in] abs The set *this will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    AtomicBasisSetView(atomic_basis_set_reference abs) :
      m_pimpl_(alias_(abs.pimpl_())) {}

    /** @brief Creates a view from an implementation aliasing the state.
     *
     *  This ctor is what lets *this alias state which is not owned by an
     *  AtomicBasisSet at all. @p pimpl must alias, not own, its state, and
     *  must alias read-only state if *this is read-only; that is not checked.
     *
     *  @param[in] pimpl The implementation *this will hold.
     *
     *  @throw std::invalid_argument if @p pimpl is null. Strong throw
     *                               guarantee.
     */
    explicit AtomicBasisSetView(pimpl_pointer pimpl) :
      m_pimpl_(std::move(pimpl)) {
        if(m_pimpl_) return;
        throw std::invalid_argument(
          "chemist::experimental::AtomicBasisSetView: the implementation "
          "must not be null.");
    }

    /** @brief Converts a mutable view into a read-only one.
     *
     *  This ctor is deliberately implicit, so that a mutable view can be
     *  passed wherever a read-only one is expected. The reverse direction is
     *  not provided.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    template<typename OtherType,
             typename = enable_mutable_to_const_t<OtherType>>
    AtomicBasisSetView(const AtomicBasisSetView<OtherType>& other) :
      m_pimpl_(other.m_pimpl_->as_const_view()) {}

    /** @brief Creates a view aliasing the same state as @p other.
     *
     *  The copy is shallow.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    AtomicBasisSetView(const AtomicBasisSetView& other) :
      m_pimpl_(other.m_pimpl_->clone()) {}

    /** @brief Takes the aliased state from @p other.
     *
     *  @note The moved-from view may only be assigned to, swapped, or
     *        destroyed.
     *
     *  @throw None No throw guarantee.
     */
    AtomicBasisSetView(AtomicBasisSetView&& other) noexcept = default;

    /** @brief Overwrites the state *this aliases with @p rhs's.
     *
     *  @note This does NOT make *this alias what @p rhs aliases. It writes
     *        @p rhs's values through into whatever *this is already
     *        aliasing. Use swap if you want to rebind *this.
     *
     *  The shape of the aliased state can not be changed through a view, so
     *  @p rhs must hold shells of the same type, the same number of shells,
     *  and the same number of primitives in each shell as *this. Its name,
     *  atomic number, center, angular momenta, and parameters are then
     *  written into the state *this aliases.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherType The atomic-basis-set type @p rhs models.
     *
     *  @param[in] rhs The set whose values should be copied into the state
     *                 *this aliases.
     *
     *  @return *this, after overwriting the aliased state.
     *
     *  @throw std::runtime_error if the shape of @p rhs differs from that of
     *                            *this (strong throw guarantee), or if the
     *                            concrete floating-point types of the
     *                            parameters differ (weak throw guarantee).
     */
    template<typename OtherDerived, typename OtherType>
    AtomicBasisSetView& operator=(
      const AtomicBasisSetCommon<OtherDerived, OtherType>& rhs) {
        static_assert(!is_const,
                      "Can not assign through a read-only AtomicBasisSetView.");
        this->assign_(rhs);
        return *this;
    }

    /** @brief Overwrites the state *this aliases with @p rhs's.
     *
     *  See the templated assignment operator; this is the copy assignment
     *  operator and behaves identically.
     */
    AtomicBasisSetView& operator=(const AtomicBasisSetView& rhs) {
        static_assert(!is_const,
                      "Can not assign through a read-only AtomicBasisSetView.");
        if(this != &rhs) this->assign_(rhs);
        return *this;
    }

    /** @brief Overwrites the state *this aliases with @p rhs's.
     *
     *  This is identical to the copy assignment operator; there is nothing to
     *  steal, because *this does not own what it aliases.
     */
    AtomicBasisSetView& operator=(AtomicBasisSetView&& rhs) {
        static_assert(!is_const,
                      "Can not assign through a read-only AtomicBasisSetView.");
        if(this != &rhs) this->assign_(rhs);
        return *this;
    }

    /// Default, no-throw dtor. Does not affect the aliased state.
    ~AtomicBasisSetView() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns an AtomicBasisSet holding a copy of the aliased state.
     *
     *  @return An owning set, independent of what *this aliases.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    atomic_basis_set_type as_atomic_basis_set() const {
        atomic_basis_set_type rv(this->get_name(), this->get_atomic_number(),
                                 this->get_center().as_point(), this->purity(),
                                 this->ordering());
        for(size_type i = 0; i < this->size(); ++i) rv.push_back(*this->at(i));
        return rv;
    }

    /** @brief Makes *this alias what @p other aliases, and vice versa.
     *
     *  Unlike assignment, this rebinds the views; it does not touch the
     *  aliased values.
     *
     *  @throw None No throw guarantee.
     */
    void swap(AtomicBasisSetView& other) noexcept {
        m_pimpl_.swap(other.m_pimpl_);
    }

private:
    /// Makes an implementation aliasing the state @p pimpl implements
    ///@{
    static pimpl_pointer alias_(pimpl_type& pimpl) {
        if constexpr(is_const) {
            return std::as_const(pimpl).as_const_view();
        } else {
            return pimpl.as_view();
        }
    }
    static pimpl_pointer alias_(const pimpl_type& pimpl) {
        return pimpl.as_const_view();
    }
    ///@}

    /// Implements the CRTP base's access to the implementation
    ///@{
    pimpl_type& pimpl_() noexcept { return *m_pimpl_; }
    const pimpl_type& pimpl_() const noexcept { return *m_pimpl_; }
    ///@}

    /// The object aliasing the state
    pimpl_pointer m_pimpl_;
};

/// Type of a view of a mutable AtomicBasisSet
using atomic_basis_set_view = AtomicBasisSetView<AtomicBasisSet>;

/// Type of a view of a read-only AtomicBasisSet
using const_atomic_basis_set_view = AtomicBasisSetView<const AtomicBasisSet>;

/** @note Only the mutable instantiation is declared here, for the reason
 *        given on ContractedGaussianView's: explicitly instantiating the
 *        read-only one would force its (deliberately non-compiling)
 *        copy-assignment operator.
 */
extern template class AtomicBasisSetView<AtomicBasisSet>;

} // namespace chemist::experimental
