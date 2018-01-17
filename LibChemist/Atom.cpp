#include "LibChemist/Atom.hpp"
#include "LibChemist/lut/AtomicInfo.hpp"
#include <tuple>

using CoordType = std::array<double,3>;

namespace LibChemist {

bool Atom::operator==(const Atom& rhs)const noexcept
{
    return std::tie(coord,properties_, basis_sets_)==
           std::tie(rhs.coord,rhs.properties_, rhs.basis_sets_);
}


Atom create_atom(const CoordType& xyz, size_t Z)
{
    const size_t isonum=detail_::most_common_isotope(Z);
    return create_atom(xyz,Z,isonum);
}

Atom create_atom(const CoordType& xyz, size_t Z, size_t isonum)
{
    const auto& ai=detail_::atomic_data_.at(Z);
    std::unordered_map<AtomProperty,double> props;
    props[AtomProperty::Z]=Z;
    props[AtomProperty::mass]=ai.mass;
    props[AtomProperty::isotope_mass]=detail_::isotope_mass(Z,isonum);
    props[AtomProperty::multiplicity]=ai.multiplicity;
    props[AtomProperty::charge]=0.0;
    props[AtomProperty::nelectrons]=Z;
    props[AtomProperty::cov_radius]=ai.covradius;
    props[AtomProperty::vdw_radius]=ai.vdwradius;
    return Atom(xyz,props);
}

Atom create_ghost(const Atom& atom)noexcept
{
    return create_atom(atom.coord,0);
}

bool is_ghost_atom(const Atom& atom)noexcept
{
    return atom.property(AtomProperty::Z)==0.0;
}

Atom create_dummy(const CoordType& xyz)noexcept
{
    return create_atom(xyz,9999);
}

bool is_dummy_atom(const Atom& atom)noexcept
{
    return atom.property(AtomProperty::Z)==9999.0;
}

Atom create_charge(const CoordType& xyz, double chg)noexcept
{
    Atom rv=create_atom(xyz,999);
    rv.property(AtomProperty::charge)=chg;
    return rv;
}

bool is_charge(const Atom& atom)noexcept
{
    return atom.property(AtomProperty::Z)==999.0;
}

bool is_real_atom(const Atom& atom)noexcept
{
    return !is_ghost_atom(atom) && !is_dummy_atom(atom) && !is_charge(atom);
}

}//End namespace
