//#pragma once
//#include <map>
//
//namespace libchemist::sparse_map {
//
//// Forward declare classes needed to specialize
//template<typename IndexType> class Domain;
//template<typename IndIndex, typename DepIndex> class SparseMap;
//
//namespace detail_ {
//template<typename IndIndex, typename DepIndex>
//struct SparseMapTraits{
//    using size_type   = std::size_t;
//    using key_type    = IndIndex;
//    using mapped_type = Domain<DepIndex>;
//    using value_type  = std::pair<const key_type, mapped_type>;
//private:
//    using map_t = std::map<key_type, mapped_type>;
//public:
//    using iterator       = typename map_t::iterator;
//    using const_iterator = typename map_t::const_iterator;
//};
//
//}
//} // namespace libchemist::sparse_map
