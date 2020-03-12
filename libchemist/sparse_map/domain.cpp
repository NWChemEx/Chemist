#include "libchemist/sparse_map/detail_/domain_pimpl.hpp"
#include <utilities/printing/print_stl.hpp>

namespace libchemist::sparse_map {

using size_type = typename Domain::size_type;
using reference = typename Domain::reference;
using const_reference = typename Domain::const_reference;

Domain::Domain() : m_pimpl_(std::make_unique<detail_::DomainPIMPL>()) {}

Domain::Domain(const Domain& rhs) {
    rhs.check_pimpl_();
    m_pimpl_ = std::make_unique<detail_::DomainPIMPL>(*rhs.m_pimpl_);
}

Domain::Domain(Domain&& rhs) noexcept :
    m_pimpl_(std::move(rhs.m_pimpl_)) {}

Domain& Domain::operator=(const Domain& rhs) {
    if(this == &rhs) return *this;
    rhs.check_pimpl_();
    m_pimpl_ = std::make_unique<detail_::DomainPIMPL>(*rhs.m_pimpl_);
    return *this;
}

Domain& Domain::operator=(Domain&& rhs) noexcept {
    if(this == &rhs) return *this;
    m_pimpl_ = std::move(rhs.m_pimpl_);
    return *this;
}

Domain::~Domain() noexcept = default;

bool Domain::count(const_reference idx) const noexcept {
    if(!m_pimpl_) return false;
    return m_pimpl_->count(idx);
}

size_type Domain::rank() const {
    check_pimpl_();
    return m_pimpl_->rank();
}

size_type Domain::size() const noexcept {
    return m_pimpl_ ? m_pimpl_->size() : 0;
}

reference Domain::operator[](size_type i) {
    check_pimpl_();
    return m_pimpl_->at(i);
}

const_reference Domain::operator[](size_type i) const {
    check_pimpl_();
    return m_pimpl_->at(i);
}

void Domain::insert(const_reference idx) {
    if(!m_pimpl_) m_pimpl_ = std::make_unique<detail_::DomainPIMPL>();
    m_pimpl_->insert(idx);
}

Domain& Domain::operator*=(const Domain& rhs) {
    auto temp = (*this)*rhs;
    swap(temp);
    return *this;
}

Domain Domain::operator*(const Domain& rhs) const {
    // Although non-canoncial we presently opt to implement operator*= in terms
    // of operator*. This is because we have to overwrite the state of the
    // current instance in order to implement *=. It's thus easier to build the
    // correct answer from scratch in operator* and swap it with the current
    // instance in *=
    Domain rv;
    const auto new_rank = rank() + rhs.rank();
    for(auto&& lidx : *this){
        for(auto&& ridx : rhs) {
            value_type new_index;
            new_index.reserve(new_rank);
            new_index.insert(new_index.end(), lidx.begin(), lidx.end());
            new_index.insert(new_index.end(), ridx.begin(), ridx.end());
            rv.insert(new_index);
        }
    }
    return rv;
}

Domain& Domain::operator+=(const Domain& rhs) {
    for(auto&& idx : rhs) insert(idx);
    return *this;
}

Domain Domain::operator+(const Domain& rhs) const {
    Domain rv(*this);
    rv += rhs;
    return rv;
}

bool Domain::operator==(const Domain& rhs) const noexcept {
    if(!m_pimpl_) return rhs.m_pimpl_ ? false : true;
    else if(!rhs.m_pimpl_) return m_pimpl_ ? false : true;
    return *m_pimpl_ == *rhs.m_pimpl_;
}

void Domain::hash(sde::Hasher& h) const {
    check_pimpl_();
    h(*m_pimpl_);
}

std::ostream& Domain::print(std::ostream& os) const {
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

void Domain::check_pimpl_() const {
    if(m_pimpl_) return;
    throw std::runtime_error(
      "PIMPL is empty. Are you trying to use a moved-from instance?");
}

} // namespace libchemist::sparse_map