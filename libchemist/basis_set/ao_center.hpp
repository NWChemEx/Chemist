#pragma once
#include <utilities/iterators/random_access_iterator_base

namespace libchemist::basis {

template<typename ValueType>
class AOCenterIterator : public utilities::iterators::RandomAccessIteratorBase<
                           AOCenterIterator<ValueType>, ValueType> {
private:
    using my_type = AOCenterIterator<ValueType>;
    using base_type =
      utilities::iterators::RandomAccessIteratorBase<my_type, ValueType>;
    using difference_type = typename base_type::difference_type;
    using reference       = typename

      my_type&
      increment() override {
        ++m_offset_;
        return *this;
    }

    const ValueType& dereference() { return m_basis_.at(m_offset_); }

    bool are_equal(const my_type& rhs) const noexcept override {
        return std::tie(m_begin_, m_end_, m_offset_) ==
               std::tie(rhs.m_begin_, rhs.m_end_, rhs.m_offset_);
    }

    my_type& decrement() override {
        --m_offset_;
        return *this;
    }

    my_type& advance(difference_type n) {
        m_offset_ += n;
        return *this;
    }

    base_t::DifferenceType distance_to(const AOCenterIterator& rhs) const {
        return rhs.m_offset_ - m_offset_;
    }

    type::size m_begin_  = 0;
    type::size m_end_    = 0;
    type::size m_offset_ = 0;
};

class AOCenter {
public:
    using const_iterator = AOCenterIterator<const AOShell>;
    AOShell& at(type::size i) const;
    type::size begin();
    end();

private:
    AOCenterIterator m_begin_;
    AOCenterIterator m_end_;
};

template<typename BasisSet>
class AOCenterViewPIMPL {
    template<typename T>
    AOCenterViewPIMPL(T&& bs) : m_bs_(std::forward<T>(bs)) {}

public:
    void group_centers_();
    std::reference_wrapper<BasisSet> m_bs_;
};

void group_centers_() {
    for(auto& shell : *m_bs_) {}
}

class AOCenterView {
public:
    AOCenterView(AOBasisSet&)
};

} // namespace libchemist::basis
