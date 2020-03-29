#pragma once
#include "libchemist/sparse_map/sparse_map.hpp"
#include <tiledarray.h>
#include <map>
#include <set>

namespace libchemist {

std::pair<std::map<std::vector<std::size_t>,
                   std::map<std::vector<std::size_t>, TA::Range>>,
          std::map<std::vector<std::size_t>, TA::Range>>
condense_tensor_range(const sparse_map::SparseMap& sm,
                      const TA::TiledRange& trange,
                      const std::set<std::size_t>& ind_modes = {});

} // namespace libchemist
