#pragma once
#include <memory>
#include <utilities/iterators/offset_iterator.hpp>>

namespace libchemist::sparse_map {
namespace detail_ {
class DomainPIMPL;
}

class Domain {
public:
    using size_type = std::size_t;
    using value_type = std::vector<size_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = utilities::OffsetIterator<Domain>;
    using const_iterator = utilities::OffsetIterator<const Domain>;

    Domain();
    ~Domain() const noexcept;

    auto begin() noexcept { return iterator(0, this); }
    auto begin() const noexcept { return const_iterator(0, this); }
    auto end() noexcept { return iterator(size(), this); }
    auto end() const noexcept { return const_iterator(size(), this); }

    bool count(const_reference idx) const noexcept;
    size_type size() const noexcept;

    reference operator[](size_type i);
    const_reference operator[](size_type i) const;

    void insert(const_reference idx);

    Domain operator*(const Domain& rhs) const;

private:
    std::unique_ptr<detail_::DomainPIMPL> m_pimpl_;
};

} // namespace libchemist::sparse_map