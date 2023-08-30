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
 * be implemented quite efficiently by storing a pointer to the supersystem
 * and the indices of the nuclei in the view.
 *
 *  @note we have declared the types in a manner which will work if the class
 *        needs to be templated down the line. Without templating we do not
 *        technically need to pull in the base class's types, nor do we need
 *        typename.
 */
template<typename NucleiType>
class NucleiSubset : public NucleiViewPIMPL<NucleiType> {
private:
    /// Type of the base class
    using base_type = NucleiViewPIMPL<NucleiType>;

public:
    /// Type the view is acting as a reference of
    using nuclei_type = typename base_type::nuclei_type;

    /// Type of a shared pointer to a nuclei_type object
    using nuclei_pointer = std::shared_ptr<nuclei_type>;

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

    /// Makes a null subset
    NucleiSubset() noexcept = default;

    /** @brief Creates a NucleiViewPIMPL which holds a subset of a Nuclei object
     *
     *  @tparam BeginItr The type of the iterator pointing to the index of the
     *                   first nucleus.
     *  @tparam EndItr The type of the iterator pointing to just past the index
     *                 of the last nucleus.
     *
     *  @param[in] supersystem The *this will be a subset of.
     *  @param[in] begin An iterator which points to the index of the first
     *                   nucleus.
     *  @param[in] end An iterator which points to just past the index of the
     *                 last nucleus.
     *
     * @throw std::bad_alloc if there is a problem allocating the memory. Strong
     *                       throw guarantee.
     *
     */
    template<typename BeginItr, typename EndItr>
    NucleiSubset(nuclei_pointer supersystem, BeginItr&& begin, EndItr&& end) :
      m_nukes_(supersystem),
      m_members_(std::forward<BeginItr>(begin), std::forward<EndItr>(end)) {}

    /** @brief Initializes *this to a deep copy of @p other.
     *
     *  The copy ctor deep copies the indices of @p other, and stores a
     *  copy of the pointer pointing to the supersystem. In turn, the resulting
     *  object aliases the same supersystem (which is consistent with *this
     *  bein the implementaiton of a view).
     *
     *  @param[in] other The object we are copying.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new state
     *                        for *this. Strong throw guarantee.
     */
    NucleiSubset(const NucleiSubset& other) = default;

    // Deleted to avoid accidental slicing
    ///@{
    NucleiSubset(NucleiSubset&&) = delete;
    NucleiSubset& operator=(const NucleiSubset& rhs) = delete;
    NucleiSubset& operator=(NucleiSubset&& rhs) = delete;
    ///@}

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  We define two NucleiSubset objects as being value equal if they both
     *  are null, or if they both are the same subset of the same supersystem.
     *  Supersystems are comapred by value, meaning they do NOT have to be the
     *  same instance.
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

    /** @brief Does *this contain state?
     *
     *  For a NucleiSubset object to be null it has to not be associated with
     *  a supersystem (in which case it also does not have any nuclei).
     *
     *  @return True if *this is null and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool is_null() const noexcept { return !static_cast<bool>(m_nukes_); }

protected:
    /// Implementes clone
    pimpl_pointer clone_() const override {
        return std::make_unique<NucleiSubset>(*this);
    }

    /// Implements getting a mutable Nucleus
    reference get_nuke_(size_type i) override {
        return (*m_nukes_)[m_members_[i]];
    }

    /// Implements getting a read-only Nucleus
    const_reference get_nuke_(size_type i) const override {
        return std::as_const(*m_nukes_)[m_members_[i]];
    }

    /// Impelments size
    size_type size_() const noexcept override { return m_members_.size(); }

    /// Implements are_equal
    bool are_equal_(const base_type& rhs) const noexcept override {
        auto prhs = dynamic_cast<const NucleiSubset*>(&rhs);
        if(prhs == nullptr) return false;
        return (*this) == (*prhs);
    }

private:
    /// The full set of nuclei
    nuclei_pointer m_nukes_;

    /// The indices in *this
    std::vector<size_type> m_members_;
};

// -----------------------------------------------------------------------------
// -- Implementations
// -----------------------------------------------------------------------------

template<typename NucleiType>
inline bool NucleiSubset<NucleiType>::operator==(
  const NucleiSubset& rhs) const noexcept {
    if(m_members_ != rhs.m_members_) return false;

    // Try to avoid comparing the Nuclei by checking for the same address
    if(m_nukes_ == rhs.m_nukes_) return true;

    // Above works if they're both null, now see if only one is null
    if(is_null() || rhs.is_null()) return false;

    // Both are non-null, so compare the Nuclei
    return *m_nukes_ == *rhs.m_nukes_;
}

} // namespace chemist::detail_