#include "LibChemist/Atom.hpp"
#include "LibChemist/Implementations/AtomPIMPL.hpp"
#include <iomanip> // For precision of floats

namespace LibChemist {

using Property      = Atom::Property;
using property_type = Atom::property_type;
using size_type     = Atom::size_type;
using coord_type    = Atom::coord_type;
using name_type     = Atom::name_type;

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


Atom::~Atom() = default;

size_type Atom::count(const Property& prop) const noexcept {
    return pimpl_->count(prop);
}

name_type& Atom::name() noexcept {return pimpl_->name(); }

size_type& Atom::at_num() noexcept { return pimpl_->at_num(); }

coord_type& Atom::coords() noexcept { return pimpl_->coords(); }

property_type& Atom::property(const Property& prop) {
    return pimpl_->property(prop);
}

void Atom::set_property(const Property& prop, property_type value) {
    pimpl_->set_property(prop, value);
}

} // namespace LibChemist

std::ostream& operator<<(std::ostream& os, const LibChemist::Atom& ai) {
    os << ai.name() << std::fixed << std::setprecision(15) << " " << ai[0]
       << " " << ai[1] << " " << ai[2];
    return os;
}
