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

int main()
{
    Tester tester("Testing set of atoms parsing capabilities");
    std::array<double,3> carts1({0.1,0.1,0.0}),carts2({1.1,0.1,0.0});
    std::array<Atom,2> Hes({create_atom(carts1,2),create_atom(carts2,2)});
    SetOfAtoms corr;
    corr.insert(Hes[0]);
    corr.insert(Hes[1]);
    corr.charge=-6.7;
    corr.multiplicity=3.0;

    std::stringstream ss(xyz_example);
    SetOfAtoms mol=parse_SetOfAtoms_file(ss,XYZParser());
    tester.test("Parsed xyz file",corr==mol);
    return tester.results();

    std::stringstream ss(xyz_example2);
    SetOfAtoms mol=parse_SetOfAtoms_file(ss,XYZParser());
    tester.test("Parsed xyz file",corr==mol);
    return tester.results();
}
