#pragma once
#include "libchemist/partitioning/partitioned_object_traits.hpp"
#include <boost/container/flat_set.hpp>
namespace libchemist::partition {
template<typename Obj2Partition>
class Partitioning;

template<typename Obj2Partition>
class Partition {
private:
    using traits_type = PartitionedObjectTraits<Obj2Partition>;

public:
    using partitioning_type = Partitioning<Obj2Partition>;
    using value_type        = typename traits_type::value_type;
    using const_reference   = typename traits_type::const_reference;
    using size_type         = typename traits_type::size_type;
    using object_type       = Obj2Partition;
    using obj_reference     = object_type&;
    using const_obj_ref     = const object_type&;

    explicit Partition(partitioning_type& parent) : m_partitioning_(parent) {}

    /** @brief The number of elements in this Partition.
     *
     *  @note This is in general **not** the same as the number or elements in
     *        the partitioned object!!!
     */
    size_type size() const noexcept { return m_members_.size(); }

    void insert(size_type i) { m_members_.insert(i); }
    void insert(const_reference_type elem);
    const_reference operator[](size_type i) const;

    const auto& object() const { return m_partitioning_.object(); }

private:
    boost::container::flat_set<size_type> m_members_;
    partitioning_type& m_partitioning_;
};

// ---------------------------- Implementations --------------------------------

#define PARTITION Partition<Obj2Partition>

template<typename Obj2Partition>

template<typename Obj2Partition>
typename PARTITION::const_reference PARTITION::operator[](size_type i) const {
    const auto i_maps_to = m_members_[i];
    return traits_type::get_elem_i(object(), i_maps_to);
}

#undef PARTITION
} // namespace libchemist::partition