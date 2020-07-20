#include "libchemist/sparse_map/domain.hpp"
#include <algorithm>

namespace libchemist::sparse_map::detail_ {

class DomainPIMPL {
public:
    using size_type       = typename Domain::size_type;
    using value_type      = typename Domain::value_type;
    using reference       = typename Domain::reference;
    using const_reference = typename Domain::const_reference;

    bool count(const_reference idx) const noexcept;
    size_type rank() const noexcept;
    size_type size() const noexcept { return m_domain_.size(); }
    reference at(size_type i);
    const_reference at(size_type i) const;

    void insert(const_reference idx);

    bool operator==(const DomainPIMPL& rhs) const noexcept;
    bool operator!=(const DomainPIMPL& rhs) const noexcept;

    void hash(sde::Hasher& h) const { h(m_domain_); }
private:

    struct VectorLess{
        bool operator()(const_reference lhs,
                        const_reference rhs) const noexcept;
    };

    void bounds_check_(size_type i) const;

    std::set<value_type, VectorLess> m_domain_;
};

//------------------------------------------------------------------------------
// Inline implementations
//------------------------------------------------------------------------------

inline bool DomainPIMPL::count(const_reference idx) const noexcept {
    return m_domain_.count(idx);
}

inline typename DomainPIMPL::size_type DomainPIMPL::rank() const noexcept {
    return m_domain_.empty() ? 0 : m_domain_.begin()->size();
}

inline typename DomainPIMPL::reference DomainPIMPL::at(size_type i) {
    bounds_check_(i);
    auto itr = m_domain_.begin();
    std::advance(itr, i);
    return *itr;
}

inline typename DomainPIMPL::const_reference
DomainPIMPL::at(size_type i) const {
    bounds_check_(i);
    auto itr = m_domain_.begin();
    std::advance(itr, i);
    return *itr;
}

inline void DomainPIMPL::insert(const_reference idx) {
    if(!m_domain_.empty() && idx.size() != rank()) {
        using namespace std::string_literals;
        throw std::runtime_error(
          "Rank of idx ("s + std::to_string(idx.size()) +
          ") != rank of domain ("s + std::to_string(rank()) + ")"s);
    }
    m_domain_.insert(idx);
}

inline bool DomainPIMPL::operator==(const DomainPIMPL& rhs) const noexcept {
    return m_domain_ == rhs.m_domain_;
}

inline bool DomainPIMPL::operator!=(const DomainPIMPL& rhs) const noexcept {
    return !((*this) == rhs);
}

inline bool
DomainPIMPL::VectorLess::operator()(const_reference lhs,
                                    const_reference rhs) const noexcept {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

inline void DomainPIMPL::bounds_check_(size_type i) const{
    if(i < size()) return;
    using namespace std::string_literals;
    throw std::out_of_range(
      "i = "s + std::to_string(i) + " is not in the range [0, "s +
      std::to_string(size()) + ")."s);

}
} // namespace libchemist::sparse_map::detail_