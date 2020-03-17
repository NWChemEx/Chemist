#include "libchemist/sparse_map/sparse_map.hpp"

namespace libchemist::sparse_map {

using size_type = typename SparseMap::size_type;
using mapped_type = typename SparseMap::mapped_type;
using iterator = typename SparseMap::iterator;
using const_iterator = typename SparseMap::const_iterator;

namespace detail_ {

struct SparseMapPIMPL {
    using key_type = typename SparseMap::key_type;
    using mapped_type = typename SparseMap::mapped_type;
    std::map<key_type, mapped_type> m_sm;
};

} // detail_

//TODO: Factor these macros out and use project-wide to reduce boilerplate

#define PIMPL_DEFAULT_CTOR(class_name, pimpl_type)\
    class_name::class_name() : m_pimpl_(std::make_unique<pimpl_type>()){}

#define PIMPL_COPY_CTOR(class_name, pimpl_type)\
    class_name::class_name(const class_name& rhs) : \
        m_pimpl_(std::make_unique<pimpl_type>(\
            rhs.m_pimpl_ ? *rhs.m_pimpl_ : pimpl_type{})){}

#define PIMPL_MOVE_CTOR(class_name)\
  class_name::class_name(class_name&& rhs) noexcept = default;

#define PIMPL_COPY_ASSIGNMENT(class_name, pimpl_type) \
    class_name& class_name::operator=(const class_name& rhs) {\
        if(this == & rhs) return *this;\
        m_pimpl_ = std::make_unique<pimpl_type>(\
            rhs.m_pimpl_ ? *rhs.m_pimpl_ : pimpl_type{});\
        return *this;\
    }

#define PIMPL_MOVE_ASSIGNMENT(class_name)\
    class_name& class_name::operator=(class_name&& rhs) noexcept = default;

#define PIMPL_DTOR(class_name)\
    class_name::~class_name() noexcept = default

#define PIMPL_DEFAULT_FXNS(class_name, pimpl_type)\
    PIMPL_DEFAULT_CTOR(class_name, pimpl_type)\
    PIMPL_COPY_CTOR(class_name, pimpl_type)\
    PIMPL_MOVE_CTOR(class_name)\
    PIMPL_COPY_ASSIGNMENT(class_name, pimpl_type)\
    PIMPL_MOVE_ASSIGNMENT(class_name)\
    PIMPL_DTOR(class_name)

PIMPL_DEFAULT_FXNS(SparseMap, detail_::SparseMapPIMPL);

size_type SparseMap::size() const noexcept {
    return m_pimpl_->m_sm.size();
}

bool SparseMap::count(const key_type& i) const noexcept {
    if(!m_pimpl_) return false;
    return m_pimpl_->m_sm.count(i);
}

size_type SparseMap::ind_rank() const noexcept {
    if(!m_pimpl_) return 0;
    if(m_pimpl_->m_sm.empty())return 0;
    return m_pimpl_->m_sm.begin()->first.size();
}

size_type SparseMap::dep_rank() const noexcept {
    if(!m_pimpl_) return 0;
    if(m_pimpl_->m_sm.empty())return 0;
    return m_pimpl_->m_sm.begin()->second.rank();
}

mapped_type& SparseMap::at(const key_type& ind) {
    return m_pimpl_->m_sm.at(ind);
}

const mapped_type& SparseMap::at(const key_type& ind) const {
    return m_pimpl_->m_sm.at(ind);
}

iterator SparseMap::begin() noexcept { return m_pimpl_->m_sm.begin(); }

const_iterator SparseMap::begin() const noexcept {
    return m_pimpl_->m_sm.begin();
}

iterator SparseMap::end() noexcept { return m_pimpl_->m_sm.end(); }

const_iterator SparseMap::end() const noexcept { return m_pimpl_->m_sm.end(); }

void SparseMap::add_to_domain(key_type ind, key_type dep) {
    if(m_pimpl_->m_sm.empty()){
        m_pimpl_->m_sm.emplace(std::move(ind), Domain{std::move(dep)});
        return;
    }
    check_rank_(ind.size(), false);
    check_rank_(dep.size(), true);
    m_pimpl_->m_sm[ind].insert(dep);
}

SparseMap SparseMap::operator*(const SparseMap& rhs) const {
    SparseMap rv;
    const auto& lsm = m_pimpl_->m_sm;
    const auto& rsm = rhs.m_pimpl_->m_sm;
    auto& osm = rv.m_pimpl_->m_sm;
    for(const auto& [k, v] : lsm)
        if(rsm.count(k))
            osm[k] = lsm.at(k) * rsm.at(k);
    return rv;
}

SparseMap& SparseMap::operator*=(const SparseMap& rhs) {
    auto temp = (*this) * rhs;
    swap(temp);
    return *this;
}

bool SparseMap::operator==(const SparseMap& rhs) const noexcept {
    if(!m_pimpl_) return !rhs.m_pimpl_;
    else if(!rhs.m_pimpl_) return false;
    return m_pimpl_->m_sm == rhs.m_pimpl_->m_sm;
}

void SparseMap::hash(sde::Hasher& h) const {
    h(m_pimpl_->m_sm);
}

std::ostream& SparseMap::print(std::ostream& os) const {
    using namespace utilities::printing;
    using utilities::printing::operator<<;
    return os << m_pimpl_->m_sm;
}

void SparseMap::check_rank_(size_type idx_rank, bool dep) const {
    const auto corr_rank = dep ? dep_rank() : ind_rank();
    if(corr_rank == idx_rank) return;
    using namespace std::string_literals;
    const std::string iden = dep ? "Dependent" : "Independent";
    throw std::runtime_error(
      iden + " indices have ranks of "s + std::to_string(corr_rank) +
      " but idx with rank " + std::to_string(idx_rank) + " was provided."s
    );
}

} // namespace libchemist::sparse_map