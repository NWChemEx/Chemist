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
#include <chemist/molecule/molecule.hpp>
#include <optional>

namespace chemist {

/** @brief Caps a severed bond.
 *
 *  See
 *  https://nwchemex-project.github.io/Chemist/developer/design/chemistry/caps.html
 *  for design details.
 *
 */
class Cap {
public:
    /// Type of the atoms comprising the cap
    using atom_type = Molecule::atom_type;

    /// Used to store the atoms comprising the cap
    using atom_vector = std::vector<atom_type>;

    /// Type of a read/write reference to an atom comprising the cap
    using atom_reference = typename atom_vector::reference;

    /// Type of a read-only reference to an atom comprising the cap
    using const_atom_reference = typename atom_vector::const_reference;

    /// Type used by *this for indexing and offsets. Unsigned integral type.
    using size_type = Molecule::size_type;

    /** @brief Creates a defaulted cap.
     *
     *  A defaulted cap is comprised of zero atoms, has no anchor atom, and
     *  does not replace any atom.
     *
     *  @throw None No throw guarantee.
     */
    Cap() = default;

    /** @brief Creates a cap with the provided state.
     *
     *  @tparam Args The types of the atoms comprising the cap. Each type in the
     *               parameter pack is assumed to be implicitly convertible to
     *               an object of type atom_type.
     *
     *  @param[in] anchor The index of the atom to which *this is anchored. It
     *                    is the responsibility of the user to ensure that the
     *                    index is correct and in bounds for their molecular
     *                    system.
     *  @param[in] replaced The index of the atom being replaced by *this. It
     *                      is the responsibility of the user to ensure that
     *                      the index is correct and in bounds for their
     *                      molecular system.
     *
     *  @param[in] atoms The literal atoms comprising the cap.
     *
     *  @throw std::bad_alloc if there is a problem allocating state for *this.
     *                        Strong throw guarantee.
     */
    template<typename... Args>
    Cap(size_type anchor, size_type replaced, Args&&... atoms) :
      m_atoms_{std::forward<Args>(atoms)...},
      m_anchor_(anchor),
      m_replaced_(replaced) {}

    /** @brief Allows adding additional atoms to the cap.
     *
     *  @param[in] atom The atom to add to the cap represented by *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        new atom. Strong throw guarantee.
     */
    void add_cap_atom(atom_type atom) {
        m_atoms_.emplace_back(std::move(atom));
    }

    /** @brief Returns the @p i -th atom comprising *this.
     *
     *  This method is used to access the atoms which have been added to *this.
     *  The atoms are stored in the order they were added. So atom 0 will be the
     *  first atom added, atom 1 the second, etc.
     *
     *  @param[in] i The index of the atom to retrieve. @p i must be in the
     *               range [0, size()).
     *
     *  @return A mutable reference to the @p i -th atom.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, size()).
     *                           Strong throw guarantee.
     */
    atom_reference cap_atom(size_type i) { return m_atoms_.at(i); }

    /** @brief Returns the @p i-th atom comprising *this.
     *
     *  This method is the same as the non-const version except that the
     *  returned atom is read-only.
     *
     *  @param[in] i The index of the atom to retrieve. @p i must be in the
     *             range [0, size()).
     *
     *  @return A read-only reference to the @p i -th atom.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, size()).
     *                           Strong throw guarantee.
     */
    const_atom_reference cap_atom(size_type i) const { return m_atoms_.at(i); }

    /** @brief How many atoms comprise *this?
     *
     *  @return The number of atoms comprising *this.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_atoms_.size(); }

    /** @brief Sets the index of the anchor atom.
     *
     *  This method can be used to set the index of the atom to which *this is
     *  attached. If the anchor index was already set, this function will
     *  overwrite the value.
     *
     *  @param[in] anchor The index of the atom to which *this is anchored.
     *
     *  @throw None No throw guarantee.
     */
    void set_anchor_index(size_type anchor) noexcept {
        m_anchor_.emplace(anchor);
    }

    /** @brief Gets the index of the anchor atom.
     *
     *  Assuming the user has set the index of the anchor atom, this method
     *  can be used to get that index.
     *
     *  @return The index of the atom *this is attached to.
     *
     *  @throw std::bad_optional_access if the anchor atom has not been set.
     *                                  Strong throw guarantee.
     */
    size_type get_anchor_index() const { return m_anchor_.value(); }

    /** @brief Sets the index of the atom *this replaces.
     *
     *  This method can be used to set the index of the atom being replaced by
     *  *this.
     *
     *  @param[in] replaced The index of the atom *this replaces.
     *
     *  @throw None No throw guarantee.
     */
    void set_replaced_index(size_type replaced) noexcept {
        m_replaced_.emplace(replaced);
    }

    /** @brief What is the index of the atom replaced by *this?
     *
     *  This method can be used to retrieve the index of the atom which *this
     *  replaces (assuming it has been set).
     *
     *  @return The index of the atom *this replaces.
     *
     *  @throw std::bad_optional_access if the index has not been set. Strong
     *                                  throw guarantee.
     */
    size_type get_replaced_index() const { return m_replaced_.value(); }

    /** @brief Determines if two cap instances are value equal.
     *
     *  Two non-default Cap instances are value equal if they both are
     *  anchored to the same atom, replace the same atom, and are comprised of
     *  atoms which are value equal (atoms must also be stored in the same
     *  order). Cap instances may also be value equal if they are both default
     *  instances.
     *
     *  @param[in] rhs The instance we are comparing *this to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Cap& rhs) const noexcept;

    /** @brief Determines if two Cap instances are different.
     *
     *  Cap instances are different if they are not value equal. This function
     *  simply negates operator==. See the description of operator== for the
     *  definition of value equal.
     *
     *  @param[in] rhs The instance we are comparing *this to.
     *
     *  @return False if *this is value equal @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const Cap& rhs) const noexcept { return !(*this == rhs); }

private:
    /// Makes a tuple containing read-only references to all the state
    auto state_() const { return std::tie(m_anchor_, m_replaced_, m_atoms_); }

    /// The index (in the parent molecule) of the atom the cap is attached to
    std::optional<size_type> m_anchor_;

    /// The index (in the parent molecule) of the atom replaced by this cap
    std::optional<size_type> m_replaced_;

    /// The atom(s) actually comprising the cap
    atom_vector m_atoms_;
};

// -----------------------------------------------------------------------------
// -- Out of line inline definitions
// -----------------------------------------------------------------------------

inline bool Cap::operator==(const Cap& rhs) const noexcept {
    return state_() == rhs.state_();
}

} // namespace chemist
