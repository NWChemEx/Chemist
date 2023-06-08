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
#include <chemist/capping/cap.hpp>
#include <utilities/containers/indexable_container_base.hpp>
#include <vector>

namespace chemist {

/** @brief The set of caps associated with a fragment.
 *
 *  When a fragment of a molecule severs covalent bonds, those bonds must be
 *  capped. This class is used to track the caps. For design details see:
 *  https://nwchemex-project.github.io/Chemist/developer/design/chemistry/caps.html
 */
class CapSet : public utilities::IndexableContainerBase<CapSet> {
private:
    /// Type of the base class
    using base_type = utilities::IndexableContainerBase<CapSet>;

public:
    /// Type of each cap
    using value_type = Cap;

    /// Type of a mutable reference to a cap
    using reference = value_type&;

    /// Type of a read-only reference to a cap
    using const_reference = const value_type&;

    /// Type of the atoms in the caps
    using atom_type = value_type::atom_type;

    /// Type used to index the anchor/replaced atoms. Unsigned integral type.
    using atom_index_type = value_type::size_type;

    /// Type used for indexing and offsets by this class. Unsigned integral.
    using size_type = typename base_type::size_type;

    /** @brief Creates ane empty CapSet.
     *
     *  The CapSet resulting from the default ctor has no caps in it. Caps can
     *  be added by calling either push_back or add_cap.
     *
     *  @throw None No throw guarantee.
     */
    CapSet() = default;

    /** @brief Creates a CapSet initialized to the values in @p il.
     *
     *  This ctor is intended primarily for unit testing and tutorials as it
     *  requires you to initialize *this with a compile-time known number of
     *  caps.
     *
     *  @param[in] il The caps to initialize *this with.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        initial state. Strong throw guarantee.
     */
    explicit CapSet(std::initializer_list<value_type> il) : m_caps_(il) {}

    /** @brief Adds @p cap to *this.
     *
     *  This method can be used to add an already created Cap instance to
     *  *this. The cap is appended on to the end of *this. If prior to calling
     *  this method *this has @f$n@f$ caps. After a successful invocation of
     *  this method, *this will have @f$n + 1@f$ caps and @f$(*this)[n]@f$ will
     *  be @p cap.
     *
     *  @param[in] cap The value being added to *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for
     *                        storing @p cap. Strong throw guarantee.
     */
    void push_back(value_type cap) { m_caps_.emplace_back(std::move(cap)); }

    /** @brief Used to construct a cap in place.
     *
     *  @tparam Args The types of the atoms comprising the caps. Must be
     *               implicitly convertible to atom_type.
     *
     *  @param[in] anchor The index of atom the cap is attached to.
     *  @param[in] replaced The index of the atom the cap is replacing.
     *  @param[in] atoms The atoms comprising the cap.
     *
     *  @throw std::bad_alloc if there is a problem creating the cap or adding
     *                        it to *this. Strong throw guarantee.
     */
    template<typename... Args>
    void add_cap(atom_index_type anchor, atom_index_type replaced,
                 Args&&... atoms) {
        push_back(value_type(anchor, replaced, std::forward<Args>(atoms)...));
    }

private:
    /// Allows the base to implement *this via CRTP
    friend base_type;

    /// Container the caps are stored in
    using cap_set = std::vector<value_type>;

    /// Used to implement operator[]/at
    reference at_(size_type i) noexcept { return m_caps_[i]; }

    /// Used to implement operator[] const/at const
    const_reference at_(size_type i) const noexcept { return m_caps_[i]; }

    /// Used to implement size()
    size_type size_() const noexcept { return m_caps_.size(); }

    /// The actual caps in this set
    cap_set m_caps_;
};

} // namespace chemist
