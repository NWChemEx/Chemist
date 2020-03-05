#include "libchemist/ta_helpers/einsum/detail_/detail_.hpp"

namespace libchemist::einsum::detail_ {

bool increment_index(types::assoc_index& idx,
                     const types::assoc_range& ranges) {
    // Loop over indices looking for one we can increment
    for(auto&& [idx_str, idx_val] : idx) {
        auto&& range = ranges.at(idx_str); // Throws if idx_str not in ranges

        if(idx_val + 1 < range.second) { // Can increment this index, so do it
            idx[idx_str] = idx_val + 1;
            return false; // Not done since we could increment this index
        } else {          // Can't increment it, so reset it
            idx[idx_str] = range.first;
        }
    }

    return true; // We apparently just reset every index, so we're done
}

} // namespace libchemist::einsum::detail_