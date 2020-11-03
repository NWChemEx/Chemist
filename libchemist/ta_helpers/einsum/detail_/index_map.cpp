#include "libchemist/ta_helpers/einsum/detail_/detail_.hpp"
#include "libchemist/ta_helpers/einsum/detail_/index_map.hpp"
#include <tuple> // for std::tie

namespace libchemist::ta_helpers::einsum::detail_ {

IndexMap::IndexMap() noexcept = default;

IndexMap::IndexMap(types::index result_idx, types::index lhs_idx,
                   types::index rhs_idx) :
  m_result_vars_(parse_index(std::move(result_idx))),
  m_lhs_vars_(parse_index(std::move(lhs_idx))),
  m_rhs_vars_(parse_index(std::move(rhs_idx))) {}

IndexMap::~IndexMap() noexcept = default;

bool IndexMap::operator==(const IndexMap& other) const noexcept {
    return std::tie(m_result_vars_, m_lhs_vars_, m_rhs_vars_) ==
           std::tie(other.m_result_vars_, other.m_lhs_vars_, other.m_rhs_vars_);
}

} // namespace libchemist::ta_helpers::einsum::detail_