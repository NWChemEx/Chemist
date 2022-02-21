#pragma once
#include "chemist/detail_/hashing.hpp"
#include <array>
#include <iostream>
#include <memory>
#include <vector>

namespace chemist::topology {
namespace detail_ {
class ConnectivityTablePIMPL;
}

class ConnectivityTable {
public:
    /// Type of an object which implements this class
    using pimpl_type = detail_::ConnectivityTablePIMPL;
    /// A pointer to a PIMPL instance
    using pimpl_ptr = std::unique_ptr<pimpl_type>;

    /// Type used for indexing/sizes
    using size_type = std::size_t;
    /// Type used for a bond
    using pair_type = std::array<size_type, 2>;
    /// Type used for a list of bonds
    using bond_list_type = std::vector<pair_type>;

    /** @brief Creates an empty ConnectivityTable.
     *
     *  The instance created with this ctor is capable of holding the
     *  connectivity information for 0 atoms. Users can call `set_n_atoms` to
     *  make the instance suitable for more atoms.
     *
     *  @throw None No throw guarantee.
     */
    ConnectivityTable() noexcept;

    /** @brief Creates an instance which can hold connectivity information for
     *         @p natoms atoms.
     *
     *  @param[in] natoms The number of atoms the table will be for.
     *
     *  @throw std::bad_alloc if there is not enough memory to allocate the
     *                        internal state. Strong throw guarantee.
     */
    explicit ConnectivityTable(size_type natoms);

    /** @brief Makes a deep copy of another ConnectivityTable instance.
     *
     *  @param[in] other The instance to copy.
     *
     *  @throw std::bad_alloc if there is a problem copying @p other. Strong
     *                        throw guarantee.
     */
    ConnectivityTable(const ConnectivityTable& other);

    /** @brief Takes ownership of another ConnectivityTable instance.
     *
     *  @param[in,out] other The instance whose state will be transferred. After
     *                       the move @p other will be in a default state.
     * @throw None no throw guarantee.
     */
    ConnectivityTable(ConnectivityTable&& other) noexcept;

    /** @brief Replaces the current instance's state with a deep copy of another
     *         ConnectivityTable instance's state.
     *
     *  @param[in] rhs The instance to copy.
     *
     *  @return The current instance, with it's state set to a deep copy of
     *          @p rhs's state, to support operator chaining.
     *
     *  @throw std::bad_alloc if there is a problem copying @p other. Strong
     *                        throw guarantee.
     */
    ConnectivityTable& operator=(const ConnectivityTable& rhs);

    /** @brief Replaces the current ConnectivityTables' state by taking
     *         ownership of another ConnectivityTable instance.
     *
     *  @param[in,out] rhs The instance whose state will be transferred. After
     *                     the move @p rhs will be in a default state.
     *
     *  @return The current instance, after taking ownership of @p rhs, to
     *          facilitate operator chaining.
     *
     *  @throw None no throw guarantee.
     */
    ConnectivityTable& operator=(ConnectivityTable&& rhs) noexcept;

    /// Default no-throw dtor
    ~ConnectivityTable() noexcept;

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
    void set_n_atoms(size_type natoms);

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

    /** @brief The number of atoms in this table.
     *
     *  @return The number of atoms this table is for.
     *
     *  @throw None no throw guarantee.
     */
    size_type natoms() const noexcept;

    /** @brief The total number of bonds in this table.
     *
     *  @return The number of bonds this table currently holds.
     *
     *  @throw None no throw guarantee.
     */
    size_type nbonds() const noexcept;

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
     *  @throw std::runtime_error if there is no PIMPL. Strong throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        return type. Strong throw guarantee.
     */
    bond_list_type bonds() const;

    /** @brief Returns the set of atoms bonded to atom @p i.
     *
     *  This function will return a set which contains the zero-based indices of
     *  the atoms which are bonded to the atom with (zero-based) index @p i. The
     *  set will be empty if no atoms are bonded to @p i.
     *
     *  @note add_bond will not allow @p i to be bonded to itself and thus @p i
     *        will not be included in the resulting set.
     *
     *  @param[in] i the zero-based index of the atom of interest.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, natoms()).
     *                           Strong throw guarantee.
     */
    std::set<size_type> bonded_atoms(size_type i) const;

    /** @brief Hashes the current ConnectivityTable.
     *
     *  @param[in,out] h The hasher instance to use for hashing this instance.
     *                   After the call @p h will contain a hash of this
     *                   instance.
     */
    void hash(chemist::detail_::Hasher& h) const;

private:
    /** @brief Returns the PIMPL in a read/write state, making a PIMPL if the
     *         instance does not have one.
     *
     *  @return The instance's current PIMPL in a read/write state.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new PIMPL.
     *                        Strong throw guarantee.
     */
    pimpl_type& pimpl_();

    /// The object which implements the ConnectivityTable
    pimpl_ptr m_pimpl_;
}; // class ConnectivityTable

/** @brief Compares two ConnectivityTable instances for equality.
 *
 *  @relates ConnectivityTable
 *
 *  Two ConnectivityTable instances are equal if they are:
 *  - for the same number of atoms
 *  - contain the same number of bonds
 *  - contain the same bonds
 *
 * @param[in] lhs The instance on the left-side of the equality.
 * @param[in] rhs The instance on the right-side of the equality.
 *
 * @return True if @p lhs and @p rhs are equal. False otherwise.
 *
 * @throw std::bad_alloc if calling bonds() throws. Strong throw guarantee.
 */
bool operator==(const ConnectivityTable& lhs, const ConnectivityTable& rhs);

/** @brief Determines if two ConnectivityTable instances are different.
 *
 *  @relates ConnectivityTable
 *
 *  Two ConnectivityTable instances are equal if they are:
 *  - for the same number of atoms
 *  - contain the same number of bonds
 *  - contain the same bonds
 *
 * @param[in] lhs The instance on the left-side of the inequality.
 * @param[in] rhs The instance on the right-side of the inequality.
 *
 * @return False if @p lhs and @p rhs are equal. True otherwise.
 *
 * @throw std::bad_alloc if calling bonds() throws. Strong throw guarantee.
 */
inline bool operator!=(const ConnectivityTable& lhs,
                       const ConnectivityTable& rhs) {
    return !(lhs == rhs);
}

/** @brief Adds a string representation of a ConnectivityTable to an ostream.
 *
 *  @relates ConnectivityTable
 *
 *  This function facilitates printing out a ConnectivityTable instance. The
 *  resulting string representation is simply a list of bonds, one per line.
 *
 *  @param[in,out] os The ostream to add the string representation to. After the
 *                    call @p os will contain the string representation of @p t.
 *  @param[in] t The ConnectivityTable instance that is getting printed.
 *
 *  @return To support operator chaining this function also returns @p os with
 *          its internal buffer updated to include a string representation of
 *          @p t.
 *
 *  @throw std::bad_alloc if calling bonds() throws. Strong throw guarantee.
 */
inline std::ostream& operator<<(std::ostream& os, const ConnectivityTable& t) {
    for(const auto& [i, j] : t.bonds()) {
        os << std::to_string(i) << " " << std::to_string(j) << std::endl;
    }
    return os;
}

} // namespace chemist::topology
