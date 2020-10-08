#include "libchemist/ta_helpers/einsum/detail_/detail_.hpp"
#include <algorithm> // For replace
#include <iterator>  // For istream_iterator
#include <sstream>   // For istringstream

namespace libchemist::ta_helpers::einsum::detail_ {

types::index_set parse_index(std::string idx) {
    // 1. Remove all existing spaces
    idx.erase(std::remove(idx.begin(), idx.end(), ' '), idx.end());

    // 2. Switch commas to spaces
    std::replace(idx.begin(), idx.end(), ',', ' ');

    // 3. Use stringstream to split on spaces
    std::istringstream iss(idx);

    // 4. Copy stream contents into index_set and return
    using iss_itr = std::istream_iterator<std::string>;
    return types::index_set(iss_itr(iss), iss_itr{});
}

} // namespace libchemist::einsum::detail_