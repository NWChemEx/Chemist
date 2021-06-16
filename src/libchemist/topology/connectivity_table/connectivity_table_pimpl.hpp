#pragma once
#include "libchemist/topology/connectivity_table.hpp"

namespace libchemist::topology::detail_ {

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
    size_type nbonds() const noexcept;

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


} // namespace libchemist::topology::detail_
