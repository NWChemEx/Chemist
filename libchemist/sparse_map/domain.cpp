#include "libchemist/sparse_map/detail_/domain_pimpl.hpp"

namespace libchemist::sparse_map {

using size_type = typename Domain::size_type;
using reference = typename Domain::reference;
using const_reference = typename Domain::const_reference;

Domain::Domain() : m_pimpl_(std::make_unique<detail_::DomainPIMPL>()) {}

Domain::~Domain() const noexcept = default;

bool Domain::count(const_reference idx) const noexcept {
    return m_pimpl_->count(idx);
}

size_type Domain::size() const noexcept { return m_pimpl_->size(); }

reference Domain::operator[](size_type i) { return m_pimpl_->at(i); }

const_reference Domain::operator[](size_type i) const {
    return m_pimpl_->at(i);
}

void Domain::insert(const_reference idx) { m_pimpl_->insert(idx); }

Domain Domain::operator*(const Domain& rhs) const {

}

} // namespace libchemist::sparse_map