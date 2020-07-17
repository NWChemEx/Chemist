#include "libchemist/sparse_map/detail_/from_sparse_map_helpers.hpp"

namespace libchemist::sparse_map::detail_ {

TA::Range extract_dep_range_(const TA::Range& range,
                             const ToTMapper& mapper) {
    using subrange_t = std::decay_t<decltype(range.dim(0))>;
    using size_type  = typename ToTMapper::size_type;

    std::vector<subrange_t> buffer;
    buffer.reserve(mapper.t_dep_rank());
    for(size_type i = 0; i < range.rank(); ++i)
        if(mapper.is_dep_mode(i)) buffer.push_back(range.dim(i));
    return TA::Range(buffer);
}

} // namespace libchemist::sparse_map::detail_
