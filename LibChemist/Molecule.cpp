#include "LibChemist/Molecule.hpp"
#include <algorithm>

namespace LibChemist {

double Molecule::charge() const {
    double q{-1.0 * nelectrons()};
    for(const auto& ai : atoms)
        q += ai.properties.at(Atom::Property::charge);
    return q;
}

BasisSet Molecule::get_basis(const std::string& name, bool ungeneralize) const {
    BasisSet rv;
    for(const Atom& ai : atoms) {
        rv = basis_set_concatenate(rv,
                                   ungeneralize ?
                                   ungeneralize_basis_set(ai.get_basis(name)) :
                                   ai.get_basis(name)
        );
    }
    return rv;
}

} // namespace LibChemist
