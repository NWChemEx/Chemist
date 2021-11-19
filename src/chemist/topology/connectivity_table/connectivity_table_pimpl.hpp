#pragma once
#include "chemist/topology/connectivity_table.hpp"

namespace chemist::topology::detail_ {

/** @brief PIMPL which stores the bonds in an adjacency matrix.
 *
 *  This PIMPL uses an natoms by natoms adjacency matrix to store the bonds. It
 *  only manipulates the elements (i,j) for which i < j (*i.e.*, the upper
 *  triangle of the adjacency matrix).
 */
class ConnectivityTablePIMPL {
public:
    /// Type used for indexing
    using size_type = typename ConnectivityTable::size_type;

    /// Type used for an index pair
    using pair_type = typename ConnectivityTable::pair_type;

    /// Type used for a list of bonds
    using bond_list_type = std::vector<pair_type>;

    /** @brief Creates an empty PIMPL.
     *
     *  The PIMPL created with this ctor is a connectivity table for 0 atoms.
     *  Users can call `set_natoms` to change how many atoms the table is for.
     *
     *  @throw None No throw guarantee.
     */
    ConnectivityTablePIMPL() = default;

    /** @brief Creates a PIMPL which can hold connectivity information for
     *         @p natoms atoms.
     *
     *  @param[in] natoms The number of atoms the table will be for.
     *
     *  @throw std::bad_alloc if there is not enough memory to allocate the
     *                        internal state. Strong throw guarantee.
     */
    explicit ConnectivityTablePIMPL(size_type natoms);

    /** @brief Sets the number of atoms the connectivity table is for.
     *
     *  This works similar to std::vector reserve. Basically it will make the
     *  current instance capable of storing connectivity information for
     *  @p natoms. If @p natoms is not the current the number of atoms that the
     *  table is for, the table will reallocate, and be in a state which has no
     *  bonds.
     *
     *  @param[in] natoms The number of atoms the connectivity table should be
     *                    able to hold.
     *
     *  @throw std::bad_alloc if there is insufficient memory to reallocate the
     *                        backend. Strong throw guarantee.
     */
    void set_natoms(size_type natoms);

    /** @brief The number of atoms in this table.
     *
     *  @return The number of atoms this table is for.
     *
     *  @throw None no throw guarantee.
     */
    inline size_type natoms() const noexcept { return m_natoms_; }

    /** @brief The total number of bonds in this table.
     *
     *  @return The number of bonds this table currently holds.
     *
     *  @throw None no throw guarantee.
     */
    auto nbonds() const noexcept;

    /** @brief Adds a bond between atoms @p i and @p j.
     *
     *  This function is used to register a bond between atoms @p i and @p j. If
     *  a bond is already present this is a no-op.
     *
     *  @note Regardless of internal storage conventions users can provide the
     *        arguments @p i and @p j in whatever order, *i.e*, it doesn't
     *        matter if @p i > @p j or vice versa.
     *
     *  @param[in] i the index of the first atom in the bond.
     *  @param[in] j the index of the second atom in the bond.
     *
     *  @throw std::out_of_range if either (or both) of @p i and/or @p j are not
     *                           in the range [0, natoms()). Strong throw
     *                           guarantee.
     *  @throw std::out_of_range if @p i equals @p j. Strong throw guarantee.
     */
    void add_bond(size_type i, size_type j);

    /** @brief Determines if two atoms are bonded.
     *
     *  This function can be used to inquire into whether two atoms are bonded.
     *
     *  @param[in] i the zero-based index of one of the atoms. Must be in the
     *               range [0, natoms())
     *  @param[in] j the zero-based index of the other atom. Must be in the
     *               range [0, natoms()).
     *
     *  @return True if atoms @p i and @p j are bonded and false otherwise.
     *
     *  @throw std::out_of_range if either @p i and/or @p j is not in the range
     *                           [0, natoms()). Strong throw guarantee.
     */
    bool are_bonded(size_type i, size_type j) const;

    /** @brief Returns a list of the bonds in the connectivity table.
     *
     *  This function will uses the internal representation of the connectivity
     *  table to return a list of bonds. Each bond is represented as a pair of
     *  zero-based indices such that the pair `{i, j}` means that atoms `i` and
     *  `j` are bonded. Pairs are always such that `i < j`. The final list is
     *  sorted lexicographically.
     *
     *  @return An `nbonds()` long list of the bonds in the table. Bonds are
     *          sorted in lexicographical order.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        return type. Strong throw guarantee.
     */
    bond_list_type bonds() const;

private:
    /// Ensures argument is less than `natoms()` and throws if it is not
    void bounds_check_(size_type atom_i) const;

    /** @brief Orders indices and checks that they make sense.
     *
     *  This function is used internally to make sure we are always accessing
     *  the upper-triangle of the adjacency matrix. It also wraps the checks we
     *  need to always perform (indices are unique and in range).
     *
     *  @param[in] i the index of the first atom in the bond.
     *  @param[in] j the index of the second atom in the bond.
     *
     *  @throw std::out_of_range if either (or both) of @p i and/or @p j are not
     *                           in the range [0, natoms()). Strong throw
     *                           guarantee.
     *  @throw std::out_of_range if @p i equals @p j. Strong throw guarantee.
     */
    pair_type sanitize_indices_(size_type i, size_type j) const;

    /// The number of atoms this table is for.
    size_type m_natoms_ = 0;

    // An m_natoms_ by m_natoms_ adjaceny matrix storing the connections
    std::vector<bool> m_connections_;
};

// ---------------------------- Implementations --------------------------------

inline ConnectivityTablePIMPL::ConnectivityTablePIMPL(size_type natoms) :
  m_natoms_(natoms), m_connections_(natoms * natoms, false) {}

inline void ConnectivityTablePIMPL::set_natoms(size_type natoms) {
    if(natoms == m_natoms_) return;
    std::vector<bool> temp(natoms * natoms, false);
    m_connections_.swap(temp);
    m_natoms_ = natoms;
}

inline auto ConnectivityTablePIMPL::nbonds() const noexcept {
    size_type n_bonds = 0;
    for(size_type i = 0; i < m_natoms_; ++i)
        for(size_type j = i + 1; j < m_natoms_; ++j)
            if(m_connections_[i * m_natoms_ + j]) ++n_bonds;
    return n_bonds;
}

inline bool ConnectivityTablePIMPL::are_bonded(size_type i, size_type j) const {
    auto [min, max] = sanitize_indices_(i, j);
    return m_connections_[min * m_natoms_ + max];
}

inline void ConnectivityTablePIMPL::add_bond(size_type i, size_type j) {
    auto [min, max]                       = sanitize_indices_(i, j);
    m_connections_[min * m_natoms_ + max] = true;
}

inline typename ConnectivityTablePIMPL::bond_list_type
ConnectivityTablePIMPL::bonds() const {
    const auto n_bonds = nbonds();
    bond_list_type rv(n_bonds);

    for(size_type i = 0, bonds_found = 0; i < m_natoms_; ++i) {
        for(size_type j = i + 1; j < m_natoms_; ++j) {
            if(are_bonded(i, j)) {
                rv[bonds_found++] = pair_type{i, j};
                if(bonds_found == n_bonds) return rv;
            }
        }
    }
    // We can get here if there's actually zero bonds...
    return rv;
}

inline void ConnectivityTablePIMPL::bounds_check_(size_type atom_i) const {
    if(atom_i < m_natoms_) return;

    const auto msg = std::to_string(atom_i) + " is greater than the number " +
                     "atoms (" + std::to_string(m_natoms_) + ").";
    throw std::out_of_range(msg);
}

inline typename ConnectivityTablePIMPL::pair_type
ConnectivityTablePIMPL::sanitize_indices_(size_type i, size_type j) const {
    auto min = std::min(i, j);
    auto max = std::max(i, j);
    bounds_check_(min);
    bounds_check_(max);
    if(min == max) {
        std::string msg = "Atom indices can not be equal! (i == j == ";
        msg += std::to_string(i) + ").";
        throw std::out_of_range(msg);
    }
    return pair_type{min, max};
}

} // namespace chemist::topology::detail_
