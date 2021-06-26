#pragma once
#include "libchemist/set_theory/set_traits.hpp"
#include <boost/container/flat_set.hpp>
namespace libchemist::set_theory {
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
 *  @note Using a Subset instance after the parent FamilyOfSets goes out of
 *        scope is undefined behavior and will likely result in a segfault.
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
    /// Type of the object which this is a view/subset of
    using parent_type = FamilyOfSets<SetType>;

    /// Type of the elements in SetType
    using value_type = typename traits_type::value_type;

    /// Type of a read-only reference to an element of a SetType object
    using const_reference = typename traits_type::const_reference;

    /// Type that SetType uses for indexing and offsets
    using size_type = typename traits_type::size_type;

    /// Type of the object this is a view of
    using object_type = SetType;

    /// Type of a read-only reference to the object this is a view of
    using const_obj_ref = const object_type&;

    /** @brief Creates a new, empty, subset of @p parent
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
     *  @throw std::out_of_range if any of the elements in @p il are not in
     *                           @p parent. Weak throw guarantee.
     */
    template<typename ElemType = size_type>
    explicit Subset(const parent_type& parent,
                    std::initializer_list<ElemType> il = {});

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
    const_obj_ref object() const { return m_parent_->object(); }

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

    // my_type operator+(const my_type& rhs) const;
    // my_type operator^(const my_type& rhs) const;
    // my_type operator-(const my_type& rhs) const;

private:
    /// Verifies that the size of object() is greater than @p i
    void obj_bounds_check_(size_type i) const;

    /// Verifies that the size of this subset is greater than @p i
    void set_bounds_check_(size_type i) const;

    /// The indices of the elements of object() in this subset
    boost::container::flat_set<size_type> m_members_;

    /// Pointer to the family of sets we belong to
    const parent_type* m_parent_;
};

// ---------------------------- Implementations --------------------------------

#define SUBSET Subset<SetType>

template<typename SetType>
template<typename ElemType>
SUBSET::Subset(const parent_type& parent, std::initializer_list<ElemType> il) :
  m_parent_(&parent) {
    for(const auto& i : il) insert(i);
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
} // namespace libchemist::set_theory