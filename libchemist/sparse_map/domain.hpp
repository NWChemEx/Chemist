#pragma once
#include <memory>                                  // For smart pointers
#include <sde/sde.hpp>                             // For hashing
#include <utilities/iterators/offset_iterator.hpp> // For iterator

namespace libchemist::sparse_map {
namespace detail_ {
class DomainPIMPL;
}

/** @brief A set of tile coordinate indices.
 *
 *  Practically speaking a domain collects the dependent tile indices associated
 *  with a particular independent element index. The SparseMap class manages
 *  the mapping of independent element indices to Domain instances.
 *
 *  @note Nothing about the machinery of the Domain class actually assumes that
 *        the values in the Domain are coordinate indices of tiles. The Domain
 *        class simply assumes it is managing indices.
 */
class Domain {
public:
    /// An unsigned integral type used for offsets and counting
    using size_type = std::size_t;

    /// The type the indices are stored as
    using value_type = std::vector<size_type>;

    /// The type of a reference to an index. Note indices are immutable.
    using reference = const value_type&;

    /// Type of a read-only reference to an index.
    using const_reference = const value_type&;

    /// Type of a bidirectional iterator
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

    /// The PIMPL which actually contains the data/implements core functionality
    std::unique_ptr<detail_::DomainPIMPL> m_pimpl_;
}; // class Domain

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
