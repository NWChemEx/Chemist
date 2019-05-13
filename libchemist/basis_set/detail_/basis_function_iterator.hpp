#pragma once

#include <utilities/iterators/random_access_iterator_base.hpp>
#include <utilities/mathematician/integer_utils.hpp>

namespace libchemist::basis_set::detail_ {

template<typename T>
class AOBasisFunctionItr
  : public RandomAccessIteratorBase<AOBasisFunctionItr<T>, T> {
    using my_type = AOBasisFunctionItr<T>;

public:
    using difference_type = typename my_type::difference_type;
    using reference       = typename my_type::reference;

    AOBasisFunctionItr(type::size index)

      my_type& advance(DifferenceType n) override {
        m_index_ += n;
        return *this;
    }
    difference_type distance_to(const my_type& rhs) const override;
    reference dereference() override { return (*m_parent_)[m_index_]; }
    bool are_equal(const my_type& rhs) const noexcept override;

private:
    type::size m_index_;
    T* m_parent_;
};

//------------------------Implementations---------------------------------------

template<typename T>
typename AOBasisFunctionItr<T>::difference_type
AOBasisFunctionItr<T>::distance_to(const AOBasisFunctionItr<T>& rhs) const {
    return utilities::UnsignedSubtract<difference_type>(rhs.m_index_, m_index_);
}

template<typename T>
bool AOBasisFunctionItr<T>::are_equal(const AOBasisFunctionItr<T>& rhs) const
  noexcept {
    return std::tie(m_index_, *m_parent_) ==
           std::tie(rhs.m_index_, *rhs.m_parent_);
}

} // namespace libchemist::basis_set::detail_
