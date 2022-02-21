#pragma once
#include "chemist/detail_/hashing.hpp"
#include "chemist/set_theory/traits/set_traits.hpp"
#include <algorithm>
#include <boost/container/flat_set.hpp>
#include <iterator>
#include <memory>

namespace chemist::set_theory {
template<typename SetType>
class FamilyOfSets;

/** @brief Models a subset of a set.
 *
 *  Given a set @f$\mathbb{S}@f$, @f$\mathbb{T}@f$ is a subset of
 *  @f$\mathbb{S}@f$ if @f$\mathbb{T}@f$ is a set and if every member of
 *  @f$\mathbb{T}@f$ is also a member of @f$\mathbb{S}@f$. @f$\mathbb{T}@f$ is a
 *  proper subset if at least one member of @f$\mathbb{S}@f$ does not appear in
 *  @f$\mathbb{T}@f$.
 *
 *  Programatically, this class serves as a proxy for an object of type
 *  @p SetType whose members are restricted to being a subset of another
 *  @p SetType instance. This proxy is a view/mask of the @p SetType instance
 *  with limited @p SetType functionality (mainly the ability to determine if
 *  members are in the proxy and the ability to iterate over the members of the
 *  proxy).
 *
 *  @tparam SetType Type of the parent set this Subset instance is a subset of.
 */
template<typename SetType>
class Subset {
private:
    /// Type of an instance of this class
    using my_type = Subset<SetType>;

    /// Type providing the API for interacting with a SetType object
    using traits_type = SetTraits<SetType>;

public:
    /// Type of the elements in SetType
    using value_type = typename traits_type::value_type;

    /// Type of a read-only reference to an element of a SetType object
    using const_reference = typename traits_type::const_reference;

    /// Type that SetType uses for indexing and offsets
    using size_type = typename traits_type::size_type;

    /// Type of the object this is a view of
    using object_type = SetType;

    /// Type of a pointer to the object this is a view of
    using object_ptr = std::shared_ptr<const object_type>;

    /// Type of a read-only reference to the object this is a view of
    using const_obj_ref = const object_type&;

    /** @brief Creates a new subset of @p parent with the elements specified in
     *         the provided initializer list.
     *
     *  This ctor creates a new subset of @p parent. The subset will be empty if
     *  no initializer list is provided. Additional elements can be added by
     *  calling `insert`.
     *
     *  @tparam ElemType The type used to specify elements of @p parent.
     *                   @p ElemType is expected to be implicitly convertible to
     *                   either size_type or to value_type.
     *
     *  @param[in] parent The set that this is a subset of.
     *  @param[in] il The elements to populate the subset with. Defaults to an
     *                empty list, which in turn creates an empty subset.
     *
     *  @throw std::runtime_error if @p parent is a null pointer. Strong throw
     *                            guarantee.
     *  @throw std::out_of_range if any of the elements in @p il are not in
     *                           @p parent. Weak throw guarantee.
     */
    template<typename ElemType = size_type>
    explicit Subset(object_ptr parent, std::initializer_list<ElemType> il = {});

    /** @brief Creates a new subset initialized by the provided iterators.
     *
     *  This ctor is used to initialize the subset given a range of elements
     *  specified by two iterators.
     *
     *  @tparam BeginItr Type of the iterator pointing to the first element to
     *                   be added. Can dereference to either an offset or the
     *                   actual element.
     *  @tparam EndItr   Type of the iterator pointing to just past the last
     *                   element to be added.
     *  @param[in] parent The superset this set is a subset of.
     *  @param[in] b The iterator pointing to the first element to include in
     *               this subset.
     *  @param[in] e The iterator pointing to just past the last element to
     *               include in this subset. It is undefined behavior if @p e is
     *               not reachable by incrementing @p b.
     *
     *  @throw std::runtime_error if @p parent is a null pointer. Strong throw
     *                            guarantee.
     *  @throw std::out_of_range if any of the elements in the provided range
     *                           are not in @p parent. Weak throw guarantee.
     */
    template<typename BeginItr, typename EndItr>
    Subset(object_ptr parent, BeginItr&& b, EndItr&& e);

    /** @brief The number of elements in this Subset.
     *
     *  This function will compute the cardinality (number of elements) of the
     *  subset.
     *
     *  @note This is in general **not** the same as the number or elements in
     *        the Subseted object!!!
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_members_.size(); }

    /** @brief Determines if this subset is empty or not.
     *
     *  This function is a convenience function for determining if the size of
     *  this set is zero.
     *
     *  @return True if this set is empty (zero elements) and false otherwise.
     *
     *  @throw None No throw gurantee.
     */
    bool empty() const noexcept { return m_members_.empty(); }

    /** @brief Returns a read-only iterator pointing to the index of the first
     *         element in the subset.
     *
     *  For computational efficiency the iterators over the subset class run
     *  over the indices of the elements in the subset and not the elements
     *  themselves.
     *
     *  @return An interator pointing to the index of the first element in this
     *          subset.
     *
     *  @throw None No throw guarantee.
     */
    auto begin() const noexcept { return m_members_.begin(); }

    /** @brief Returns a read-only iterator pointing to just past the index of
     *         the last element in the subset.
     *
     *  For computational efficiency the iterators over the subset class run
     *  over the indices of the elements in the subset and not the elements
     *  themselves.
     *
     *  @return An interator pointing to just past the index of the last element
     *          in this subset.
     *
     *  @throw None No throw guarantee.
     */
    auto end() const noexcept { return m_members_.end(); }

    /** @brief Returns an element of this subset by offset.
     *
     *  This function can be used to retrieve an element of the subset by
     *  offset. It behave similar to `operator[]` with the addition of a bounds
     *  check on @p i. If you know that @p i is in bounds it is recommended that
     *  you call `operator[]` instead.
     *
     *  @param[in] i The offset of the requested element. @p i must be in the
     *               range [0, size()).
     *
     *  @return A read-only reference to the requested object.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, size()). Strong
     *                           throw guarantee.
     */
    const_reference at(size_type i) const;

    /** @brief Returns an element of this subset by offset.
     *
     *  This function works similar to `at` except there is no bounds check. If
     *  @p i is out of bounds a seg fault will likely occur. If you are unsure
     *  of whether @p i is in bounds use `at`.
     *
     *  @param[in] i Which of elements in this subset to return. @p i must be in
     *               the range [0,size()); however, no bounds check occurrs.
     *
     *  @return A read-only reference to the @p i -th element in the subset.
     *
     *  @throw None No throw guarantee.
     */
    const_reference operator[](size_type i) const;

    /** @brief Returns the number of times @p elem appears in this subset.
     *
     *  This function allows you to determine how many times the object @p elem
     *  appears in this Subset. Since the @p elem may appear at most once in
     *  this set, the result is either 0 or 1, i.e., true if @p elem is in the
     *  set and false if it is not. No check is made to ensure that @p elem is
     *  actually in the parent set.
     *
     *  @param[in] elem The element we are looking for.
     *
     *  @return True if @p elem is in the set and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool count(const_reference elem) const;

    /** @brief Returns a read-only reference to the parent set.
     *
     *  This accessor can be used to retrieve the parent set that this instance
     *  is a subset of. N.B. this is the object living in the FamilyOfSets not
     *  the FamilyOfSets instance.
     *
     *  @return A read-only reference to the parent set.
     *
     *  @throw None No throw guarantee.
     */
    const_obj_ref object() const { return *m_parent_; }

    /** @brief Inserts an element by offset in the parent set.
     *
     *  This function provides a mechanism for adding elements of the parent set
     *  to this subset by using the offset of the element. The offset is the
     *  offset in the parent set. Repeated insertions of an element do not
     *  change the state of the Subset.
     *
     *  @param[in] i The offset of the element we are inserting. Must be in the
     *               range [0, traits_type::size(object())).
     *
     *  @throw std::out_of_range if @p i is not in the range
     *                           [0,traits_type::size(object())). Strong throw
     *                           guarantee.
     *
     *  @warning Be very careful using insert when the parent set is filled with
     *           objects of type `size_type`. In this case the only distinction
     *           between insertion by offset and insertion by value is whether
     *           or not the argument is a reference or value. This is
     *           particularly tricky given implicit conversion rules. That all
     *           said using the FamilyOfSets/Subset classes for a container
     *           filled with objects of `size_type` is more costly than actually
     *           having copies on account of the additional indirection. Hence
     *           this is considered a sufficiently rare edge-case that we are
     *           okay with the weird semantics for it.
     */
    void insert(size_type i);

    /** @brief Inserts an element by value.
     *
     *  This function allows you to insert the specified element into the
     *  subset. The element must appear in the parent set. Repeated insertions
     *  of the same element do not change the state of the Subset.
     *
     *  @param[in] elem The element to add to this subset.
     *
     *  @throw std::out_of_range if @p elem is not found in object(). Strong
     *                           throw guarantee.
     *
     *  @warning Be very careful using insert when the parent set is filled with
     *           objects of type `size_type`. In this case the only distinction
     *           between insertion by offset and insertion by value is whether
     *           or not the argument is a reference or value. This is
     *           particularly tricky given implicit conversion rules. That all
     *           said using the FamilyOfSets/Subset classes for a container
     *           filled with objects of `size_type` is more costly than actually
     *           having copies on account of the additional indirection. Hence
     *           this is considered a sufficiently rare edge-case that we are
     *           okay with the weird semantics for it.
     */
    void insert(const_reference elem);

    /** @brief Makes this instance the union of itself and @p rhs.
     *
     *  The union of two sets @f$A@f$ and @f$B@f$ is a set @f$C@f$ such that
     *  every element in @f$A@f$ and @f$B@f$ is also in @f$C@f$. This function
     *  will overwrite the current subset's state with the union of itself and
     *  @p rhs.
     *
     *  @param[in] rhs The Subset we are taking a union with.
     *
     *  @return The current instance after taking the union with @p rhs.
     *
     *  @throws std::runtime_error if @p rhs has a different parent set. Strong
     *                             throw guarantee.
     *  @throws std::bad_alloc if memory allocation fails. Weak throw guarantee.
     */
    my_type& operator+=(const my_type& rhs);

    /** @brief Returns the union of this instance with @p rhs.
     *
     *  The union of two sets @f$A@f$ and @f$B@f$ is a set @f$C@f$ such that
     *  every element in @f$A@f$ and @f$B@f$ is also in @f$C@f$. This function
     *  will compute the union of the current subset with @p rhs and return it.
     *
     *  @param[in] rhs The instance we are taking the union with.
     *
     *  @return The union of this instance with @p rhs.
     *
     *  @throw std::runtime_error if @p rhs has a different parent set than the
     *                            current instance. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating memory. Weak
     *                        throw guarantee.
     */
    my_type operator+(const my_type& rhs) const;

    /** @brief Removes @p rhs from the current set.
     *
     *  The set difference of two sets, @f$A@f$ and @f$B@f$, is a set @f$C@f$
     *  which contains all of the elements in @f$A@f$ that are not in @f$B@f$.
     *  This function will set the current instance to the set difference of
     *  itself with @p rhs.
     *
     *  @return The current subset after setting it to the set difference with
     *          @p rhs.
     *
     *  @param[in] rhs The set we are taking the set difference with.
     *
     *  @throw std::runtime_error if @p rhs has a different parent set. Strong
     *                            throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating memory. Strong
     *                        throw guarantee.
     */
    my_type& operator-=(const my_type& rhs);

    /** @brief Returns the set difference of this with @p rhs.
     *
     *  The set difference of two sets, @f$A@f$ and @f$B@f$, is a set @f$C@f$
     *  which contains all of the elements in @f$A@f$ that are not in @f$B@f$.
     *  This function will return the set difference of the current instance
     *  with @p rhs.
     *
     *  @return The current subset after setting it to the set difference with
     *          @p rhs.
     *
     *  @param[in] rhs The set we are taking the set difference with.
     *
     *  @throw std::runtime_error if @p rhs has a different parent set. Strong
     *                            throw guarantee.
     *  @throw std::bad_alloc if a memory allocation problem occurs. Strong
     *                        throw guarantee.
     */
    my_type operator-(const my_type& rhs) const;

    /** @brief Sets this to the intersection of itself with @p rhs.
     *
     *  The intersction of two sets, @f$A@f$ and @f$B@f$, is the set @f$C@f$
     *  containing all elements that appear in both @f$A@f$ and @f$B@f$. This
     *  function will set the current instance to the intersection of itself
     *  with @p rhs.
     *
     *  @param[in] rhs The set we are taking the intersection with.
     *
     *  @return The current instance after setting it to the intersection of
     *          itself with @p rhs.
     *
     *  @throw std::runtime_error if @p rhs has a different parent set. Strong
     *                            throw guarantee.
     *  @throw std::bad_alloc if a memory allocation problem occurs. Strong
     *                        throw guarantee.
     */
    my_type& operator^=(const my_type& rhs);

    /** @brief Returns the intersection of this with @p rhs.
     *
     *  The intersction of two sets, @f$A@f$ and @f$B@f$, is the set @f$C@f$
     *  containing all elements that appear in both @f$A@f$ and @f$B@f$. This
     *  function will return the intersection of itself with @p rhs.
     *
     *  @param[in] rhs The set we are taking the intersection with.
     *
     *  @return The intersection of this with @p rhs.
     *
     *  @throw std::runtime_error if @p rhs has a different parent set. Strong
     *                            throw guarantee.
     *  @throw std::bad_alloc if a memory allocation problem occurs. Strong
     *                        throw guarantee.
     */
    my_type operator^(const my_type& rhs) const;

    /** @brief Determines if this Subset comes before @p rhs lexicographically.
     *
     *  This function will compare the current subset to @p rhs and determine
     *  if it comes before it. The comparison is done on the element's offsets
     *  and not the elements themselves. That is to say the set `{0, 1}` comes
     *  before the set `{0, 2}` even if the `object()[1]` comes after
     *  `object()[2]` when sorting the objects.
     *
     *  In order to be no-throw this function assumes that the Subset instances
     *  are subsets of the same superset.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if this instance is lexicographically ordered before @p rhs
     *          and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator<(const my_type& rhs) const noexcept;

    /** @brief Hashes the current subset.
     *
     *  @param[in,out] h The hasher being used to hash this Subset. After the
     *                   call, the internal hash of @p h will be updated to
     *                   include the hash of this Subset.
     */
    void hash(chemist::detail_::Hasher& h) const;

private:
    /// Type of the container holding the set
    using set_type = boost::container::flat_set<size_type>;

    /// Verifies that the size of object() is greater than @p i
    void obj_bounds_check_(size_type i) const;

    /// Verifies that the size of this subset is greater than @p i
    void set_bounds_check_(size_type i) const;

    /// The indices of the elements of object() in this subset
    set_type m_members_;

    /// Pointer to the family of sets we belong to
    object_ptr m_parent_;
};

/** @brief Compares two Subset instances for equality.
 *
 *  @relates Subset
 *
 *  Two Subset instances are equal if they are subsets of the same set and if
 *  they contain the same elements. In particular this does not place any
 *  restriction on what family of sets they belong to.
 *
 *  @tparam LHSSetType The type of the set @p lhs is drawn from.
 *  @tparam RHSSetType The type of the set @p rhs is drawn from.
 *
 *  @param[in] lhs The set on the left side of the equality operator.
 *  @param[in] rhs The set on the right side of the equality operator.
 *
 *  @return True if @p lhs is equal to @p rhs and false otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename LHSSetType, typename RHSSetType>
bool operator==(const Subset<LHSSetType>& lhs, const Subset<RHSSetType>& rhs) {
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>) {
        return false;
    } else {
        if(lhs.object() != rhs.object()) return false;
        if(lhs.size() != rhs.size()) return false;
        using size_type = typename Subset<LHSSetType>::size_type;
        for(size_type i = 0; i < lhs.size(); ++i)
            if(lhs[i] != rhs[i]) return false;
        return true;
    }
}

/** @brief Determines if two Subset instances are different.
 *
 *  @relates Subset
 *
 *  Two Subset instances are equal if they are subsets of the same set and
 * if they contain the same elements. In particular this does not place any
 *  restriction on what family of sets they belong to.
 *
 *  @tparam LHSSetType The type of the set @p lhs is drawn from.
 *  @tparam RHSSetType The type of the set @p rhs is drawn from.
 *
 *  @param[in] lhs The instance on the left side of the not-equal operator
 *  @param[in] rhs The instance on the right side of the not-equal operator
 *
 *  @return False if @p lhs is equal to @p rhs and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename LHSSetType, typename RHSSetType>
bool operator!=(const Subset<LHSSetType>& lhs, const Subset<RHSSetType>& rhs) {
    return !(lhs == rhs);
}

template<typename SetType>
std::ostream& operator<<(std::ostream& os, const Subset<SetType>& ss) {
    os << "{";
    for(const auto& x : ss) os << x << " ";
    return os << "}";
}

// ---------------------------- Implementations --------------------------------

#define SUBSET Subset<SetType>

template<typename SetType>
template<typename ElemType>
SUBSET::Subset(object_ptr parent, std::initializer_list<ElemType> il) :
  m_parent_(parent) {
    if(!m_parent_)
        throw std::runtime_error("Attempting to initialize with null ptr");
    for(const auto& i : il) insert(i);
}

template<typename SetType>
template<typename BeginItr, typename EndItr>
SUBSET::Subset(object_ptr parent, BeginItr&& b, EndItr&& e) :
  m_parent_(parent) {
    if(!m_parent_)
        throw std::runtime_error("Attempting to initialize with null ptr");
    while(b != e) {
        insert(*b);
        ++b;
    }
}

template<typename SetType>
void SUBSET::insert(size_type i) {
    obj_bounds_check_(i);
    m_members_.insert(i);
}

template<typename SetType>
void SUBSET::insert(const_reference elem) {
    // Don't call Subset::insert(size_type) or we repeat the bounds check
    m_members_.insert(traits_type::get_index(object(), elem));
}

template<typename SetType>
typename SUBSET::const_reference SUBSET::at(size_type i) const {
    set_bounds_check_(i);
    return operator[](i);
}

template<typename SetType>
typename SUBSET::const_reference SUBSET::operator[](size_type i) const {
    const auto i_maps_to = *(m_members_.begin() + i);
    return traits_type::get_elem_i(object(), i_maps_to);
}

template<typename SetType>
bool SUBSET::count(const_reference elem) const {
    for(size_type i = 0; i < size(); ++i)
        if((*this)[i] == elem) return true;
    return false;
}

template<typename SetType>
SUBSET& SUBSET::operator+=(const Subset& rhs) {
    if(object() != rhs.object())
        throw std::runtime_error("Subsets are not part of the same superset.");
    m_members_.insert(rhs.m_members_.begin(), rhs.m_members_.end());
    return *this;
}

template<typename SetType>
SUBSET SUBSET::operator+(const Subset& rhs) const {
    return Subset(*this) += rhs;
}

template<typename SetType>
SUBSET& SUBSET::operator-=(const Subset& rhs) {
    if(object() != rhs.object())
        throw std::runtime_error("Subsets are not part of the same superset.");

    auto lbegin = m_members_.begin();
    auto lend   = m_members_.end();
    auto rbegin = rhs.m_members_.begin();
    auto rend   = rhs.m_members_.end();
    set_type result;
    std::insert_iterator<set_type> out_itr(result, result.begin());
    std::set_difference(lbegin, lend, rbegin, rend, out_itr);
    m_members_.swap(result);
    return *this;
}

template<typename SetType>
SUBSET SUBSET::operator-(const Subset& rhs) const {
    return Subset(*this) -= rhs;
}

template<typename SetType>
SUBSET& SUBSET::operator^=(const Subset& rhs) {
    if(object() != rhs.object())
        throw std::runtime_error("Subsets are not part of the same superset.");

    auto lbegin = m_members_.begin();
    auto lend   = m_members_.end();
    auto rbegin = rhs.m_members_.begin();
    auto rend   = rhs.m_members_.end();
    set_type result;
    std::insert_iterator<set_type> out_itr(result, result.begin());
    std::set_intersection(lbegin, lend, rbegin, rend, out_itr);
    m_members_.swap(result);
    return *this;
}

template<typename SetType>
SUBSET SUBSET::operator^(const Subset& rhs) const {
    return Subset(*this) ^= rhs;
}

template<typename SetType>
bool SUBSET::operator<(const Subset& rhs) const noexcept {
    if(object() != rhs.object()) return false;
    return m_members_ < rhs.m_members_;
}

template<typename SetType>
void SUBSET::hash(chemist::detail_::Hasher& h) const {
    for(const auto& x : m_members_) h(x);
    h(*m_parent_);
}

template<typename SetType>
void SUBSET::obj_bounds_check_(size_type i) const {
    const auto nelems = traits_type::size(object());
    if(i < nelems) return;
    throw std::out_of_range("Element index " + std::to_string(i) +
                            " is not in " + "range for parent object with " +
                            std::to_string(nelems) + " elements.");
}

template<typename SetType>
void SUBSET::set_bounds_check_(size_type i) const {
    if(i < size()) return;
    throw std::out_of_range("Element index " + std::to_string(i) +
                            " is not in " + "range for Subset with " +
                            std::to_string(size()) + " elements.");
}

#undef Subset
} // namespace chemist::set_theory
