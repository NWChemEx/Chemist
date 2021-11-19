#pragma once
#include <stdexcept>
namespace chemist::set_theory {

template<typename T>
class Subset;

/** @brief Defines the interface that the Subset and FamilyOfSets classes use
 *         to interface with the parent object.
 *
 *  The primary template assumes that the parent object has publically
 *  accessible:
 *  - type `size_type` defining the type the object uses for offsets and
 *    indexing.
 *  - type `value_type` defining the type of the elements in the object
 *  - member `size()` which can be used to retrieve the number of elements in
 *    the object.
 *  - member `operator[](size_type)` which can be used to retrieve the an
 *    element of the object by offset
 *  - type `value_type` is comparable via `operator==`.
 *
 *  If any of the above is not true then using an object of type `SetType`
 *  with the Subset/FamilyOfSets classes requires specializing `SetTraits` for
 *  type `SetType`.
 *
 *  @tparam SetType The type of the object being subset-ed. To use the primary
 *                  template definiton @p SetType must satisfy the criteria
 *                  listed in the description. Otherwise use of FamilyOfSets and
 *                  Subset with @p SetType requires specializing SetTraits.
 */
template<typename SetType>
struct SetTraits {
    /// Type that @p SetType uses for indexing and offsets
    using size_type = typename SetType::size_type;

    /// Unqualified type of the elements in a container of type @p SetType
    using value_type = typename SetType::value_type;

    /// Type of a read/write reference to an element in @p SetType
    using reference_type = value_type&;

    /// Type of a read-only reference to an element in @p SetType
    using const_reference = const value_type&;

    /// Type of each Subset in the FamilyOfSets
    using subset_type = Subset<SetType>;

    /** @brief Defines the API so Subset/FamilyOfSets can get the number of
     *         elements in the object.
     *
     *  The primary template assumes that @p SetType defines a member
     *  function `size`. This function simply calls that member and returns the
     *  result.
     *
     *  @param[in] obj The object that we want to know the size of.
     *
     *  @return The number of elements in @p obj
     *
     *  @throw ??? Throws if @p SetType throws. Same throw guarantee.
     */
    static size_type size(const SetType& obj) { return obj.size(); }

    /** @brief Defines the API for getting the i-th element in @p obj
     *
     *  This function checks that @p i is less than `size(obj)`, throwing if it
     *  is not, and then defers to @p SetType 's operator[].
     *
     *  @param[in] obj The object we are retrieving an element from.
     *  @param[in] i The offset of the desired element. @p i should be in the
     *               range [0, size(obj)).
     *
     *  @return A read-only reference to the i-th element in @p obj.
     *
     *  @throws std::out_of_range if @p i is not in the range [0,size(obj)).
     *                            Strong throw guarantee.
     *  @throws ??? if @p SetType 's operator[] throws or if size(obj)
     *              throws. In either case, same throw guarantee.
     */
    static const_reference get_elem_i(const SetType& obj, size_type i) {
        if(i < size(obj)) return obj[i];

        throw std::out_of_range("Index " + std::to_string(i) +
                                " is out of range for object containing " +
                                std::to_string(size(obj)) + " elements");
    }

    /** @brief Defines the API for getting the offset of an element @p elem.
     *
     *  This function loops over the elements in @p obj, using `get_elem_i`, and
     *  compares the elements to @p elem. If a match is found the index of the
     *  match is returned. If no match is found an exception is raised.
     *
     *  @param[in] obj The object we are looking for @p elem in.
     *  @param[in] elem The object we are searching @p obj for.
     *
     *  @return The index of @p elem in @p obj.
     *
     *  @throw std::out_of_range if @p elem is not in @p obj. Strong throw
     *                           guarantee.
     *  @throw ??? if `size(obj)`, `get_elem_i(obj,n)` (n less than `size(obj)`,
     *             or the `value_type::operator==` throws. Same throw guarantee.
     */
    static size_type get_index(const SetType& obj, const_reference elem) {
        for(size_type i = 0; i < size(obj); ++i)
            if(get_elem_i(obj, i) == elem) return i;
        throw std::out_of_range("Object does not contain element");
    }
};

} // namespace chemist::set_theory