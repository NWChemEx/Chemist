#include "libchemist/sparse_map/detail_/from_sparse_map_helpers.hpp"

namespace libchemist::sparse_map::detail_ {

TA::TiledRange make_tot_trange_(const TA::TiledRange& t_trange,
                      const std::map<std::size_t, std::size_t>& ind_sm_modes2t,
                      const std::map<std::size_t, TA::TiledRange1>& free_ind_ranges) {
    using trange1_type = std::decay_t<decltype(t_trange.dim(0))>;
    const auto ind_rank = ind_sm_modes2t.size() + free_ind_ranges.size();
    std::vector<trange1_type> rv_trange_data;
    rv_trange_data.reserve(ind_rank);

    for(std::size_t i = 0; i < ind_rank; ++i){
        const bool is_free = free_ind_ranges.count(i);
        rv_trange_data.push_back(
          is_free ? free_ind_ranges.at(i) : t_trange.dim(ind_sm_modes2t.at(i))
        );
    }
    return TA::TiledRange(rv_trange_data.begin(), rv_trange_data.end());
}

} // namespace libchemist::sparse_map::detail_
