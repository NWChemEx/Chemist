#include "libchemist/sparse_map/detail_/domain_traits.hpp"
#include <memory>
#include <sde/detail_/memoization.hpp>
#include <set>

namespace libchemist::sparse_map::detail_ {

/** @brief Class which holds the state of the Domain.
 *
 *  The Domain holds a bunch of indices. There's a variety of ways that a Domain
 *  instance could actually hold these indices and we leave it up to the PIMPL
 *  to choose how this is done.
 *
 *  @note Once an index has been inserted into a Domain it can only be retrieved
 *        in a read-only fashion (this permits the Domain to be implemented in
 *        a manner which does not store every index).
 */
template<typename IndexType>
class DomainPIMPL {
private:
    /// Type of this instance
    using my_type = DomainPIMPL<IndexType>;
public:
    /// Type of the traits struct defining the types for the Domain hierarchy
    using traits_type = DomainTraits<IndexType>;

    /// The type used for offsets and indexing
    using size_type       = typename traits_type::size_type;

    /// The type used to store an index
    using value_type      = typename traits_type::value_type;

    /// The type of a read-only reference to an index
    using const_reference = typename traits_type::const_reference;

    virtual ~DomainPIMPL() noexcept = default;

    /** @brief Polymorphic copy constructor.
     *
     *  This method can be used to create a copy of the derived class through
     *  the base class. When copying a DomainPIMPL instance the Domain class
     *  should go through this member and not the copy ctor/assignment operator.
     *
     *  @return A unique_ptr with a deep copy of the derived class.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        copy. Strong throw guarantee.
     *
     *  @note Derived classes should override clone_ so that it returns a deep
     *        copy of the derived class.
     */
    std::unique_ptr<my_type> clone() const { return clone_(); }

    /** @brief Determines if the specified index is in the domain or not.
     *
     *  Domains act like sets thus a given index is either in the Domain or not.
     *  The count function can be used to determine if a specific index is in
     *  the Domain or not.
     *
     * @param[in] idx The index whose membership in the domain is in question.
     *
     * @return True if the index is in the domain and false otherwise.
     *
     * @throw None No throw guarantee.
     */
    bool count(const_reference idx) const noexcept;

    /** @brief The rank of the indices in the Domain.
     *
     *  Domains are sets of indices. Each index is made up of @f$N@f$ components
     *  where @f$N@f$ is its rank. All the indices in a Domain must have the
     *  same rank. This function can be used to inquire as to what that rank is.
     *
     *  @note In order to be no-throw this function will return 0 if there are
     *        no indices in the domain. Hence a return of 0 does not necessarily
     *        mean that the domain contains a rank 0 index.
     *
     *  @return The rank of the indices in this domain.
     *
     *  @throw None No throw guarantee.
     */
    size_type rank() const noexcept;

    /** @brief Returns the number of indices in the Domain.
     *
     *  This function is used to determine the number of indices in the Domain.
     *  This is not the size (i.e., rank) of the indices in this Domain.
     *
     *  @return The number of indices in this Domain.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_domain_.size(); }

    /** @brief Returns the @p i -th index in the Domain
     *
     *  Domains are ordered meaning that any given time it makes sense to speak
     *  of say the zeroth, first, second, etc. index. That said the order can
     *  change upon insertion of a new index as the indices are also sorted.
     *  This function is used to retrieve a read-only reference to the @p i -th
     *  index.
     *
     *  @param[in] i The offset of the requested index. @p i must be in the
     *               range [0, size()).
     *
     *  @return A read-only reference to the @p i-th index.
     *  @throw std::out_of_range if @p i is not in the range [0, size()). Strong
     *                           throw guarantee.
     */
    const_reference at(size_type i) const;

    /** @brief Adds @p idx to the Domain
     *
     *  @param[in] idx The index we are adding to the Domain.
     *
     *  Wthrow std::runtime_error If the rank of the index does not match the
     *                            rank of the indices already in the Domain.
     *                            Strong throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to store the
     *                        index. Strong throw guarantee.
     */
    void insert(value_type idx);

    /** @brief Compares two DomainPIMPL instances for exact equality.
     *
     *  Two DomainPIMPL instances are equal if they contain the same set of
     *  indices, regardless of whether the individual indices are actually
     *  stored in memory.
     *
     *  @param[in] rhs The DomainPIMPL on the right side of the operator.
     *
     *  @return True if the two instances contain the same indices and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const my_type& rhs) const noexcept;

    /** @brief Computes the hash of the Domain.
     *
     *  @param[in,out] h The object computing the hash. After this call the
     *                   internal state will be updated with the hash of the
     *                   Domain's state.
     */
    void hash(sde::Hasher& h) const { h(m_domain_); }
private:
    /// Should be overridden by derived class to make a polymorphic clone
    virtual std::unique_ptr<my_type> clone_() const;

    /// Ensures that @p i is in the range [0, size())
    void bounds_check_(size_type i) const;

    /// The instance actually holding the state.
    std::set<value_type> m_domain_;
}; // class DomainPIMPL

/** @brief Determines if two DomainPIMPL instances are different
 *
 *  Two DomainPIMPL instances are equal if they contain the same set of
 *  indices, regardless of whether the individual indices are actually
 *  stored in memory.
 *
 *  @tparam IndexType The type of index in the domain.
 *
 *  @param[in] lhs The DomainPIMPL on the left side of the operator
 *  @param[in] rhs The DomainPIMPL on the right side of the operator
 *
 *  @return True if the two instances are different and false otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename IndexType>
bool operator!=(const DomainPIMPL<IndexType>& lhs,
                const DomainPIMPL<IndexType>& rhs) {
    return !(lhs == rhs);
}

//------------------------------------------------------------------------------
// Inline implementations
//------------------------------------------------------------------------------

/// Macro of DomainPIMPL with template parameters in order to simplify defs
#define DOMAINPIMPL DomainPIMPL<IndexType>

template<typename IndexType>
bool DOMAINPIMPL::count(const_reference idx) const noexcept {
    return m_domain_.count(idx);
}

template<typename IndexType>
typename DOMAINPIMPL::size_type DOMAINPIMPL::rank() const noexcept {
    return m_domain_.empty() ? 0 : m_domain_.begin()->size();
}

template<typename IndexType>
typename DOMAINPIMPL::const_reference DOMAINPIMPL::at(size_type i) const {
    bounds_check_(i);
    auto itr = m_domain_.begin();
    std::advance(itr, i);
    return *itr;
}

template<typename IndexType>
void DOMAINPIMPL::insert(value_type idx) {
    if(!m_domain_.empty() && idx.size() != rank()) {
        using namespace std::string_literals;
        throw std::runtime_error(
          "Rank of idx ("s + std::to_string(idx.size()) +
          ") != rank of domain ("s + std::to_string(rank()) + ")"s);
    }
    m_domain_.insert(idx);
}

template<typename IndexType>
bool DOMAINPIMPL::operator==(const DomainPIMPL& rhs) const noexcept {
    if(rank() != rhs.rank()) return false;
    if(size() != rhs.size()) return false;
    return m_domain_ == rhs.m_domain_;
}

template<typename IndexType>
std::unique_ptr<DOMAINPIMPL> DOMAINPIMPL::clone_() const {
    return std::make_unique<my_type>(*this);
}

template<typename IndexType>
void DOMAINPIMPL::bounds_check_(size_type i) const{
    if(i < size()) return;
    using namespace std::string_literals;
    throw std::out_of_range(
      "i = "s + std::to_string(i) + " is not in the range [0, "s +
      std::to_string(size()) + ")."s);

}

#undef DOMAINPIMPL

} // namespace libchemist::sparse_map::detail_
