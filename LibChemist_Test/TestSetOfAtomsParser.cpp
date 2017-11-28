#include "LibChemist/SetOfAtomsParser.hpp"
#include "TestHelpers.hpp"

using namespace LibChemist;

//A fake xyz example
std::string xyz_example=
" -6.7 3\n"
"         He 0.1 .1 0.0    \n"
" HE 1.1 0.1 0.0\n";

std::string xyz_example2=
" 2\n"
"\n"
"         He 0.1 .1 0.0    \n"
" HE 1.1 0.1 0.0\n";

std::string h2o_example=
"3\n"
"\n"
"H    0.000000000000000   1.579252144093028   2.174611055780858\n"
"O    0.000000000000000   0.000000000000000   0.000000000000000\n"
"H    0.000000000000000   1.579252144093028  -2.174611055780858\n"
" \n";

int main()
{
    Tester tester("Testing set of atoms parsing capabilities");
    std::array<double,3> carts1({0.1,0.1,0.0}),carts2({1.1,0.1,0.0});
    std::array<Atom,2> Hes({create_atom(carts1,2),create_atom(carts2,2)});
    SetOfAtoms corr;
    corr.insert(Hes[0]);
    corr.insert(Hes[1]);
    corr.insert(Hes[0]);
    corr.charge=-6.7;
    corr.multiplicity=3.0;

    std::stringstream ss(xyz_example);
    SetOfAtoms mol=parse_SetOfAtoms_file(ss,XYZParser());
    tester.test("Parsed xyz file",corr==mol);

    SetOfAtoms corr2;
    corr2.insert(Hes[0]);
    corr2.insert(Hes[1]);
    corr2.insert(Hes[0]);
  
    std::stringstream ss2(xyz_example2);
    SetOfAtoms mol2=parse_SetOfAtoms_file(ss2,XYZWikiParser());
    tester.test("Parsed xyz file",corr2==mol2);

    SetOfAtoms corr_h2o;
    corr_h2o.insert(create_atom({0.000000000000000,1.579252144093028,2.174611055780858},1));
    corr_h2o.insert(create_atom({0.000000000000000,0.000000000000000,0.000000000000000},8));
    corr_h2o.insert(create_atom({0.000000000000000,1.579252144093028,-2.174611055780858},1));

    std::stringstream ss3(h2o_example);
    SetOfAtoms h2o=parse_SetOfAtoms_file(ss3,XYZWikiParser());
    tester.test("Parsed xyz file",corr_h2o==h2o);
    return tester.results();
}
