#pragma once
#include "libchemist/sparse_map/detail_/domain_traits.hpp"
#include "libchemist/sparse_map/index.hpp"
#include <memory>                                  // For smart pointers
#include <sde/sde.hpp>                             // For hashing
#include <utilities/iterators/offset_iterator.hpp> // For iterator

namespace libchemist::sparse_map {

template<typename T> class Domain;

namespace detail_{

template<typename T> class DomainPIMPL;

template<typename DerivedType, typename IndexType>
class DomainBase {
public:
    using value_type = IndexType;
private:
    using my_type     = DomainBase<DerivedType, IndexType>;
    using pimpl_type  = DomainPIMPL<value_type>;
    using pimpl_ptr   = std::unique_ptr<value_type>;
    using traits_type = DomainTraits<value_type>;
public:
    using size_type       = typename traits_type::size_type;

    using const_reference = typename traits_type::const_reference;

    /// Type of a bidirectional iterator with read/write access to indices
    using iterator = utilities::iterators::OffsetIterator<my_type>;

    /// Type of a bidirectional iterator with read-only access to indices
    using const_iterator = utilities::iterators::OffsetIterator<const my_type>;

    DomainBase();
    DomainBase(std::initializer_list<value_type> il);

    DomainBase(const DomainBase& rhs);
    DomainBase(DomainBase&& rhs) noexcept;
    DomainBase& operator=(const DomainBase& rhs);
    DomainBase& operator=(DomainBase&& rhs) noexcept;
    ~DomainBase() noexcept;

    void swap(DomainBase& rhs) noexcept { m_pimpl_.swap(rhs.m_pimpl_); }

    auto begin() noexcept { return iterator(0, this); }
    auto begin() const noexcept { return const_iterator(0, this); }
    auto end() noexcept { return iterator(size(), this); }
    auto end() const noexcept { return const_iterator(size(), this); }

    bool empty() const noexcept { return begin() == end(); }
    size_type rank() const;
    size_type size() const noexcept;

    const_reference operator[](size_type i) const;

    void insert(value_type idx);
    bool count(const_reference idx) const noexcept;


    DomainBase& operator*=(const DomainBase& rhs);
    DerivedType operator*(const DerivedType& rhs) const;
    DomainBase& operator+=(const DomainBase& rhs);
    DerivedType operator+(const DerivedType& rhs) const;

    bool operator==(const DomainBase& rhs) const noexcept;

    void hash(sde::Hasher& h) const;
    std::ostream& print(std::ostream& os) const;

protected:
    DerivedType& downcast_() noexcept;
    const DerivedType& downcast_() const noexcept;
    pimpl_type& pimpl_();
    const pimpl_type& pimpl_() const;
private:
    pimpl_ptr m_pimpl_;
};

template<typename DerivedType, typename IndexType>
bool operator!=(const DomainBase<DerivedType, IndexType>& lhs,
                const DomainBase<DerivedType, IndexType>& rhs) {
    return !(lhs == rhs);
}

extern template class DomainBase<Domain<ElementIndex>, ElementIndex>;
extern template class DomainBase<Domain<TileIndex>, TileIndex>;

}} // namespace libhcemist::sparse_map::detail_
