/*
 * Copyright 2023 NWChemEx-Project
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

/** @brief Implements a NucleiView that is a subset of a Nuclei object.
 *
 *  This class is primarily envisioned as implementing NucleiView for
 *  ``FragmentedNuclei``. In this case, we have a supersystem which is an
 *  object of type ``Nuclei`` and the ``FragmentedNuclei`` is a container of
 * ``NucleiView`` objects which are views of the supersystem. The views can
 * be implemented quite efficiently by storing a view of the supersystem
 * and the indices of the nuclei in the view.
 *
 */
template<typename NucleiType>
class NucleiSubset : public NucleiViewPIMPL<NucleiType> {
private:
    /// Type of the base class
    using base_type = NucleiViewPIMPL<NucleiType>;

public:
    /// The type *this is implementing
    using typename base_type::parent_type;

    /// Type the view is acting as a reference of
    using nuclei_type = typename base_type::nuclei_type;

    /// Type of a pointer to the base PIMPL
    using pimpl_pointer = typename base_type::pimpl_pointer;

    /// Type of a Nucleus object
    using value_type = typename base_type::value_type;

    /// Type of a mutable reference to a Nucleus object
    using reference = typename base_type::reference;

    /// Type of a read-only reference to a Nucleus object
    using const_reference = typename base_type::const_reference;

    /// Type nuclei_type uses for indexing and offsets
    using size_type = typename nuclei_type::size_type;

    using member_list_type = typename parent_type::member_list_type;

    /// Makes a null subset
    NucleiSubset() noexcept = default;

    /** @brief Creates a view which is a subset of @p supersystem
     *
     *  This is the value ctor for the class. It will create a view which
     *  aliases a subset of @p supersystem.
     *
     *  @param[in] supersystm a view which aliases the supersystem.
     *  @param[in] members Which nuclei in @p supersystem should be included in
     *                     the subset? Values in @p nuclei should be in the
     *                     range [0, supersystem.size())
     *
     *  @throw None No throw guarantee.
     */
    NucleiSubset(parent_type supersystem, member_list_type members) :
      m_nuclei_(std::move(supersystem)), m_members_(std::move(members)) {}

    /** @brief Initializes *this to alias the same Nuclei as @p other.
     *
     *  The copy ctor deep copies the indices of @p other, and stores a
     *  copy of the reference to the supersystem. In turn, the resulting
     *  object aliases the same supersystem (which is consistent with *this
     *  being the implementation of a view).
     *
     *  @param[in] other The object we are copying.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state
     *                        for *this. Strong throw guarantee.
     */
    NucleiSubset(const NucleiSubset& other) = default;

    // Deleted to avoid accidental slicing, NucleiView implements them for user
    ///@{
    NucleiSubset(NucleiSubset&&)                     = delete;
    NucleiSubset& operator=(const NucleiSubset& rhs) = delete;
    NucleiSubset& operator=(NucleiSubset&& rhs)      = delete;
    ///@}

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  We define two NucleiSubset objects as being value equal if they both
     *  alias Nuclei objects which are value equal.
     *
     *  @param[in] rhs The NucleiSubset we are comparing to.
     *
     *  @return True if *this is value equal to @p rhs and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     *
     */
    bool operator==(const NucleiSubset& rhs) const noexcept;

protected:
    /// Implementes clone
    pimpl_pointer clone_() const override {
        return std::make_unique<NucleiSubset>(*this);
    }

    /// Implements getting a mutable Nucleus
    reference get_nuke_(size_type i) override {
        return m_nuclei_[m_members_[i]];
    }

    /// Implements getting a read-only Nucleus
    const_reference get_nuke_(size_type i) const override {
        return m_nuclei_[m_members_[i]];
    }

    /// Impelments size
    size_type size_() const noexcept override { return m_members_.size(); }

    /// Implements are_equal
    bool are_equal_(const base_type& rhs) const noexcept override {
        return base_type::template are_equal_impl_<NucleiSubset>(rhs);
    }

private:
    /// The full set of nuclei
    parent_type m_nuclei_;

    /// The indices in *this
    member_list_type m_members_;
};

// -- Implementations ----------------------------------------------------------

template<typename NucleiType>
inline bool NucleiSubset<NucleiType>::operator==(
  const NucleiSubset& rhs) const noexcept {
    if(this->size() != rhs.size()) return false;
    if(this->size() == 0) return true;

    // If supersystems are the same just compare the members arrays
    if(m_nuclei_ == rhs.m_nuclei_) { return m_members_ == rhs.m_members_; }

    // different supersystems means we need to compare the values one by one
    for(size_type i = 0; i < this->size(); ++i)
        if(this->get_nuke(i) != rhs.get_nuke(i)) return false;

    return true;
}

} // namespace chemist::detail_
