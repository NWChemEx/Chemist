//#pragma once
//#include "chemist/sparse_map.hpp"
//#include <map>
//#include <utilities/printing/print_stl.hpp>
//
// namespace chemist::detail_ {
//
// class SparseMapPIMPL {
// public:
//    using size_type  = typename SparseMap::size_type;
//    using index_type = typename SparseMap::index_type;
//
//    bool count(const index_type& ind) const noexcept;
//    bool empty() const noexcept { return m_sm_.empty(); }
//
//    void add_to_domain(index_type ind, index_type dep);
//
//    void check_dep_size(const index_type& idx) const {
//        if(m_sm.empty())
//            return;
//        else if(m_sm.begin()->second.begin()->size() == idx.size())
//            return;
//        throw std::runtime_error("Dependent index is wrong rank");
//    }
//
//    void check_ind_size(const index_type& idx) const;
//
// private:
//    std::map<index_type, std::vector<index_type>> m_sm_;
//};
//
// inline bool SparseMapPIMPL::count(const index_type& ind) const noexcept {
//    return m_sm_.count(ind);
//}
//
// inline void SparseMapPIMPL::add_to_domain(index_type ind, index_type dep) {
//    if(!count(ind)) {
//        m_sm.emplace(std::move(ind), std::vector{std::move(dep)});
//    }
//    else{ m_sm[ind].emplace_back(std::move(dep)); }
//}
//
// inline void SparseMapPIMPL::check_dep_size(const index_type& idx) const {
//    // See if we added any indices yet, if not the size is fine
//    if(empty()) return;
//
//    // We have indices, all of them must have the
//    if(m_sm.begin()->second.begin()->size() == idx.size())
//        return;
//    throw std::runtime_error("Dependent index is wrong rank");
//}
//
// inline void SparseMapPIMPL::check_ind_size(const index_type& idx) const {
//    if(empty())
//        return;
//    else if(m_sm.begin()->first.size() == idx.size())
//        return;
//    throw std::runtime_error("Independent index is wrong rank");
//}
//
//} // namespace detail_