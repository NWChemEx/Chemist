/*
 * Copyright 2024 NWChemEx-Project
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
#include "nuclei_view_pimpl.hpp"

namespace chemist::detail_ {

template<typename NucleiType>
class ContiguousNucleiView : public NucleiViewPIMPL<NucleiType> {
private:
    /// Type of *this
    using my_type = ContiguousNucleiView<NucleiType>;

    /// Type of the base class
    using base_type = NucleiViewPIMPL<NucleiType>;

public:
    /// Type *this aliases
    using typename base_type::nuclei_type;

    /// Type of a mutable reference to a Nucleus object
    using typename base_type::reference;

    /// Type of a read-only reference to a Nucleus object
    using typename base_type::const_reference;

    /// Class defining the types of the Charges part of *this
    using typename base_type::charges_traits;

    /// Class defining types for the Nucleus element in *this
    using typename base_type::nucleus_traits;

    /// Type acting like a mutable reference of a Charges object
    using charges_reference = typename charges_traits::view_type;

    /// Type of a pointer to a nucleus's name
    using name_pointer = typename nucleus_traits::name_pointer;

    /// Type of a pointer to a nucleus's atomic number
    using atomic_number_pointer =
      typename nucleus_traits::atomic_number_pointer;

    /// Type of a pointer to a nucleus's mass
    using mass_pointer = typename nucleus_traits::mass_pointer;

    /// Type used for indexing and offsets
    using typename base_type::size_type;

    /// Type of a pointer to the base of the PIMPL
    using typename base_type::pimpl_pointer;

    /** @brief Aliases an empty Nuclei object.
     *
     *  The default ctor will create a PIMPL which aliases an empty Nuclei
     *  object.
     *
     *  @throw None No throw guarantee.
     */
    ContiguousNucleiView() = default;

    /** @brief Wraps the state associated with the aliased Nuclei object.
     *
     *  This is the primary ctor for creating a stateful PIMPL. This ctor
     *  assumes that every point charge in @p charges is associated with a
     *  nuclei such that charges[i] is the i-th point charge piece of the i-th
     *  nucleus.
     *
     *  @param[in] charges A reference to the piece of the Nuclei class acting
     *                     as a Charges object.
     *  @param[in] pnames A pointer to the first nucleus's name.
     *  @param[in] patomic_numbers A pointer to the first nucleus's atomic
     *                             number.
     *  @param[in] pmasses A pointer to the first nucleus's mass.
     *
     *  @throw None No throw guarantee
     */
    ContiguousNucleiView(charges_reference charges, name_pointer pnames,
                         atomic_number_pointer patomic_numbers,
                         mass_pointer pmasses) :
      m_charges_(std::move(charges)),
      m_pnames_(pnames),
      m_patomic_numbers_(patomic_numbers),
      m_pmasses_(pmasses) {}

    /** @brief Creates a new PIMPL by copying the state of @p other.
     *
     *  It should be noted that this PIMPL deep copies the state in @p other,
     *  but only shallow copies the aliased Nuclei object. In other words,
     *  the resulting PIMPL aliases the same Nuclei object as @p other, but
     *  causing *this to point to a different Nuclei object will not affect
     *  @p other.
     *
     *  @param[in] other The view to copy.
     *
     *  @throw std::bad_alloc if copying the state fails. Strong throw
     *                        gurantee.
     */
    ContiguousNucleiView(const ContiguousNucleiView& other) = default;

    /// Implemented generically by NucleiView
    ContiguousNucleiView& operator=(const ContiguousNucleiView&) = delete;

    /// Implemented generically by NucleiView
    ContiguousNucleiView(ContiguousNucleiView&&) = delete;

    /// Implemented generically by NucleiView
    ContiguousNucleiView& operator=(ContiguousNucleiView&&) = delete;

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Determins if *this and @p rhs alias equivalent Nuclei objects.
     *
     *  This method compares the Nuclei object aliases by *this to the object
     *  aliased by @p rhs. This method does not exclusively compare whether
     *  *this and @p rhs alias the same memory (though it tries to exploit it
     *  for faster comparisons), but will actually compare the values too.
     *
     *  @param[in] rhs The view to compare against.
     *
     *  @return True if the Nuclei object aliased by *this is value equal to the
     *          Nuclei object aliased by @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ContiguousNucleiView& rhs) const noexcept;

protected:
    pimpl_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }

    reference get_nuke_(size_type i) override {
        return reference(m_pnames_[i], m_patomic_numbers_[i], m_pmasses_[i],
                         m_charges_[i]);
    }

    const_reference get_nuke_(size_type i) const override {
        return const_reference(m_pnames_[i], m_patomic_numbers_[i],
                               m_pmasses_[i], m_charges_[i]);
    }

    size_type size_() const noexcept { return m_charges_.size(); }

    bool are_equal_(const base_type& other) const noexcept override {
        return base_type::template are_equal_impl_<my_type>(other);
    }

private:
    /// The part of *this which acts like a Charges object
    charges_reference m_charges_;

    /// A pointer to the first Nucleus object's name
    name_pointer m_pnames_ = nullptr;

    /// A pointer to the first Nucleus object's atomic number
    atomic_number_pointer m_patomic_numbers_ = nullptr;

    /// A pointer to the first Nucleus object's mass
    mass_pointer m_pmasses_ = nullptr;
};

// -----------------------------------------------------------------------------
// -- Out of line definitions
// -----------------------------------------------------------------------------

template<typename NucleiType>
bool ContiguousNucleiView<NucleiType>::operator==(
  const ContiguousNucleiView& rhs) const noexcept {
    const auto n = size_();

    if(n != rhs.size_()) return false;
    if(n == 0) return true;

    // n.b. we now know both have the same non-zero number of nuclei

    // For each pointer in *this we first check if they are the same address
    // (which guarantees they are the same values). If they are not the same
    // address we need to actually compare the values.
    if(m_pnames_ != rhs.m_pnames_) {
        const auto* plhs = m_pnames_;
        const auto* prhs = rhs.m_pnames_;
        if(!std::equal(plhs, plhs + n, prhs)) return false;
    }

    if(m_patomic_numbers_ != rhs.m_patomic_numbers_) {
        const auto* plhs = m_patomic_numbers_;
        const auto* prhs = rhs.m_patomic_numbers_;
        if(!std::equal(plhs, plhs + n, prhs)) return false;
    }

    if(m_pmasses_ != rhs.m_pmasses_) {
        const auto* plhs = m_pmasses_;
        const auto* prhs = rhs.m_pmasses_;
        if(!std::equal(plhs, plhs + n, prhs)) return false;
    }

    return m_charges_ == rhs.m_charges_;
}

} // namespace chemist::detail_
