#include "chemist/sparse_map/domain/detail_/domain_base.hpp"
#include "chemist/sparse_map/domain/domain.hpp"
#include "domain_pimpl.hpp"
#include "tiled_domain_pimpl.hpp"
#include <utilities/printing/print_stl.hpp>

namespace {

// Hides decisions affecting which PIMPL the class gets
template<typename IndexType, typename... Args>
std::unique_ptr<chemist::sparse_map::detail_::DomainPIMPL<IndexType>> new_pimpl(
  Args&&... args) {
    using namespace chemist::sparse_map;
    using namespace chemist::sparse_map::detail_;
    using pimpl_type = DomainPIMPL<IndexType>;

    // Note if this changes you'll also have to change it in set_trange's
    // implementation in domain.cpp
    using tpimpl_type = TiledDomainPIMPL<pimpl_type>;

    if constexpr(std::is_same_v<IndexType, TileIndex>) {
        return std::make_unique<tpimpl_type>(std::forward<Args>(args)...);
    } else {
        return std::make_unique<pimpl_type>(std::forward<Args>(args)...);
    }
}

} // namespace

namespace chemist::sparse_map::detail_ {

#define DOMAINBASE DomainBase<DerivedType, IndexType>

//------------------------------------------------------------------------------
//                               CTors
//------------------------------------------------------------------------------

template<typename DerivedType, typename IndexType>
DOMAINBASE::DomainBase() : m_pimpl_(new_pimpl<IndexType>()) {}

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
    rhs.pimpl_().clone().swap(m_pimpl_);
    return *this;
}

template<typename DerivedType, typename IndexType>
DOMAINBASE& DOMAINBASE::operator=(DomainBase&& rhs) noexcept {
    if(this == &rhs) return *this;
    m_pimpl_ = std::move(rhs.m_pimpl_);
    return *this;
}

template<typename DerivedType, typename IndexType>
DOMAINBASE::~DomainBase() noexcept = default;

//------------------------------------------------------------------------------
//                              Accessors
//------------------------------------------------------------------------------

template<typename DerivedType, typename IndexType>
typename DOMAINBASE::size_type DOMAINBASE::rank() const noexcept {
    return m_pimpl_ ? pimpl_().rank() : 0;
}

template<typename DerivedType, typename IndexType>
typename DOMAINBASE::size_type DOMAINBASE::size() const noexcept {
    return m_pimpl_ ? pimpl_().size() : 0;
}

template<typename DerivedType, typename IndexType>
std::vector<typename DOMAINBASE::size_type> DOMAINBASE::result_extents() const {
    return m_pimpl_ ? pimpl_().result_extents() : std::vector<size_type>{};
}

template<typename DerivedType, typename IndexType>
typename DOMAINBASE::value_type DOMAINBASE::result_index(
  const value_type& old) const {
    if(empty()) throw std::out_of_range("Domain is empty");
    return pimpl_().result_index(old);
}

template<typename DerivedType, typename IndexType>
bool DOMAINBASE::count(const_reference idx) const noexcept {
    if(!m_pimpl_) return false;
    return pimpl_().count(idx);
}

template<typename DerivedType, typename IndexType>
typename DOMAINBASE::value_type DOMAINBASE::operator[](size_type i) const {
    return pimpl_().at(i);
}

//------------------------------------------------------------------------------
//                                  Setters
//------------------------------------------------------------------------------

template<typename DerivedType, typename IndexType>
void DOMAINBASE::insert(value_type idx) {
    if(!m_pimpl_) m_pimpl_ = new_pimpl<IndexType>();
    pimpl_().insert(std::move(idx));
}

template<typename DerivedType, typename IndexType>
DerivedType DOMAINBASE::inject(
  const std::map<size_type, size_type>& injections) const {
    using vector_type = typename IndexType::index_type;

    if(empty()) return downcast_();

    // The rank of the indices in the resulting Domain
    const auto out_rank = rank() + injections.size();

    // If we have rank r indices and we are given n injections, we will make a
    // a rank r + n index, hence all modes in the input must be in the range
    // [0, n].
    for(const auto& [k, v] : injections) {
        if(k > out_rank) {
            throw std::out_of_range("Mode " + std::to_string(k) +
                                    "  is not in the range [0, " +
                                    std::to_string(out_rank) + "]. ");
        }
    }

    DerivedType rv;

    for(const auto& idx : *this) {
        vector_type new_idx(out_rank, 0);
        for(std::size_t i = 0, counter = 0; i < out_rank; ++i) {
            if(injections.count(i))
                new_idx[i] = injections.at(i);
            else {
                new_idx[i] = idx[counter];
                ++counter;
            }
        }
        rv.insert(IndexType(std::move(new_idx)));
    }
    return rv;
}

template<typename DerivedType, typename IndexType>
DerivedType& DOMAINBASE::operator*=(const DomainBase& rhs) {
    if(!rhs.m_pimpl_ || !m_pimpl_) {
        m_pimpl_ = new_pimpl<IndexType>();
        return downcast_();
    }

    (*m_pimpl_) *= (*rhs.m_pimpl_);
    return downcast_();
}

template<typename DerivedType, typename IndexType>
DerivedType DOMAINBASE::operator*(const DomainBase& rhs) const {
    DerivedType rv(downcast_());
    rv *= rhs;
    return rv;
}

template<typename DerivedType, typename IndexType>
DerivedType& DOMAINBASE::operator+=(const DomainBase& rhs) {
    if(!m_pimpl_) m_pimpl_ = new_pimpl<IndexType>();
    if(!rhs.m_pimpl_) return downcast_();

    (*m_pimpl_) += (*rhs.m_pimpl_);
    return downcast_();
}

template<typename DerivedType, typename IndexType>
DerivedType DOMAINBASE::operator+(const DomainBase& rhs) const {
    DerivedType rv(downcast_());
    rv += rhs;
    return rv;
}

template<typename DerivedType, typename IndexType>
DerivedType& DOMAINBASE::operator^=(const DomainBase& rhs) {
    if(!m_pimpl_ || !rhs.m_pimpl_) {
        m_pimpl_ = new_pimpl<IndexType>();
        return downcast_();
    }
    (*m_pimpl_) ^= (*rhs.m_pimpl_);
    return downcast_();
}

template<typename DerivedType, typename IndexType>
DerivedType DOMAINBASE::operator^(const DomainBase& rhs) const {
    DerivedType rv(downcast_());
    rv ^= rhs;
    return rv;
}

//------------------------------------------------------------------------------
//                                Utilities
//------------------------------------------------------------------------------

template<typename DerivedType, typename IndexType>
bool DOMAINBASE::operator==(const DomainBase& rhs) const noexcept {
    if(!m_pimpl_)
        return !rhs.m_pimpl_;
    else if(!rhs.m_pimpl_)
        return false;
    return *m_pimpl_ == *rhs.m_pimpl_;
}

template<typename DerivedType, typename IndexType>
void DOMAINBASE::hash(pluginplay::Hasher& h) const {
    h(pimpl_());
}

template<typename DerivedType, typename IndexType>
std::ostream& DOMAINBASE::print(std::ostream& os) const {
    os << "{";
    using utilities::printing::operator<<;
    auto begin_itr = begin();
    auto end_itr   = end();
    if(begin_itr != end_itr) {
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

//------------------------------------------------------------------------------
//                              Private Methods
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
//                           Template Instantiations
//------------------------------------------------------------------------------

template class DomainBase<Domain<ElementIndex>, ElementIndex>;
template class DomainBase<Domain<TileIndex>, TileIndex>;

} // namespace chemist::sparse_map::detail_
