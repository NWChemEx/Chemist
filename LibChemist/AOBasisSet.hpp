#pragma once
#include "LibChemist/AOShell.hpp"
#include <vector> //Needed for iterators

namespace LibChemist {
namespace detail_ {
/// Forward declare the PIMPL
template<typename T>
class ContainerPIMPL;
} // namespace detail_

/** @brief A class for storing many AOShell instances
 *
 *  The AOBasisSet class aggregates a collection of AOShell instances.
 *  Additionally it ensures the members of those shells are stored contiguously
 *  in memory as is required for most integral libraries.  Thus something like
 *  ```.cpp
 *  double* c_ij = &(bs[0].coef(0));
 *  for(auto shelli : bs) {
 *      //Assume this takes a begin and end iterator
 *      do_something_with_coefs(c_ij, c_ij + shelli.nprims());
 *      c_ij+=shelli.nprims();
 *  }
 *  ```
 *  is guaranteed to work.
 */
class AOBasisSet {
public:
    /// The type of the elements stored in this container
    using value_type = AOShell;

    /// The type of a reference to a shell
    using reference = value_type&;

    /// The type of a const reference to a shell
    using const_reference = const value_type&;

    /// The type of a counting number used by this class
    using size_type = std::size_t;

    /// The type of an iterator to this class
    using iterator = typename std::vector<value_type>::iterator;

    /// The type of a read-only iterator to this class
    using const_iterator = typename std::vector<value_type>::const_iterator;

    /**
     * @brief Makes an empty AOBasisSet instance.
     *
     * @throw std::bad_alloc if there is insufficient memory to make a new
     *        instance of the PIMPL.  Strong throw guarantee.
     */
    AOBasisSet();

    /**
     * @defgroup Copy/Move CTors and Assignment
     *
     * All copies of AOBasisSet instances are deep copies.
     *
     * @param[in] rhs the AOBasisSet instance to copy/move.  After moving
     *            @p rhs, it is guaranteed to be in a valid, but otherwise
     *            undefined state.
     * @throw std::bad_alloc Copy ctor/assignment throws if there is
     *        insufficient memory to copy the state over.  Strong throw
     *        guarantee.  The move ctor/assignment operator is no throw
     *        guarantee.
     */
    ///@{
    AOBasisSet(const AOBasisSet& rhs);
    AOBasisSet(AOBasisSet&& rhs) noexcept;
    AOBasisSet& operator=(const AOBasisSet& rhs);
    AOBasisSet& operator=(AOBasisSet&& rhs) noexcept;
    ///@}

    /**
     * @brief Frees up memory associated with this basis set.
     *
     * This dtor amounts to the default dtor behind the scenes.
     *
     * @throw None. No throw guarantee.
     */
    ~AOBasisSet() noexcept;

    /**
     * @defgroup State CTors
     *
     * @brief The constructors in this section allow one to create a new basis
     * set instance with state.
     *
     * @param[in] shell The shell to add to the basis set.
     * @param[in] il An initializer list of AOShell instances.
     * @param[in] args The remaining arguments to be parsed.
     * @tparam Args The types of the remaining arguments.  Assumed to be
     *         `AOShell`.
     *
     * @throw std::bad_alloc if there is insufficient memory to complete the
     *        copy.  Strong throw guarantee.
     */
    ///@{
    template<typename... Args>
    explicit AOBasisSet(const_reference shell, Args&&... args) :
      AOBasisSet({shell, std::forward<Args>(args)...}) {}

    AOBasisSet(std::initializer_list<value_type> il);
    ///@}

    /**
     * @brief Adds a shell to the basis set
     *
     * @warning Calling this function may cause the internal state to be
     * reallocated.  If this occurs, all pointers and references to shells
     * within this basis set are invalidated.
     *
     * @param shell The shell to add.  The contents of @p shell will be copied
     *        into the basis set.
     * @throw std::bad_alloc if there is insufficient memory to perform the
     *        copy. Weak throw guarantee.
     */
    void push_back(value_type shell);

    /**
     * @brief Determines the number of shells in this basis set.
     * @return The number of shells in this basis set.
     * @throw None No throw guarantee.
     */
    size_type nshells() const noexcept;
    size_type nbf() const noexcept {
        size_type total = 0;
        for(const auto& shelli : *this) total += shelli.size();
        return total;
    }

    /**
     * @defgroup Accessors
     *
     * @brief These member functions allow the user to get a particular shell
     *        from within the basis set.
     *
     * It should be noted that the AOShell instances returned by an AOBasisSet
     * are basically "views" of the basis set.  What that means is their data
     * members are linked to the internal state of the basis set.  Thus:
     *
     * ```.cpp
     * AOBasisSet bs(AOShell(AOPrimitive{1.2, 2.3}));
     * auto& shelli = bs[0];
     * assert(shelli.alpha(0) != 2.4);
     * shelli.alpha(0) = 2.4;
     * auto& shellj = bs[0];
     * assert(shellj.alpha(0) == 2.4);
     * ```
     * will run as expected.  These links can be broken by copying.  In other
     * words:
     *
     * ```.cpp
     * AOBasisSet bs(AOShell(AOPrimitive{1.2, 2.3}));
     * //Note the lack of a reference here
     * auto shelli = bs[0];
     * assert(shelli.alpha(0) != 2.4);
     * shelli.alpha(0) = 2.4;
     * auto& shellj = bs[0];
     * assert(shelli.alpha(0) == 2.4);
     * assert(shellj.alpha(0) != 2.4);
     * ```
     * demonstrates how to break the link.
     *
     * @param[in] i The number of the shell to retrieve.  i should be in the
     *            range [0, size()).  No bounds check is performed.
     * @return The requested shell.  Note the shell is a reference to the state
     *         stored within this class and is only valid for as long as the
     *         AOBasisSet instance that created it is.
     * @throw None No throw guarantee.
     */
    ///@{
    reference at(size_type i) noexcept;
    const_reference at(size_type i) const noexcept {
        return const_cast<AOBasisSet&>(*this).at(i);
    }
    reference operator[](size_type i) noexcept { return at(i); }
    const_reference operator[](size_type i) const noexcept { return at(i); }
    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }
    ///@}

private:
    /// The object that actually implements the AOBasisSet class
    std::unique_ptr<detail_::ContainerPIMPL<AOBasisSet>> pimpl_;
};

/**
 * @brief Returns the maximum angular momentum of a basis set and the first
 * shell with that value.
 * @relates AOBasisSet
 *
 * @param[in] bs The basis set to consider.
 * @return A pair of the maximum angular momentum value and an iterator to the
 *         first shell with that value.  If there are no shells in the basis set
 *         the return will be 0 for the angular momentum and a pointer to just
 *         past the end of the basis set.
 * @throw None. No throw guarantee.
 */
std::pair<typename AOBasisSet::size_type, typename AOBasisSet::iterator> max_l(
  AOBasisSet& bs) noexcept;

/**
 * @brief Returns the maximum angular momentum of a read-only basis set and the
 * first shell with that value.
 * @relates AOBasisSet
 *
 * See the documentation for the read/write version.
 */
std::pair<typename AOBasisSet::size_type, typename AOBasisSet::const_iterator>
max_l(const AOBasisSet& bs) noexcept;


/**
 * @defgroup AOBasisSet comparison operators
 * @brief Compares two AOBasisSet instances
 *
 * Two AOBasisSet instances are equal if they have the same number of shells,
 * and `lhs[i] == rhs[i]` for all shells.
 *
 * @param[in] lhs The AOBasisSet on the left of the comparison
 * @param[in] rhs The AOBasisSet on the right of the comparison
 * @return The value of the requested comparision
 * @throw None All comparisons are no throw guarantee.
 */
///@{
inline bool operator==(const LibChemist::AOBasisSet& lhs,
                       const LibChemist::AOBasisSet& rhs) noexcept {
    if(lhs.nshells() != rhs.nshells()) return false;
    for(std::size_t i = 0; i < lhs.nshells(); ++i)
        if(lhs[i] != rhs[i]) return false;
    return true;
}

inline bool operator!=(const LibChemist::AOBasisSet& lhs,
                       const LibChemist::AOBasisSet& rhs) noexcept {
    return !(lhs == rhs);
}
///@}

/**
 * @brief Allows printing of an AOBasisSet instance
 *
 * @param os The stream to print the basis set to.
 * @param bs The basis set instance to print.
 * @return The stream containing the text representation of the basis set.
 * @throw std::ios_base::failure if anything goes wrong.  Weak throw guarantee.
 */
std::ostream& operator<<(std::ostream& os, const LibChemist::AOBasisSet& bs);

} // namespace LibChemist
