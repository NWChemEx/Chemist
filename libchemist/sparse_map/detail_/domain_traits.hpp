#pragma once
#include <vector>

namespace libchemist::sparse_map::detail_ {

template<typename IndexType>
struct DomainTraits {
    using size_type       = std::size_t;
    using value_type      = IndexType;
    using const_reference = const IndexType&;
};


}
