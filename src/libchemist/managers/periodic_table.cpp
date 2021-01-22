#include "libchemist/managers/detail_/periodic_table_pimpl.hpp"
#include "libchemist/managers/periodic_table.hpp"

namespace libchemist {

using size_type    = typename PeriodicTable::size_type;
using isotope_list = typename PeriodicTable::isotope_list;

PeriodicTable::PeriodicTable() : pimpl_(detail_::nwx_default_ptable()) {}
PeriodicTable::PeriodicTable(const PeriodicTable& rhs) :
  pimpl_(rhs.pimpl_->clone()) {}
PeriodicTable::PeriodicTable(PeriodicTable&& rhs) noexcept = default;
PeriodicTable& PeriodicTable::operator=(const PeriodicTable& rhs) {
    return (*this) = std::move(PeriodicTable(rhs));
}
PeriodicTable& PeriodicTable::operator=(PeriodicTable&& rhs) noexcept = default;
PeriodicTable::~PeriodicTable() noexcept                              = default;

size_type PeriodicTable::max_Z() const noexcept { return pimpl_->max_Z(); }
isotope_list PeriodicTable::isotopes(size_type Z) const {
    return pimpl_->isotopes(Z);
}
size_type PeriodicTable::sym_2_Z(const std::string& sym) const {
    return pimpl_->sym_2_Z(sym);
}
Atom PeriodicTable::get_atom(size_type Z) const { return pimpl_->get_atom(Z); }
Atom PeriodicTable::get_isotope(size_type Z, size_type mass_num) const {
    return pimpl_->get_isotope(Z, mass_num);
}

} // namespace libchemist
