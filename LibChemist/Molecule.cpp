#include "LibChemist/Implementations/MolPIMPL.hpp"
#include "LibChemist/Molecule.hpp"
#include <SDE/Memoization.hpp>
#include <iostream> //For std::endl

namespace LibChemist {

using value_type      = typename Molecule::value_type;
using reference       = typename Molecule::reference;
using const_reference = typename Molecule::const_reference;
using size_type       = typename Molecule::size_type;
using iterator        = typename Molecule::iterator;
using const_iterator  = typename Molecule::const_iterator;

Molecule::Molecule() : pimpl_(std::make_unique<detail_::ContiguousMol>()) {}
Molecule::Molecule(const Molecule& rhs) : Molecule() {
    for(auto ai : rhs) push_back(ai);
    charge()       = rhs.charge();
    multiplicity() = rhs.multiplicity();
}
Molecule::Molecule(Molecule&& rhs) noexcept = default;
Molecule& Molecule::operator                =(const Molecule& rhs) {
    return *this = std::move(Molecule(rhs));
}
Molecule& Molecule::operator=(Molecule&& rhs) noexcept = default;
Molecule::Molecule(std::initializer_list<Atom> atoms) : Molecule() {
    for(auto& a : atoms) push_back(a);
}

Molecule::~Molecule() noexcept = default;

double& Molecule::charge() noexcept { return pimpl_->charge(); }
size_type& Molecule::multiplicity() noexcept { return pimpl_->multiplicity(); }
size_type Molecule::size() const noexcept { return pimpl_->size(); }

size_type Molecule::nelectrons() const noexcept {
    size_type n = 0;
    for(const auto& atom : *this) n += atom.Z();
    return n - std::lround(charge());
}
size_type Molecule::nalpha() const noexcept {
    const size_type nopen   = multiplicity() - 1;
    const size_type nclosed = nelectrons() - nopen;
    return nclosed / 2 + nopen;
}
size_type Molecule::nbeta() const noexcept {
    const size_type nopen   = multiplicity() - 1;
    const size_type nclosed = nelectrons() - nopen;
    return nclosed / 2;
}

reference Molecule::at(size_type i) noexcept { return pimpl_->at(i); }
void Molecule::push_back(value_type atom) {
    pimpl_->push_back(std::move(atom));
}

iterator Molecule::begin() noexcept { return pimpl_->begin(); }
const_iterator Molecule::begin() const noexcept { return pimpl_->begin(); }
iterator Molecule::end() noexcept { return pimpl_->end(); }
const_iterator Molecule::end() const noexcept { return pimpl_->end(); }

void Molecule::hash(bphash::Hasher& h) const {
    h(charge(), multiplicity());
    for(const auto& x : *this) h(x);
}

std::ostream& operator<<(std::ostream& os, const LibChemist::Molecule& mol) {
    for(const auto& ai : mol) os << ai << std::endl;
    return os;
}

bool operator==(const Molecule& lhs, const Molecule& rhs) noexcept {
    // Check for equivalent sizes before iterating through all atoms
    if(lhs.size() != rhs.size()) return false;

    return (std::tie(lhs.charge(), lhs.multiplicity()) ==
            std::tie(rhs.charge(), rhs.multiplicity())) &&
           std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

} // namespace LibChemist
