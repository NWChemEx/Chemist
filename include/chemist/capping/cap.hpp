#pragma once
#include <chemist/molecule/molecule.hpp>

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

    /// Type of a read/write reference to an object of type size_type
    using size_reference = size_type&;

    /// Type of a read-only reference to an object of type size_type
    using const_size_reference = const size_type&;

    /** @brief Creates a defaulted cap.
     *
     *  A defaulted cap is comprised of zero atoms, is anchored on atom 0, and
     *  replaces atom 0 (the latter two conditions are simply the default
     *  state)
     */
    Cap() = default;

    template<typename... Args>
    Cap(size_type anchor, size_type replaced, Args&&... atoms) :
      m_atoms_{std::forward<Args>(atoms)...},
      m_anchor_(anchor),
      m_replaced_(replaced) {}

    void add_cap_atom(atom_type atom) {
        m_atoms_.emplace_back(std::move(atom));
    }

    atom_reference atom(size_type i) { return m_atoms_.at(i); }

    const_atom_reference atom(size_type i) const { return m_atoms_.at(i); }

    size_type size() const noexcept { return m_atoms_.size(); }

    size_reference anchor_index() noexcept { return m_anchor_; }

    const_size_reference anchor_index() const noexcept { return m_anchor_; }

    size_reference replaced_index() noexcept { return m_replaced_; }

    const_size_reference replaced_index() const noexcept { return m_replaced_; }

    bool operator==(const Cap& rhs) const noexcept;

    bool operator!=(const Cap& rhs) const noexcept { return !(*this == rhs); }

private:
    /// Makes a tuple containing read-only references to all the state
    auto state_() const { return std::tie(m_anchor_, m_replaced_, m_atoms_); }

    /// The index (in the parent molecule) of the atom the cap is attached to
    size_type m_anchor_ = 0;

    /// The index (in the parent molecule) of the atom replaced by this cap
    size_type m_replaced_ = 0;

    /// The atom(s) actually comprising the cap
    atom_vector m_atoms_;
};

inline bool Cap::operator==(const Cap& rhs) const noexcept {
    return state_() == rhs.state_();
}

} // namespace chemist
