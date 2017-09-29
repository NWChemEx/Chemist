#include "LibChemist/Atom.hpp"
#include "TestHelpers.hpp"

using namespace LibChemist;


int main()
{
    Tester tester("Testing Atom class");

    std::array<double,3> origin({0.0,0.0,0.0});
    //Manually created uranium atom
    Atom U(origin,92.0,238,238.02891,238.05079,0.0,5.0,
           92.0,3.7038632058973433,3.51489059335156);

    Atom Defaulted;
    tester.test("Defaulted is not uranium",U!=Defaulted);
    Defaulted=U;
    tester.test("Assignment",Defaulted==U);
    Atom Copy(U);
    tester.test("Copy constructor",Copy==Defaulted && Copy==U);
    Atom Move(std::move(U));
    tester.test("Move constructor",Move==Copy && Move==Defaulted);
    U=std::move(Move);
    tester.test("Move assignment",U==Defaulted && U==Copy);

    Atom U2=create_atom({0.0,0.0,0.0},92);
    tester.test("Create Atom",U2==U && U2==Defaulted && U2==Copy);

    Atom U3=create_atom({0.0,0.0,0.0},92,238);
    tester.test("Create Atom",U3==U2 && U3==U && U3==Defaulted && U3==Copy);

    Atom GhU=create_ghost(U);
    tester.test("Make ghost",is_ghost_atom(GhU));

    Atom Chg=create_charge(U.coord,3.8);
    tester.test("Make charge",is_charge(Chg));

    Atom Dummy=create_dummy(U.coord);
    tester.test("Make dummy",is_dummy_atom(Dummy));

    BasisShell shell(ShellType::CartesianGaussian,
                     0,1,
                     std::vector<double>({1.2,2.3,3.4}),
                     std::vector<double>({1.0,2.0,3.0}));
    BasisSet bs;
    bs.add_shell(origin.data(),shell);
    U.add_shell("PRIMARY",shell);

    tester.test("Basis",U.get_basis("PRIMARY")==bs);


    return tester.results();
}
