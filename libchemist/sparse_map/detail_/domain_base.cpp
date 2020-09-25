#include "libchemist/sparse_map/detail_/domain_base.hpp"
#include "libchemist/sparse_map/detail_/domain_pimpl.hpp"
#include "libchemist/sparse_map/domain.hpp"
#include <utilities/printing/print_stl.hpp>

namespace libchemist::sparse_map::detail_ {

#define DOMAINBASE DomainBase<DerivedType, IndexType>

//------------------------------------------------------------------------------
//                               CTors
//------------------------------------------------------------------------------

template<typename DerivedType, typename IndexType>
DOMAINBASE::DomainBase() :
  m_pimpl_(std::make_unique<pimpl_type>()) {}

template<typename DerivedType, typename IndexType>
DOMAINBASE::DomainBase(std::initializer_list<value_type> il) : DomainBase() {
    for(auto&& x : il) pimpl_().insert(std::move(x));
}

template<typename DerivedType, typename IndexType>
DOMAINBASE::DomainBase(const DomainBase& rhs) :
  m_pimpl_(rhs.pimpl_().clone()) {}

template<typename DerivedType, typename IndexType>
DOMAINBASE::DomainBase(DomainBase&& rhs) noexcept :
  m_pimpl_(std::move(rhs.m_pimpl_)) {}

template<typename DerivedType, typename IndexType>
DOMAINBASE& DOMAINBASE::operator=(const DomainBase& rhs) {
    if(this == &rhs) return *this;
    m_pimpl_.swap(rhs.pimpl_().clone());
    return *this;
}

template<typename DerivedType, typename IndexType>
DOMAINBASE& DOMAINBASE::operator=(DomainBase&& rhs) {
    if(this == &rhs) return *this;
    m_pimpl_ = std::move(rhs.m_pimpl_);
    return *this;
}

template<typename DerivedType, typename IndexType>
DOMAINBASE::~DomainBase() noexcept = default;

template<typename DerivedType, typename IndexType>
typename DOMAINBASE::size_type DOMAINBASE::rank() const {
    return pimpl_().rank();
}

template<typename DerivedType, typename IndexType>
typename DOMAINBASE::size_type DOMAINBASE::size() const noexcept {
    return m_pimpl_ ? pimpl_().size() : 0;
}

template<typename DerivedType, typename IndexType>
bool DOMAINBASE::count(const_reference idx) const noexcept {
    if(!m_pimpl_) return false;
    return pimpl_().count(idx);
}

template<typename DerivedType, typename IndexType>
typename DOMAINBASE::const_reference DOMAINBASE::operator[](size_type i) const {
    return pimpl_().at(i);
}

template<typename DerivedType, typename IndexType>
void DOMAINBASE::insert(value_type idx) {
    if(!m_pimpl_) m_pimpl_ = std::make_unique<pimpl_type>();
    pimpl_().insert(std::move(idx));
}

template<typename DerivedType, typename IndexType>
DOMAINBASE& DOMAINBASE::operator*=(const DomainBase& rhs) {
    auto new_pimpl = pimpl_().clone();
    const auto new_rank = rank() + rhs.rank();
    using vector_type = typename value_type::index_type;
    for(auto&& lidx : *this)
        for(auto&& ridx: rhs){
            vector_type new_index;
            new_index.reserve(new_rank);
            new_index.insert(new_index.end(), lidx.begin(), lidx.end());
            new_index.insert(new_index.end(), ridx.begin(), ridx.end());
            new_pimpl->insert(value_type(std::move(new_index)));
        }
    m_pimpl_.swap(new_pimpl);
    return *this;
}

template<typename DerivedType, typename IndexType>
DerivedType DOMAINBASE::operator*(const DerivedType& rhs) const {
    DerivedType rv(downcast_());
    rv *= rhs;
    return rv;
}

template<typename DerivedType, typename IndexType>
DOMAINBASE& DOMAINBASE::operator+=(const DomainBase& rhs) {
    for(auto&& idx : rhs) insert(idx);
    return *this;
}

template<typename DerivedType, typename IndexType>
DerivedType DOMAINBASE::operator+(const DerivedType& rhs) const {
    DerivedType rv(downcast_());
    rv += rhs;
    return rv;
}

template<typename DerivedType, typename IndexType>
bool DOMAINBASE::operator==(const DomainBase& rhs) const noexcept {
    if(!(m_pimpl_ || rhs.m_pimpl_)) return true;
    return *m_pimpl_ == *rhs.m_pimpl_;
}

template<typename DerivedType, typename IndexType>
void DOMAINBASE::hash(sde::Hasher& h) const { h(pimpl_()); }

template<typename DerivedType, typename IndexType>
std::ostream& DOMAINBASE::print(std::ostream& os) const {
    os << "{";
    using utilities::printing::operator<<;
    auto begin_itr = begin();
    auto end_itr = end();
    if(begin_itr != end_itr){
        os << *begin_itr;
        ++begin_itr;
    }
    while(begin_itr != end_itr) {
        os << ", " << *begin_itr;
        ++begin_itr;
    }
    os << "}";
    return os;
}

template<typename DerivedType, typename IndexType>
DerivedType& DOMAINBASE::downcast_() noexcept {
    return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, typename IndexType>
const DerivedType& DOMAINBASE::downcast_() const noexcept {
    return static_cast<const DerivedType&>(*this);
}

template<typename DerivedType, typename IndexType>
typename DOMAINBASE::pimpl_type& DOMAINBASE::pimpl_() {
    if(m_pimpl_) return *m_pimpl_;
    throw std::runtime_error("PIMPL not set. Did you move from this instance?");
}

template<typename DerivedType, typename IndexType>
const typename DOMAINBASE::pimpl_type& DOMAINBASE::pimpl_() const {
    if(m_pimpl_) return *m_pimpl_;
    throw std::runtime_error("PIMPL not set. Did you move from this instance?");
}

#undef DOMAINBASE

template class DomainBase<Domain<ElementIndex>, ElementIndex>;
template class DomainBase<Domain<TileIndex>, TileIndex>;

}
