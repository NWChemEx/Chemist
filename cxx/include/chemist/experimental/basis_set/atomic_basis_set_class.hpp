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
#include <chemist/experimental/basis_set/atomic_basis_set_common.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <chemist/types/floating_point.hpp>
#include <memory>
#include <utility>

namespace chemist::experimental {

/** @brief Models the basis set of one atom by value.
 *
 *  See AtomicBasisSetCommon for the shared API and for how the shells are
 *  stored. *this owns that state. See AtomicBasisSetView for a class with
 *  the same API which aliases state owned by something else.
 *
 *  The type of the shells is chosen when *this is constructed, from a
 *  ShellPurity and an AOOrdering, and is fixed thereafter: every shell added
 *  to *this has that type.
 *
 *  A default-constructed instance holds no shells, has an empty name and an
 *  atomic number of 0, is centered at the origin, and holds Cartesian shells
 *  in CCA order.
 *
 *  @note A moved-from instance may only be assigned to, swapped, or
 *        destroyed.
 */
class AtomicBasisSet
  : public AtomicBasisSetCommon<AtomicBasisSet, AtomicBasisSet> {
private:
    /// Type *this inherits from
    using base_type = AtomicBasisSetCommon<AtomicBasisSet, AtomicBasisSet>;

    /// Lets the CRTP bases reach pimpl_()
    template<typename OtherDerived, typename OtherType>
    friend class AtomicBasisSetCommon;

    /// Lets a view alias *this's state
    template<typename AtomicBasisSetType>
    friend class AtomicBasisSetView;

    /// The concrete types add_shell may store; see PointSet::fp_types
    using fp_types = chemist::types::floating_point_types;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::angular_momentum_type;
    using typename base_type::atomic_number_type;
    using typename base_type::buffer_type;
    using typename base_type::center_type;
    using typename base_type::const_buffer_reference;
    using typename base_type::name_type;
    using typename base_type::pimpl_pointer;
    using typename base_type::pimpl_type;
    using typename base_type::size_type;
    ///@}

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates an empty set of Cartesian CCA shells.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    AtomicBasisSet();

    /** @brief Creates an empty set.
     *
     *  @param[in] name The name of the basis set, e.g. "cc-pVDZ".
     *  @param[in] atomic_number The atomic number of the atom *this is for.
     *  @param[in] center The point every shell will be centered on.
     *  @param[in] purity The purity of every shell *this will hold.
     *  @param[in] ordering The AO ordering of every shell *this will hold.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    AtomicBasisSet(name_type name, atomic_number_type atomic_number,
                   center_type center,
                   ShellPurity purity  = ShellPurity::cartesian,
                   AOOrdering ordering = AOOrdering::cca);

    /** @brief Creates a deep copy of @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    AtomicBasisSet(const AtomicBasisSet& other);

    /** @brief Takes ownership of @p other's state.
     *
     *  @throw None No throw guarantee.
     */
    AtomicBasisSet(AtomicBasisSet&& other) noexcept;

    /** @brief Overwrites *this with a deep copy of @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    AtomicBasisSet& operator=(const AtomicBasisSet& other);

    /** @brief Overwrites *this with @p other's state.
     *
     *  @throw None No throw guarantee.
     */
    AtomicBasisSet& operator=(AtomicBasisSet&& other) noexcept;

    /// Default, no-throw dtor
    ~AtomicBasisSet() noexcept;

    // -------------------------------------------------------------------------
    // -- Adding shells
    // -------------------------------------------------------------------------

    /** @brief Adds a shell built from the provided parameters.
     *
     *  The shell is centered on the center of *this, and has the type of the
     *  shells in *this.
     *
     *  @warning This reallocates the state of *this and therefore invalidates
     *           every shell, primitive, and buffer view previously obtained
     *           from *this.
     *
     *  @param[in] l The total angular momentum of the shell.
     *  @param[in] cbegin Iterator to the beginning of the container holding
     *                    the coefficients.
     *  @param[in] cend Iterator to the end of the container holding the
     *                  coefficients.
     *  @param[in] ebegin Iterator to the beginning of the container holding
     *                    the exponents.
     *  @param[in] eend Iterator to the end of the container holding the
     *                  exponents.
     *
     *  @throw std::invalid_argument if the coefficient and exponent ranges do
     *                               not have the same length. Strong throw
     *                               guarantee.
     *  @throw std::runtime_error if the parameters are not of the same
     *                            concrete floating-point type as the ones
     *                            already in *this. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    void add_shell(angular_momentum_type l, CoefBeginItr&& cbegin,
                   CoefEndItr&& cend, ExpBeginItr&& ebegin, ExpEndItr&& eend) {
        buffer_type cs, es;
        for(auto itr = cbegin; itr != cend; ++itr)
            cs.template push_back<fp_types>(*itr);
        for(auto itr = ebegin; itr != eend; ++itr)
            es.template push_back<fp_types>(*itr);
        m_pimpl_->add_shell(l, cs.as_view(), es.as_view());
    }

    /** @brief Adds a copy of @p shell to *this.
     *
     *  Only the angular momentum and the primitives of @p shell are copied
     *  into *this; *this does not alias @p shell afterwards.
     *
     *  @warning This invalidates the same things as add_shell.
     *
     *  @param[in] shell The shell to add. It must have the type of the shells
     *                   in *this and be centered on the center of *this.
     *
     *  @throw std::invalid_argument if @p shell's purity, AO ordering, or
     *                               center differs from that of *this. Strong
     *                               throw guarantee.
     *  @throw std::runtime_error under the same conditions as add_shell.
     *                            Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the new state.
     *                        Strong throw guarantee.
     */
    ///@{
    void push_back(const AOShellView& shell) { m_pimpl_->push_back(shell); }
    void push_back(const AOShell& shell) { push_back(*shell.as_view()); }
    ///@}

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @throw None No throw guarantee.
     */
    void swap(AtomicBasisSet& other) noexcept;

    /** @brief Deserializes *this from @p ar.
     *
     *  Reads back what AtomicBasisSetCommon::save wrote, replacing whatever
     *  *this held beforehand.
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
        int purity{}, ordering{};
        ar(purity, ordering);
        name_type name;
        atomic_number_type z{};
        ar(name, z);
        center_type center;
        center.load(ar);
        AtomicBasisSet buffer(std::move(name), z, std::move(center),
                              static_cast<ShellPurity>(purity),
                              static_cast<AOOrdering>(ordering));
        size_type n_shells{};
        ar(n_shells);
        for(size_type i = 0; i < n_shells; ++i) {
            angular_momentum_type l{};
            size_type n_prims{};
            ar(l, n_prims);
            buffer_type cs, es;
            for(size_type j = 0; j < n_prims; ++j) {
                cs.template push_back<fp_types>(detail_::load_float(ar));
                es.template push_back<fp_types>(detail_::load_float(ar));
            }
            buffer.m_pimpl_->add_shell(l, cs.as_view(), es.as_view());
        }
        swap(buffer);
    }

private:
    /// Implements the CRTP base's access to the implementation
    ///@{
    pimpl_type& pimpl_() noexcept { return *m_pimpl_; }
    const pimpl_type& pimpl_() const noexcept { return *m_pimpl_; }
    ///@}

    /// The object implementing *this
    pimpl_pointer m_pimpl_;
};

} // namespace chemist::experimental
