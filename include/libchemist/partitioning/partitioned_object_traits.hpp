#pragma once

namespace libchemist::partitioning {

template<typename T>
class Partition;

/** @brief Defines the interface that the Partition and Partitioning classes use
 *         to interface with the partitioned object.
 *
 *  The primary template assumes that the object to partition has publically
 *  accessible:
 *  - type `size_type` defining the type the object uses for offsets and
 *    indexing.
 *  - type `value_type` defining the type of the elements in the object
 *  - member `size()` which can be used to retrieve the number of elements in
 *    the object.
 *  - member `operator[](size_type)` which can be used to retrieve the an
 *    element of the object by offset
 *
 *  If any of the above is not true then using an object of type `Obj2Partition`
 *  with the Partition/Partitioning classes requires specializing
 *  `PartitionedObjectTraits` for type `Obj2Partition`.
 */
template<typename Obj2Partition>
struct PartitionedObjectTraits {
    using size_type       = typename Obj2Partition::size_type;
    using value_type      = typename Obj2Partition::value_type;
    using reference_type  = value_type&;
    using const_reference = const value_type&;
    using partition_type  = Partition<Obj2Partition>;

    static size_type size(const Obj2Partition& obj) { return obj.size(); }

    static const_reference get_elem_i(const Obj2Partition& obj, size_type i) {
        if(i < size(obj)) return obj[i];

        throw std::out_of_range("Index " + std::to_string(i) +
                                " is out of range for object containing " +
                                std::to_string(size(obj)) + " elements");
    }

    static size_type get_index(const Obj2Partition& obj, const_reference elem) {
        for(size_type i = 0; i < size(obj); ++i)
            if(get_elem_i(obj, i) == elem) return i;
        throw std::out_of_range("Object does not contain element");
    }
};

} // namespace libchemist::partitioning