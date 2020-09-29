#pragma once
#include <utilities/iterators/offset_iterator.hpp> // For iterator
#include <vector>

namespace libchemist::sparse_map::detail_ {

template<typename IndexType>
struct DomainTraits {
    using size_type       = std::size_t;
    using value_type      = IndexType;
    using const_reference = const IndexType&;
    template<typename T>
    using iterator = utilities::iterators::OffsetIterator<T>;

    template<typename T>
    using const_iterator = utilities::iterators::OffsetIterator<const T>;
};


}
