#include "chemist/topology/connectivity_table.hpp"
#include "connectivity_table_pimpl.hpp"

namespace chemist::topology {

using pimpl_type     = typename ConnectivityTable::pimpl_type;
using bond_list_type = typename ConnectivityTable::bond_list_type;
using size_type      = typename ConnectivityTable::size_type;

//---------------------------------- CTors -------------------------------------

ConnectivityTable::ConnectivityTable() noexcept = default;

ConnectivityTable::ConnectivityTable(size_type natoms) :
  m_pimpl_(std::make_unique<pimpl_type>(natoms)) {}

ConnectivityTable::ConnectivityTable(const ConnectivityTable& other) :
  m_pimpl_(std::make_unique<pimpl_type>(other.m_pimpl_ ? *other.m_pimpl_ :
                                                         pimpl_type{})) {}

ConnectivityTable::ConnectivityTable(ConnectivityTable&& other) noexcept :
  m_pimpl_(std::move(other.m_pimpl_)) {}

ConnectivityTable& ConnectivityTable::operator=(const ConnectivityTable& rhs) {
    if(this == &rhs) return *this;
    if(!rhs.m_pimpl_) {
        m_pimpl_.reset();
        return *this;
    }
    m_pimpl_ = std::make_unique<pimpl_type>(*rhs.m_pimpl_);
    return *this;
}

ConnectivityTable& ConnectivityTable::operator=(
  ConnectivityTable&& rhs) noexcept {
    if(this == &rhs) return *this;
    m_pimpl_ = std::move(rhs.m_pimpl_);
    return *this;
}

ConnectivityTable::~ConnectivityTable() noexcept = default;

void ConnectivityTable::set_n_atoms(size_type natoms) {
    pimpl_().set_natoms(natoms);
}

void ConnectivityTable::add_bond(size_type i, size_type j) {
    pimpl_().add_bond(i, j);
}

size_type ConnectivityTable::natoms() const noexcept {
    if(m_pimpl_) return m_pimpl_->natoms();
    return 0;
}

size_type ConnectivityTable::nbonds() const noexcept {
    if(m_pimpl_) return m_pimpl_->nbonds();
    return 0;
}

bond_list_type ConnectivityTable::bonds() const {
    if(m_pimpl_) return m_pimpl_->bonds();
    return bond_list_type{};
}

std::set<size_type> ConnectivityTable::bonded_atoms(size_type i) const {
    const auto n = natoms();
    if(i >= n)
        throw std::out_of_range("i = " + std::to_string(i) +
                                " is not in the range [0, " +
                                std::to_string(n) + ").");
    std::set<size_type> rv;
    const auto& p = *m_pimpl_;
    for(size_type j = 0; j < n; ++j) {
        if(i == j) continue;
        if(p.are_bonded(i, j)) rv.insert(j);
    }
    return rv;
}

void ConnectivityTable::hash(chemist::detail_::Hasher& h) const {
    h(natoms());
    h(bonds());
}

pimpl_type& ConnectivityTable::pimpl_() {
    if(!m_pimpl_) m_pimpl_ = std::make_unique<pimpl_type>();
    return *m_pimpl_;
}

bool operator==(const ConnectivityTable& lhs, const ConnectivityTable& rhs) {
    if(lhs.nbonds() != rhs.nbonds()) return false;
    if(lhs.natoms() != rhs.natoms()) return false;
    if(lhs.bonds() != rhs.bonds()) return false;
    return true;
}

} // namespace chemist::topology
