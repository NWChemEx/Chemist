#pragma once
#include "chemist/ta_helpers/einsum/types.hpp"

namespace chemist::ta_helpers::einsum::detail_ {

/** @brief Splits a string index on `,` characters.
 *
 *  Admittedly TiledArray has support for this via the VariableList class;
 *  however, it won't allow one to specify the same index in the same tensor
 *  (*e.g.*, `T("i,i")` for a trace). This function removes that restriction.
 *
 *  @param[in] idx The string representation of the indices. Individual indices
 *                 are assumed to be delimited by commas.
 *
 *  @return An `std::vector` of the indices such that the 0-th element of the
 *          vector is the 0-th index in @p idx.
 */
types::index_set parse_index(std::string idx);

inline auto initial_index(const types::assoc_range& ranges) {
    types::assoc_index index;
    for(auto&& [k, v] : ranges) index[k] = v.first;
    return index;
}

/** @brief Increments the provided index subject to the provided ranges
 *
 *  This function will take an associative index (map from string indices to
 *  integral values) and update it using the provided associative ranges (map
 *  from a string index to a pair of integral values such that the first value
 *  of the pair is the beginning of the range and the second value of the pair
 *  is the just past the end of the range). Indices are updated in column-major
 *  order (the first index runs fast).
 *
 *  @param[in,out] idx The index to increment. It will be incremented in place
 *                     (i.e., the value will be mutated). It will be reset to
 *                     the first value if iteration has completed.
 *  @param[in] ranges  The range each index spans. Each range is assumed to be
 *                     half-open such that the beginning value is in the range,
 *                     but the ending value is just outside the range.
 *  @return `true` if iteration has completed and `false` otherwise.
 */
bool increment_index(types::assoc_index& idx, const types::assoc_range& ranges);

} // namespace chemist::ta_helpers::einsum::detail_