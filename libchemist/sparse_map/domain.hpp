#pragma once
#include <memory>                                  // For smart pointers
#include <sde/sde.hpp>                             // For hashing
#include <utilities/iterators/offset_iterator.hpp> // For iterator

namespace libchemist::sparse_map {
namespace detail_ {
class DomainPIMPL;
}

class Domain {
public:
    using size_type = std::size_t;
    using value_type = std::vector<size_type>;
    using reference = const value_type&;
    using const_reference = const value_type&;
    using iterator = utilities::iterators::OffsetIterator<Domain>;
    using const_iterator = utilities::iterators::OffsetIterator<const Domain>;

    Domain();
    Domain(const Domain& rhs);
    Domain(Domain&& rhs) noexcept;
    Domain& operator=(const Domain& rhs);
    Domain& operator=(Domain&& rhs) noexcept;
    Domain(std::initializer_list<value_type> il);
    ~Domain() noexcept;

    void swap(Domain& rhs) noexcept { m_pimpl_.swap(rhs.m_pimpl_); }

    auto begin() noexcept { return iterator(0, this); }
    auto begin() const noexcept { return const_iterator(0, this); }
    auto end() noexcept { return iterator(size(), this); }
    auto end() const noexcept { return const_iterator(size(), this); }

    bool count(const_reference idx) const noexcept;
    bool empty() const noexcept { return begin() == end(); }
    size_type rank() const;
    size_type size() const noexcept;

    reference operator[](size_type i);
    const_reference operator[](size_type i) const;

    void insert(const_reference idx);

    Domain& operator*=(const Domain& rhs);
    Domain operator*(const Domain& rhs) const;
    Domain& operator+=(const Domain& rhs);
    Domain operator+(const Domain& rhs) const;

    bool operator==(const Domain& rhs) const noexcept;
    bool operator!=(const Domain& rhs) const noexcept;

    void hash(sde::Hasher& h) const;
    std::ostream& print(std::ostream& os) const;
private:

    /** @brief Ensures that the PIMPL is non-null.
     *
     *  For the most part it is not possible for the PIMPL to be null as even
     *  the default ctor initializes one; however, in order to ensure that the
     *  move ctor/assignment operator are nothrow we can't allocate a new PIMPL
     *  for the moved-from instance. Hence a moved-from Domain instance, will
     *  contain a null pointer until it is provided with a new PIMPL. This
     *  function checks to see if the PIMPL is null and raises an error if it
     *  is.
     *
     *  @throw std::runtime_error if the PIMPL is null. Strong throw guarantee.
     */
    void check_pimpl_() const;

    std::unique_ptr<detail_::DomainPIMPL> m_pimpl_;
};

inline std::ostream& operator<<(std::ostream& os, const Domain& d){
    return d.print(os);
}

//------------------------------------------------------------------------------
// Inline Implementations
//------------------------------------------------------------------------------

inline Domain::Domain(std::initializer_list<value_type> il) : Domain() {
    for(auto&& x : il)insert(x);
}

inline bool Domain::operator!=(const Domain& rhs) const noexcept {
    return !((*this) == rhs);
}

} // namespace libchemist::sparse_map