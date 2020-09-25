//#pragma once
//#include <map>
//#include <set>
//#include <vector>
//#include <stdexcept>
//
//namespace libchemist::sparse_map::detail_ {
//
///** @brief
// *
// *  @note The concepts of independent and dependent modes only make sense in
// *        terms of a ToT (or the SparseMap being used to create one).
// */
//class ToTMapper {
//private:
//    using il_type = std::initializer_list<std::pair<const std::size_t, std::size_t>>;
//public:
//    using size_type     = std::size_t;
//    using index_type    = std::vector<size_type>;
//    using mode_map_type = std::map<size_type, size_type>;
//    using mode_set_type = std::set<size_type>;
//
//    ToTMapper(size_type rank, il_type il) :
//      ToTMapper(rank, mode_map_type(il)) {}
//
//    /**
//     *
//     * @param rank
//     * @param[in] ind_tot_mode_2_t_mode A map such `ind_tot_mode_2_t_mode[i]`
//     *            is the mode of the tensor that the i-th independent mode of
//     *            the ToT maps to. Note that a given independent ToT mode may
//     *            not map onto any mode of the tensor (*i.e.* it is a free
//     *            independent mode). ind_tot_mode_2_t is assumed to be a
//     *            bijection (*i.e.*, `ind_tot_mode_2_t_mode[i]` does not equal
//     *            `ind_tot_mode_2_t_mode[j]` unless `i == j`).
//     *
//     * @throw std::runtime_error if `ind_tot_mode_2_t_mode` is not a bijection.
//     *                           Strong throw guarantee.
//     */
//    ToTMapper(size_type rank, mode_map_type ind_tot_mode_2_t_mode = {});
//
//    size_type t_ind_rank() const { return m_ind_modes_.size(); }
//
//    size_type t_dep_rank() const { return m_dep_modes_.size(); }
//
//    size_type t_rank() const { return t_ind_rank() + t_dep_rank(); }
//
//    /** @brief Does the i-th mode of the tensor map to an independent ToT mode?
//     *
//     * @param i
//     * @return
//     */
//    bool is_ind_mode(size_type i) const { return m_ind_modes_.count(i); }
//
//    /** @brief Does the i-th mode of the tensor map to an dependent ToT mode?
//     *
//     * @param[in] i The mode of
//     * @return
//     */
//    bool is_dep_mode(size_type i) const { return m_dep_modes_.count(i); }
//
//    const auto& ind_modes() const { return m_ind_modes_; }
//
//    const auto& dep_modes() const { return m_dep_modes_; }
//
//    template<typename IndIndexType, typename DepIndexType>
//    auto tot2t_index(IndIndexType&& ind_idx, DepIndexType&& dep_idx) const;
//
//private:
//    mode_map_type m_ind_tot_mode_2_t_mode_;
//
//    mode_map_type m_t_mode_2_ind_tot_mode_;
//
//    /// Set of modes of the tensor which map to independent ToT modes
//    mode_set_type m_ind_modes_;
//
//    /// Set of modes of the tensor which map to dependent ToT modes
//    mode_set_type m_dep_modes_;
//}; // class ToTMapper
//
//// ------------------------ Inline Implementations -----------------------------
//
//inline ToTMapper::ToTMapper(size_type rank,
//                            mode_map_type ind_tot_mode_2_t_mode) :
//  m_ind_tot_mode_2_t_mode_(std::move(ind_tot_mode_2_t_mode)) {
//    for(const auto& [k, v] : m_ind_tot_mode_2_t_mode_) {
//        if(m_t_mode_2_ind_tot_mode_.count(v))
//            throw std::runtime_error(
//              "ind_tot_mode_2_t_mode is not a bijection");
//        m_t_mode_2_ind_tot_mode_[v] = k;
//        m_ind_modes_.insert(v);
//    }
//    for(size_type i = 0; i < rank; ++i)
//        if(!m_ind_modes_.count(i)) m_dep_modes_.insert(i);
//}
//
//template<typename IndIndexType, typename DepIndexType>
//auto ToTMapper::tot2t_index(IndIndexType&& ind_idx,
//                            DepIndexType&& dep_idx) const {
//    const auto rank = t_rank();
//    index_type rv(rank);
//    for(size_type i = 0, counter = 0; i < rank; ++i) {
//        const bool is_ind = is_ind_mode(i);
//        rv[i] = is_ind ? ind_idx.at(m_t_mode_2_ind_tot_mode_.at(i)) :
//                         dep_idx.at(counter++);
//    }
//    return rv;
//}
//
//} // namespace libchemist::sparse_map::detail_
