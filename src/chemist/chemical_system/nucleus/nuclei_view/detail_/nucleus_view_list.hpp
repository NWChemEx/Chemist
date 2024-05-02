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

/** @brief Implements a NucleiView that holds a list of NucleusView objects.
 *
 *  @tparam NucleiType The cv-qualified Nuclei type that *this will act like it
 *                     aliases.
 *
 *  This PIMPL is useful when you are cherry-picking nuclei from several
 *  different Nuclei objects and want to use those nuclei as if they were a
 *  single Nuclei object.
 */
template<typename NucleiType>
class NucleusViewList : public NucleiViewPIMPL<NucleiType> {
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

    /// Type of a container filled with references to nucleus objects
    using reference_container = typename parent_type::reference_container;

    /// Aliases an empty set of nuclei
    NucleusViewList() noexcept = default;

    /** @brief Aliases the Nuclei object represented by @p nuclei.
     *
     *  After construction *this will alias a Nuclei object whose members are
     *  the Nucleus objects aliased by the elements of @p nuclei. The order of
     *  elements in @p nuclei will be preserved.
     *
     *  @param[in] nuclei The aliased Nucleus objects to form the members of
     *                    *this.
     *
     *  @throw None No throw guarantee.
     */
    explicit NucleusViewList(reference_container nuclei) :
      m_nuclei_(std::move(nuclei)) {}

    /** @brief Creates a new view containing copies of the views in @p other.
     *
     *  This ctor will initialize *this with copies of the NucleusView objects
     *  in @p other. The resulting copies still alias the same Nucleus objects,
     *  but causing the NucleusViews in *this to alias different Nucleus objects
     *  will NOT affect @p other.
     *
     *  @param[in] other The object whose state should be copied.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy. Strong
     *                        throw guarantee.
     *
     */
    NucleusViewList(const NucleusViewList& other) = default;

    /// Implemented generically by NucleiView, deleted to avoid slicing
    ///@{
    NucleusViewList(NucleusViewList&&) = delete;
    NucleusViewList& operator=(const NucleusViewList&) = delete;
    NucleusViewList& operator=(NucleusViewList&&) = delete;
    ///@}

    /** @brief Are the Nuclei aliased by *this and @p rhs value equal?
     *
     *  This method compares the Nuclei object aliased by *this to the one
     *  aliased by @p rhs.
     *
     *  @param[in] rhs The NucleusViewList object to compare against.
     *
     *  @return True if *this and @p rhs alias Nuclei objects which are value
     *          equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const NucleusViewList& rhs) const noexcept {
        return m_nuclei_ == rhs.m_nuclei_;
    }

protected:
    /// Implements clone
    pimpl_pointer clone_() const override {
        return std::make_unique<NucleusViewList>(*this);
    }

    /// Implements getting a mutable Nucleus
    reference get_nuke_(size_type i) override { return m_nuclei_[i]; }

    /// Implements getting a read-only Nucleus
    const_reference get_nuke_(size_type i) const override {
        return m_nuclei_[i];
    }

    /// Implements size
    size_type size_() const noexcept override { return m_nuclei_.size(); }

    /// Implements are_equal
    bool are_equal_(const base_type& rhs) const noexcept override {
        return base_type::template are_equal_impl_<NucleusViewList>(rhs);
    }

private:
    /// The actual nuclei
    reference_container m_nuclei_;
};

} // namespace chemist::detail_
