#pragma once
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace libchemist::ta_helpers::einsum::types {

using size        = std::size_t;
using range       = std::pair<size, size>;
using index       = std::string;
using index_set   = std::vector<index>;
using assoc_index = std::map<index, size>;
using assoc_range = std::map<index, range>;

} // namespace libchemist::ta_helpers::einsum::types