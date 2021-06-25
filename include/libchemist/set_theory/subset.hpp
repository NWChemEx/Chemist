#pragma once
#include "libchemist/set_theory/set_traits.hpp"
#include <boost/container/flat_set.hpp>
namespace libchemist::set_theory {
template<typename SetType>
class FamilyOfSets;

template<typename SetType>
class Subset {
private:
    using my_type     = Subset<SetType>;
    using traits_type = SetTraits<SetType>;

public:
    using parent_type     = FamilyOfSets<SetType>;
    using value_type      = typename traits_type::value_type;
    using const_reference = typename traits_type::const_reference;
    using size_type       = typename traits_type::size_type;
    using object_type     = SetType;
    using obj_reference   = object_type&;
    using const_obj_ref   = const object_type&;

    explicit Subset(parent_type& parent) : m_parent_(parent) {}

    /** @brief The number of elements in this Subset.
     *
     *  @note This is in general **not** the same as the number or elements in
     *        the Subseted object!!!
     */
    size_type size() const noexcept { return m_members_.size(); }
    bool empty() const noexcept { return m_members_.empty(); }

    void insert(size_type i);
    void insert(const_reference elem);

    const_reference at(size_type i) const;
    const_reference operator[](size_type i) const;

    // my_type operator+(const my_type& rhs) const;
    // my_type operator^(const my_type& rhs) const;
    // my_type operator-(const my_type& rhs) const;

    const auto& object() const { return m_parent_.object(); }

private:
    void obj_bounds_check_(size_type i) const;
    void set_bounds_check_(size_type i) const;
    boost::container::flat_set<size_type> m_members_;
    parent_type& m_parent_;
};

// ---------------------------- Implementations --------------------------------

#define SUBSET Subset<SetType>

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
    const auto i_maps_to = m_members_[i];
    return traits_type::get_elem_i(object(), i_maps_to);
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