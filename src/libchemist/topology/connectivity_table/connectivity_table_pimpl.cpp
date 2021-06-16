#include "connectivity_table_pimpl.hpp"
#include <algorithm> // for max, min

namespace libchemist::topology::detail_ {

// ---------------------------- Implementations --------------------------------

ConnectivityTablePIMPL::ConnectivityTablePIMPL(size_type natoms) :
  m_natoms_(natoms), m_connections_(natoms * natoms, false) {}

void ConnectivityTablePIMPL::set_natoms(size_type natoms) {
    if(natoms == m_natoms_) return;
    std::vector<bool> temp(natoms * natoms, false);
    m_connections_.swap(temp);
    m_natoms_ = natoms;
}

ConnectivityTablePIMPL::size_type ConnectivityTablePIMPL::nbonds() const noexcept {
    size_type n_bonds = 0;
    for(size_type i = 0; i < m_natoms_; ++i)
        for(size_type j = i + 1; j < m_natoms_; ++j)
            if(m_connections_[i * m_natoms_ + j]) ++n_bonds;
    return n_bonds;
}

bool ConnectivityTablePIMPL::are_bonded(size_type i, size_type j) const {
    auto [min, max] = sanitize_indices_(i, j);
    return m_connections_[min * m_natoms_ + max];
}

void ConnectivityTablePIMPL::add_bond(size_type i, size_type j) {
    auto [min, max]                       = sanitize_indices_(i, j);
    m_connections_[min * m_natoms_ + max] = true;
}

typename ConnectivityTablePIMPL::bond_list_type ConnectivityTablePIMPL::bonds()
  const {
    const auto n_bonds = nbonds();
    bond_list_type rv(n_bonds);

    for(size_type i = 0, bonds_found = 0; i < m_natoms_; ++i) {
        for(size_type j = i + 1; j < m_natoms_; ++j) {
            if(are_bonded(i, j)) {
                rv[bonds_found++] = pair_type{i, j};
                if(bonds_found == n_bonds) return rv;
            }
        }
    }
    // We can get here if there's actually zero bonds...
    return rv;
}

void ConnectivityTablePIMPL::bounds_check_(size_type atom_i) const {
    if(atom_i < m_natoms_) return;

    const auto msg = std::to_string(atom_i) + " is greater than the number " +
                     "atoms (" + std::to_string(m_natoms_) + ").";
    throw std::out_of_range(msg);
}

typename ConnectivityTablePIMPL::pair_type
ConnectivityTablePIMPL::sanitize_indices_(size_type i, size_type j) const {
    auto min = std::min(i, j);
    auto max = std::max(i, j);
    bounds_check_(min);
    bounds_check_(max);
    if(min == max) {
        std::string msg = "Atom indices can not be equal! (i == j == ";
        msg += std::to_string(i) + ").";
        throw std::out_of_range(msg);
    }
    return pair_type{min, max};
}

}
