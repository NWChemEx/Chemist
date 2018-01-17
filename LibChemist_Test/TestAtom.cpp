#include "LibChemist/Atom.hpp"

#include "catch/catch.hpp"

using namespace LibChemist;


TEST_CASE("Atom class")
{
    std::array<double,3> origin({0.0,0.0,0.0});
    std::unordered_map<AtomProperty,double> props;
    props[AtomProperty::Z]=92.0;
    props[AtomProperty::mass]=238.02891;
    props[AtomProperty::isotope_mass]=238.05079;
    props[AtomProperty::charge]=0.0;
    props[AtomProperty::multiplicity]=5.0;
    props[AtomProperty::nelectrons]=92.0;
    props[AtomProperty::cov_radius]=3.7038632058973433;
    props[AtomProperty::vdw_radius]=3.51489059335156;
    //Manually created uranium atom
    Atom U(origin,props);

    Atom Defaulted;
    REQUIRE(U!=Defaulted);
    Defaulted=U;
    REQUIRE(Defaulted==U);
    Atom Copy(U);
    REQUIRE(Copy==Defaulted);
    REQUIRE(Copy==U);
    Atom Move(std::move(U));
    REQUIRE(Move==Copy);
    REQUIRE(Move==Defaulted);
    U=std::move(Move);
    REQUIRE(U==Defaulted);
    REQUIRE(U==Copy);

    Atom U2=create_atom({0.0,0.0,0.0},92);
    REQUIRE(U2==U); 
    REQUIRE(U2==Defaulted); 
    REQUIRE(U2==Copy);

    Atom U3=create_atom({0.0,0.0,0.0},92,238);
    REQUIRE(U3==U2);
    REQUIRE(U3==U);
    REQUIRE(U3==Defaulted);
    REQUIRE(U3==Copy);

    Atom GhU=create_ghost(U);
    REQUIRE(is_ghost_atom(GhU));

    Atom Chg=create_charge(U.coord,3.8);
    REQUIRE(is_charge(Chg));

    Atom Dummy=create_dummy(U.coord);
    REQUIRE(is_dummy_atom(Dummy));

    BasisShell shell(ShellType::CartesianGaussian,
                     0,1,
                     std::vector<double>({1.2,2.3,3.4}),
                     std::vector<double>({1.0,2.0,3.0}));
    BasisSet bs;
    bs.add_shell(origin.data(),shell);
    U.add_shell("PRIMARY",shell);

    REQUIRE(U.get_basis("PRIMARY")==bs);
}
