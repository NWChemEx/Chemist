//#include "libchemist/detail_/sparse_map_pimpl.hpp"
//
//namespace {
//
//template<typename T>
//auto vector_concat(const std::vector<T>& lhs, const std::vector<T>& rhs) {
//    std::vector<T> rv;
//    rv.reserve(lhs.size() + rhs.size());
//    rv.insert(rv.end(), lhs.begin(), lhs.end());
//    rv.insert(rv.end(), rhs.begin(), rhs.end());
//    return rv;
//}
//
//} // namespace
//
//namespace libchemist {
//
//using size_type  = typename SparseMap::size_type;
//using index_type = typename SparseMap::index_type;
//
//SparseMap::SparseMap() :
//  m_pimpl_(std::make_unique<detail_::SparseMapPIMPL>()) {}
//
//SparseMap::SparseMap(const SparseMap& rhs) :
//  m_pimpl_(std::make_unique<detail_::SparseMapPIMPL>(*rhs.m_pimpl_)) {}
//
//SparseMap::SparseMap(SparseMap&& rhs) noexcept = default;
//
//SparseMap& SparseMap::operator=(const SparseMap& rhs) {
//    if(this == &rhs) return *this;
//    m_pimpl_ = std::make_unique<detail_::SparseMapPIMPL>(*rhs.m_pimpl_);
//    return *this;
//}
//
//SparseMap& SparseMap::operator=(SparseMap&& rhs) noexcept = default;
//
//SparseMap::~SparseMap() noexcept = default;
//
//void SparseMap::add_to_domain(index_type ind, index_type dep) {
//    m_pimpl_->check_ind_size(ind);
//    m_pimpl_->check_dep_size(dep);
//    m_pimpl_->m_sm.emplace(std::make_pair(std::move(ind), std::move(dep)));
//}
//
//SparseMap SparseMap::tensor_product(const SparseMap& rhs) const {
//    SparseMap rv;
//    for(auto&& [k, v] : m_pimpl_->m_sm) {
//        if(!rhs.m_pimpl_->m_sm.count(k)) continue;
//        const auto& v2 = rhs.m_pimpl_->m_sm.at(k);
//        index_type idx = vector_concat(v, v2);
//        rv.m_pimpl_->m_sm.emplace(std::make_pair(k, std::move(idx)));
//    }
//    return rv;
//}
//
//bool SparseMap::operator==(const SparseMap& other) const noexcept {
//    return m_pimpl_->m_sm == other.m_pimpl_->m_sm;
//}
//
//void SparseMap::hash(sde::Hasher& h) const { h(m_pimpl_->m_sm); }
//
//std::ostream& SparseMap::print(std::ostream& os) const {
//    using utilities::printing::operator<<;
//    return os << m_pimpl_->m_sm;
//}
//
//} // namespace libchemist