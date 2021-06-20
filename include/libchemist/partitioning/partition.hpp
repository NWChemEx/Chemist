#pragma once
#include "libchemist/partitioning/partitioned_object_traits.hpp"
#include <boost/container/flat_set.hpp>
namespace libchemist::partition {
template<typename Obj2Partition>
class Partitioning;

template<typename Obj2Partition>
class Partition {
private:
    using my_type     = Partition<Obj2Partition>;
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
    bool empty() const noexcept { return m_members_.empty(); }

    void insert(size_type i) { m_members_.insert(i); }
    void insert(const_reference elem);

    const_reference at(size_type i) const;
    const_reference operator[](size_type i) const;

    my_type operator+(const my_type& rhs) const;
    my_type operator^(const my_type& rhs) const;
    my_type operator-(const my_type& rhs) const;

    const auto& object() const { return m_partitioning_.object(); }

private:
    void obj_bounds_check_(size_type i) const;
    void set_bounds_check_(size_type i) const;
    boost::container::flat_set<size_type> m_members_;
    partitioning_type& m_partitioning_;
};

// ---------------------------- Implementations --------------------------------

#define PARTITION Partition<Obj2Partition>

template<typename Obj2Partition>
void PARTITION::insert(size_type i) {
    obj_bounds_check_(i);
    m_members_.insert(i);
}

template<typename Obj2Partition>
void PARTITION::insert(const_reference elem) {
    m_members_.insert(traits_type::get_index(object(), elem));
}

template<typename Obj2Partition>
typename PARTITION::const_reference PARTITION::at(size_type i) const {
    set_bounds_check_(i);
    return operator[](i);
}

template<typename Obj2Partition>
typename PARTITION::const_reference PARTITION::operator[](size_type i) const {
    const auto i_maps_to = m_members_[i];
    return traits_type::get_elem_i(object(), i_maps_to);
}

template<typename Obj2Partition>
void PARTITION::obj_bounds_check_(size_type i) const {
    const auto nelems = traits_type::size(object());
    if(i < nelems) return;
    throw std::out_of_range("Element index " + std::to_string(i) +
                            " is not in " + "range for parent object with " +
                            std::to_string(nelems) + " elements.");
}

template<typename Obj2Partition>
void PARTITION::set_bounds_check_(size_type i) const {
    if(i < size()) return;
    throw std::out_of_range("Element index " + std::to_string(i) +
                            " is not in " + "range for partition with " +
                            std::to_string(size()) + " elements.");
}

#undef PARTITION
} // namespace libchemist::partition