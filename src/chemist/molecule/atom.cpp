#include "atom_pimpl.hpp"
#include "chemist/detail_/hashing.hpp" //For hashing
#include "chemist/molecule/atom.hpp"
#include <iomanip> // For precision of floats

namespace chemist {

using size_type  = typename Atom::size_type;
using coord_type = typename Atom::coord_type;
using name_type  = typename Atom::name_type;
using mass_type  = typename Atom::mass_type;

Atom::Atom() : pimpl_(std::make_unique<detail_::ContiguousAtomPIMPL>()) {}
Atom::Atom(const Atom& rhs) : pimpl_(rhs.pimpl_->clone()) {}
Atom::Atom(Atom&& rhs) noexcept = default;
Atom& Atom::operator            =(const Atom& rhs) {
    // Note using the copy ctor would reallocate the buffers, this way we skip
    // the reallocation
    name()   = rhs.name();
    Z()      = rhs.Z();
    mass()   = rhs.mass();
    coords() = rhs.coords();
    return *this;
}
Atom& Atom::operator=(Atom&& rhs) noexcept = default;
Atom::Atom(std::unique_ptr<detail_::AtomPIMPL> pimpl) :
  pimpl_(std::move(pimpl)) {}
Atom::~Atom() noexcept = default;

name_type& Atom::name() noexcept { return pimpl_->name(); }

size_type& Atom::Z() noexcept { return pimpl_->at_num(); }

coord_type& Atom::coords() noexcept { return pimpl_->coords(); }

mass_type& Atom::mass() noexcept { return pimpl_->mass(); }

void Atom::hash(detail_::Hasher& h) const { h(Z(), coords(), mass(), name()); }

bool operator==(const Atom& lhs, const Atom& rhs) noexcept {
    return std::tie(lhs.Z(), lhs.coords(), lhs.mass(), lhs.name()) ==
           std::tie(rhs.Z(), rhs.coords(), rhs.mass(), rhs.name());
}

std::ostream& operator<<(std::ostream& os, const Atom& ai) {
    os << ai.name() << std::fixed << std::setprecision(15) << " " << ai[0]
       << " " << ai[1] << " " << ai[2];
    return os;
}

} // namespace chemist
