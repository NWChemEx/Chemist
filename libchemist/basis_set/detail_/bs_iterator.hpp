#pragma once
#include "libchemist/types.hpp"
#include <utilities/iterators/random_access_iterator_base.hpp>
#include <utilities/mathematician/integer_utils.hpp>

namespace libchemist::basis_set::detail_ {
template<typename ValueType, typename ParentType>
class BSIterator : public utilities::iterators::RandomAccessIteratorBase<
                     BSIterator<ValueType, ParentType>, ValueType> {
public:
    using my_type         = BSIterator<ValueType, ParentType>;
    using difference_type = typename my_type::difference_type;

    BSIterator(type::size me, ParentType parent) noexcept :
      m_index_(me),
      m_parent_(parent),
      m_value_((*parent)[me]) {}

    my_type& advance(difference_type i) override {
        m_index_ += i;
        m_value_ = (*m_parent_)[m_index_];
        return *this;
    }

    difference_type distance_to(const my_type& rhs) const override {
        return utilities::UnsignedSubtract(rhs.m_index_, m_index_);
    }

    ValueType& dereference() override { return m_value_; }

    bool are_equal(const my_type& rhs) const noexcept override {
        return m_index_ == rhs.m_index_ && (*m_parent_) == (*rhs.m_parent_);
    }

    type::size m_index_;
    ParentType m_parent_;
    ValueType m_value_;
};

template<typename ParentType>
auto make_bs_iterator(type::size index, ParentType&& parent) {
    using value_type  = decltype(parent->operator[](index));
    using parent_type = std::remove_reference_t<ParentType>;
    return BSIterator<value_type, parent_type>(
      index, std::forward<ParentType>(parent));
}

} // namespace libchemist::basis_set::detail_
