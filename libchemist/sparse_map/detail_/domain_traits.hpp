#pragma once
#include <utilities/iterators/offset_iterator.hpp> // For iterator
#include <vector>

namespace libchemist::sparse_map{

template<typename IndexType> class Domain;

namespace detail_ {

/** @brief Code factorization for setting the types across the Domain class
 *         family.
 *
 *  The DomainTraits struct is the single source of truth for setting types of
 *  class members for the Domain family of classes. The primary template is only
 *  forward declared requiring users to specialize it for their particular type
 *  of Domain.
 *
 *  @tparam T The class this struct defines the types of.
 */
template<typename T> struct DomainTraits;

/** @brief Partial specialization of DomainTraits to classes of type Domain<T>
 *
 *  @tparam IndexType The type of the indices stored in the Domain class.
 */
template<typename IndexType>
struct DomainTraits<Domain<IndexType>> {

    using size_type       = std::size_t;
    using value_type      = IndexType;

    using const_reference = const IndexType&;

    template<typename T>
    using iterator = utilities::iterators::OffsetIterator<T>;

    template<typename T>
    using const_iterator = utilities::iterators::OffsetIterator<const T>;
}; // struct DomainTraits<Domain<IndexType>>


} // namespace detail_
} // namespace libchemist::sparse_map
