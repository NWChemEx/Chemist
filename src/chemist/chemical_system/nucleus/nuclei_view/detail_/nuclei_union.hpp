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
#include <cassert>

namespace chemist::detail_ {

/** @brief Implements a NucleiView that is a union of NucleiView objects.
 *
 *  @tparam NucleiType The cv-qualified Nuclei type that *this will act like it
 *                     aliases.
 *
 *  This PIMPL is useful when you want to use one or more NucleiView objects as
 *  if they were a single NucleiView object, but don't want to rearrange the
 *  underlying state.
 *
 *  @warning At present, the resulting union can contain duplicates. This is
 *           consistent with the Nuclei class itself which allows duplicates.
 */
template<typename NucleiType>
class NucleiUnion : public NucleiViewPIMPL<NucleiType> {
private:
    /// Type of the base class
    using base_type = NucleiViewPIMPL<NucleiType>;

public:
    /// The type *this is implementing
    using typename base_type::parent_type;

    /// Type of a pointer pointing to the base class
    using typename base_type::pimpl_pointer;

    /// The type of a mutable reference to a nucleus
    using typename base_type::reference;

    /// The type of a read-only reference to a nucleus
    using typename base_type::const_reference;

    /// Type used for indexing and offsets
    using typename base_type::size_type;

    /// Type of a container filled with NucleiView objects
    using nuclei_view_container = typename parent_type::nuclei_view_container;

    /// Aliases an empty set of nuclei
    NucleiUnion() noexcept = default;

    /** @brief Aliases the Nuclei object resulting from taking the union of
     *         the Nuclei objects aliased by @p nuclei.
     *
     *
     *  After construction *this will alias a Nuclei object whose members are
     *  the union of the Nuclei objects aliased in @p nuclei.
     *
     *  @param[in] nuclei The aliased Nuclei objects to form the members of
     *                    *this.
     *
     *  @throw None No throw guarantee.
     */
    explicit NucleiUnion(nuclei_view_container nuclei) :
      m_nuclei_(std::move(nuclei)) {}

    /** @brief Creates a new view containing copies of the views in @p other.
     *
     *  This ctor will initialize *this with copies of the NucleiView objects
     *  in @p other. The resulting copies still alias the same Nuclei objects,
     *  but causing the NucleiViews in *this to alias different Nuclei objects
     *  will NOT affect @p other.
     *
     *  @param[in] other The object whose state should be copied.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy. Strong
     *                        throw guarantee.
     *
     */
    NucleiUnion(const NucleiUnion& other) = default;

    /// Implemented generically by NucleiView, deleted to avoid slicing
    ///@{

    NucleiUnion(NucleiUnion&&) = delete;
    NucleiUnion& operator=(const NucleiUnion&) = delete;
    NucleiUnion& operator=(NucleiUnion&&) = delete;

    /** @brief Are the Nuclei aliased by *this and @p rhs value equal?
     *
     *  This method compares the Nuclei object aliased by *this to the one
     *  aliased by @p rhs.
     *
     *  @param[in] rhs The NucleiUnion object to compare against.
     *
     *  @return True if *this and @p rhs alias Nuclei objects which are value
     *          equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const NucleiUnion& rhs) const noexcept;

protected:
    /// Implements clone
    pimpl_pointer clone_() const override {
        return std::make_unique<NucleiUnion>(*this);
    }

    /// Implements getting a mutable Nucleus
    reference get_nuke_(size_type i) override;

    /// Implements getting a read-only Nucleus
    const_reference get_nuke_(size_type i) const override;

    /// Implements size
    size_type size_() const noexcept override;

    /// Implements are_equal
    bool are_equal_(const base_type& rhs) const noexcept override {
        return base_type::template are_equal_impl_<NucleiUnion>(rhs);
    }

private:
    /// The actual nuclei
    nuclei_view_container m_nuclei_;
};

template<typename NucleiType>
bool NucleiUnion<NucleiType>::operator==(
  const NucleiUnion& rhs) const noexcept {
    if(this->size() != rhs.size()) return false;
    if(this->size() == 0) return true;

    for(size_type i = 0; i < this->size(); ++i) {
        if(this->get_nuke(i) != rhs.get_nuke(i)) return false;
    }
    return true;
}

template<typename NucleiType>
typename NucleiUnion<NucleiType>::reference NucleiUnion<NucleiType>::get_nuke_(
  size_type i) {
    // Trying to return from the loop yields a compiler warning, about missing
    // return type, so we move the return outside the loop.
    size_type j = 0;
    for(; j < m_nuclei_.size(); ++j) {
        if(m_nuclei_[j].size() > i) break;
        i -= m_nuclei_[j].size();
    }
    return m_nuclei_[j][i];
}

template<typename NucleiType>
typename NucleiUnion<NucleiType>::const_reference
NucleiUnion<NucleiType>::get_nuke_(size_type i) const {
    size_type j = 0;
    for(; j < m_nuclei_.size(); ++j) {
        if(m_nuclei_[j].size() > i) break;
        i -= m_nuclei_[j].size();
    }
    return m_nuclei_[j][i];
}

template<typename NucleiType>
typename NucleiUnion<NucleiType>::size_type NucleiUnion<NucleiType>::size_()
  const noexcept {
    size_type total_size = 0;
    for(auto& nuclei_i : m_nuclei_) total_size += nuclei_i.size();
    return total_size;
}

} // namespace chemist::detail_
