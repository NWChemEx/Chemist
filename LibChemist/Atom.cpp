#include "LibChemist/Atom.hpp"
#include "LibChemist/lut/AtomicInfo.hpp"
#include <tuple>

using CoordType = std::array<double,3>;

namespace LibChemist {

Atom::Atom(const CoordType& xyz, double Z_, size_t isotope_,
            double mass_, double isotope_mass_, double charge_,
            double multiplicity_, double nelectrons_, double cov_radius_,
            double vdw_radius_)noexcept:
    Z(Z_),isotope(isotope_),mass(mass_),isotope_mass(isotope_mass_),
    charge(charge_),multiplicity(multiplicity_),nelectrons(nelectrons_),
    cov_radius(cov_radius_),vdw_radius(vdw_radius_),
    coord({xyz[0],xyz[1],xyz[2]})
{
}


bool Atom::operator==(const Atom& rhs)const noexcept
{
    return std::tie(Z,isotope,mass,isotope_mass,charge,multiplicity,
                    nelectrons,cov_radius,vdw_radius,coord)==
           std::tie(rhs.Z,rhs.isotope,rhs.mass,rhs.isotope_mass,rhs.charge,
                    rhs.multiplicity,rhs.nelectrons,rhs.cov_radius,
                    rhs.vdw_radius,rhs.coord);
}


Atom create_atom(const CoordType& xyz, size_t Z)
{
    const size_t isonum=detail_::most_common_isotope(Z);
    return create_atom(xyz,Z,isonum);
}

Atom create_atom(const CoordType& xyz, size_t Z, size_t isonum)
{
    const auto& ai=detail_::atomic_data_.at(Z);
    return Atom(xyz,Z,isonum,ai.mass,detail_::isotope_mass(Z,isonum),0.0,
                ai.multiplicity,Z,ai.covradius,ai.vdwradius);
}

Atom create_ghost(const Atom& atom)noexcept
{
    Atom rv=create_atom(atom.coord,0);
    return rv;
}

bool is_ghost_atom(const Atom& atom)noexcept
{
    return atom.Z==0.0;
}

Atom create_dummy(const CoordType& xyz)noexcept
{
    return create_atom(xyz,9999);
}

bool is_dummy_atom(const Atom& atom)noexcept
{
    return atom.Z==9999.0;
}

Atom create_charge(const CoordType& xyz, double chg)noexcept
{
    Atom rv=create_atom(xyz,999);
    rv.charge=chg;
    return rv;
}

bool is_charge(const Atom& atom)noexcept
{
    return atom.Z==999.0;
}

bool is_real_atom(const Atom& atom)noexcept
{
    return !is_ghost_atom(atom) && !is_dummy_atom(atom) && !is_charge(atom);
}

}//End namespace
