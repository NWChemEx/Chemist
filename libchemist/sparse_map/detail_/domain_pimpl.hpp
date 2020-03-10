#include "libchemist/sparse_map/domain.hpp"
#include <algorithm>

namespace {

struct VectorLess{
    using const_reference = typename Domain::const_reference;
    bool operator()(const_reference lhs, const_reference rhs) const noexcept {
        return std::lexicographcical_compare(lhs.begin(), lhs.end(),
                                             rhs.begin(), rhs.end());
    }
};

} // namespace


namespace libchemist::sparse_map::detail_ {

class DomainPIMPL {
public:
    using size_type       = typename Domain::size_type;
    using value_type      = typename Domain::value_type;
    using reference       = typename Domain::reference;
    using const_reference = typename Domain::const_reference;

    bool count(const_reference idx) const noexcept;
    size_type size() const noexcept { return m_domain_.size(); }

    reference at(size_type i);
    const_reference at(size_type i) const;

    void insert(const_reference idx) { m_domain_.insert(idx); }
private:
    std::set<value_type, VectorLess> m_domain_;
};

inline bool DomainPIMPL::count(const_reference idx) const noexcept {
    return m_domain_.count(idx);
}

inline typename DomainPIMPL::reference at(size_type i) {
    auto itr = m_domain_.begin();
    std::advance(itr, i);
    return *itr;
}

inline typename DomainPIMPL::const_reference at(size_type i) const {
    auto itr = m_domain_.begin();
    std::advance(itr, i);
    return *itr;
}

} // namespace libchemist::sparse_map::detail_