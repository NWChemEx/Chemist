#pragma once
#include "libchemist/ta_helpers/einsum/types.hpp"

namespace libchemist::ta_helpers::einsum::detail_ {

class IndexMap {
public:
  IndexMap() noexcept;
  IndexMap(types::index result_idx, types::index lhs_idx,
           types::index rhs_idx);
  ~IndexMap() noexcept;

  template<typename T>
  auto select_result(T&& quantities) const;

  template<typename T>
  auto select_lhs(T&& quantities) const;

  template<typename T>
  auto select_rhs(T&& quantities) const;

  template<typename T>
  auto select(T&& quantities) const;

  const auto& result_vars() const { return m_result_vars_; }
  const auto& lhs_vars() const { return m_lhs_vars_; }
  const auto& rhs_vars() const { return m_rhs_vars_; }

  bool operator==(const IndexMap& other) const noexcept;
  bool operator!=(const IndexMap& other) const noexcept;

private:
  template<typename T>
  auto select_(const types::index_set& indices, T&& quantities) const;

  types::index_set m_result_vars_;
  types::index_set m_lhs_vars_;
  types::index_set m_rhs_vars_;
};

template<typename T>
auto IndexMap::select_result(T&& quantities) const {
  return select_(result_vars(), std::forward<T>(quantities));
}

template<typename T>
auto IndexMap::select_lhs(T&& quantities) const {
  return select_(lhs_vars(), std::forward<T>(quantities));
}

template<typename T>
auto IndexMap::select_rhs(T&& quantities) const {
  return select_(rhs_vars(), std::forward<T>(quantities));
}

template<typename T>
auto IndexMap::select(T&& qs) const {
  return std::make_tuple(select_result(qs), select_lhs(qs), select_rhs(qs));
}

inline bool IndexMap::operator!=(const IndexMap& other) const noexcept {
  return !((*this) == other);
}

template<typename T>
auto IndexMap::select_(const types::index_set& indices, T&& quantities) const {
  using result_t = decltype(quantities.at(indices[0]));
  using clean_t  = std::decay_t<result_t>;
  std::vector<clean_t> rv;
  for(auto x : indices) rv.push_back(quantities.at(x));
  return rv;
}

} // namespace libchemist::einsum::detail_