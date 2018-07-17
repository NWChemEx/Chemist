#include "LibChemist/Atom.hpp"
#include "LibChemist/Implementations/AtomPIMPL.hpp"
#include <tuple>

namespace LibChemist {

using Property      = Atom::Property;
using property_type = double;
using size_type     = Atom::size_type;
using coord_type    = Atom::coord_type;
using prop_map_type = std::map<Property, property_type>;

Atom::Atom() : pimpl_(std::make_unique<detail_::StandAloneAtomPIMPL>()) {}
Atom::Atom(const Atom& rhs) : pimpl_(rhs.pimpl_->clone()) {}
Atom::Atom(Atom&& rhs) noexcept : pimpl_(std::move(rhs.pimpl_)) {}
Atom& Atom::operator=(const Atom& rhs) {
    rhs.pimpl_->clone().swap(pimpl_);
    return *this;
}
Atom& Atom::operator=(Atom&& rhs) noexcept {
    pimpl_.swap(rhs.pimpl_);
    return *this;
}
Atom::Atom(prop_map_type&& props) :
  pimpl_(std::make_unique<detail_::StandAloneAtomPIMPL>(std::move(props))) {}

Atom::~Atom() {}

size_type Atom::count(const Property& prop) const noexcept {
    return pimpl_->count(prop);
}

coord_type& Atom::coords() noexcept { return pimpl_->coords(); }

property_type& Atom::property(const Property& prop) {
    return pimpl_->property(prop);
}

} // namespace LibChemist
