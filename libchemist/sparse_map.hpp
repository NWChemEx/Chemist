//#pragma once
//#include <memory>
//#include <sde/detail_/memoization.hpp>
//#include <utilities/iterators/offset_iterator.hpp>
//#include <vector>
//
//namespace libchemist {
//namespace detail_ {
//class SparseMapPIMPL;
//}
//
//
//
///** @brief Stores the sparsity relation from one index space to another
// *
// *  The SparseMap class assumes that the modes of a tensor can be classified as
// *  either independent or dependent.
// *
// */
//class SparseMap {
//public:
//    using size_type  = std::size_t;
//    using index_type = std::vector<size_type>;
//
//    SparseMap();
//    SparseMap(const SparseMap& rhs);
//    SparseMap(SparseMap&& rhs) noexcept;
//    SparseMap& operator=(const SparseMap& rhs);
//    SparseMap& operator=(SparseMap&& rhs) noexcept;
//    ~SparseMap() noexcept;
//
//    void swap(SparseMap& rhs) noexcept { m_pimpl_.swap(rhs.m_pimpl_); }
//
//    void add_to_domain(size_type ind, size_type dep);
//    void add_to_domain(index_type ind, index_type dep);
//
//    SparseMap tensor_product(const SparseMap& rhs) const;
//    SparseMap independent_union(const SparseMap& rhs) const;
//
//    bool operator==(const SparseMap& rhs) const noexcept;
//    bool operator!=(const SparseMap& rhs) const noexcept;
//
//    void hash(sde::Hasher& h) const;
//
//    std::ostream& print(std::ostream& os) const;
//
//private:
//    std::unique_ptr<detail_::SparseMapPIMPL> m_pimpl_;
//}; // class SparseMap
//
//std::ostream& operator<<(std::ostream& os, const SparseMap& sm) {
//    return sm.print(os);
//}
//
////------------------------------------------------------------------------------
//// Inline Implementations
////------------------------------------------------------------------------------
//
//inline void SparseMap::add_to_domain(size_type ind, size_type dep) {
//    add_to_domain(index_type{ind}, index_type{dep});
//}
//
//inline bool SparseMap::operator!=(const SparseMap& rhs) const noexcept {
//    return !((*this) == rhs);
//}
//
//} // namespace mp2