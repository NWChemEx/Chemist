#include "LibChemist/Implementations/MolPIMPL.hpp"
#include "LibChemist/Molecule.hpp"
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
reference Molecule::at(size_type i) noexcept { return pimpl_->at(i); }
void Molecule::push_back(value_type atom) {
    pimpl_->push_back(std::move(atom));
}

iterator Molecule::begin() noexcept { return pimpl_->begin(); }
const_iterator Molecule::begin() const noexcept { return pimpl_->begin(); }
iterator Molecule::end() noexcept { return pimpl_->end(); }
const_iterator Molecule::end() const noexcept { return pimpl_->end(); }

std::ostream& operator<<(std::ostream& os, const Molecule& mol) {
    for(const auto& ai : mol) os << ai << std::endl;
    return os;
}

} // namespace LibChemist
