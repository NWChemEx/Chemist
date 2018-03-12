#include "LibChemist/Atom.hpp"
#include <tuple>

namespace LibChemist {

using xyz_type = typename Atom::xyz_type;
using Property = typename Atom::atom_property_type;
using properties_map = typename Atom::properties_map;
using basis_lut_type = typename Atom::basis_lut_type;

namespace detail_ {

///Makes an atom where all properties are zero initialized
static Atom null_atom(){
    Atom rv;
    rv.properties = properties_map{ {Property::Z, 0.0}, {Property::mass, 0.0},
                                    {Property::isotope_mass, 0.0},
                                    {Property::cov_radius, 0.0},
                                    {Property::vdw_radius, 0.0}};
    return rv;
}

}

bool Atom::operator==(const Atom& rhs) const noexcept {
    return std::tie(coords, properties, bases) ==
           std::tie(rhs.coords, rhs.properties, rhs.bases);
}

Atom create_ghost(const Atom& atom) {
    auto rv = create_dummy(atom);
    rv.bases = atom.bases;
    return rv;
}

bool is_ghost_atom(const Atom& atom) {
    return atom == create_ghost(atom);
}

Atom create_dummy(const Atom& atom) {
    auto rv = detail_::null_atom();
    rv.coords = atom.coords;
    return rv;
}

bool is_dummy_atom(const Atom& atom)  {
    return atom == create_dummy(atom);
}

Atom create_charge(const Atom& atom, double chg) {
    auto rv = create_dummy(atom);
    rv.properties[Property::Z] = chg;
    return rv;
}

bool is_charge(const Atom& atom) {
    return atom == create_charge(atom, atom.properties.at(Property::Z));
}

bool is_real_atom(const Atom& atom) {
    return !is_ghost_atom(atom) && !is_dummy_atom(atom) && !is_charge(atom);
}

} // namespace LibChemist
