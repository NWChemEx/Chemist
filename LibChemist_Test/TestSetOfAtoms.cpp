#include "LibChemist/SetOfAtoms.hpp"

#include "catch/catch.hpp"

using namespace LibChemist;

TEST_CASE("SetOfAtoms class")
{
    //A point at the origin
    std::array<double,3> origin({0.0,0.0,0.0});
    //Will be 6 points +/- 3.76 a.u. from the origin with octahedral symmetry
    std::vector<std::array<double,3>> Fpos(6,origin);
    //Will be the 7 atoms in UF6
    std::array<Atom,7> corr_UF6;

    //Uranium is at origin
    corr_UF6[0]=create_atom(origin,92);
    //Put Fluorines octahedrally around it
    for(size_t i=0,counter=0;i<3;++i)
        for(double coord:{3.76,-3.76})
        {
            Fpos[counter][i]=coord;
            corr_UF6[counter+1]=create_atom(Fpos[counter],9);
            ++counter;
        }

    //Test default constructor
    SetOfAtoms UF6;
    REQUIRE(UF6.charge==0.0);
    REQUIRE(UF6.multiplicity==1.0);
    REQUIRE(UF6.size()==0);
    REQUIRE_FALSE(UF6.count(corr_UF6[0]));

    //Fill the defaulted system
    for(const auto& ai: corr_UF6)
        UF6.insert(ai);
    REQUIRE(UF6.size()==7);

    //Test the atoms we inserted
    for(size_t i=0;i<7;++i)
    {
        const std::string label="Atom # "+std::to_string(i);
        REQUIRE(UF6[i]==corr_UF6[i]);
        REQUIRE(UF6.count(corr_UF6[i]));
    }

    //Test other constructors and assignments
    SetOfAtoms Copy(UF6);
    REQUIRE(Copy==UF6);

    SetOfAtoms Moved(std::move(Copy));
    REQUIRE(Moved==UF6);

    Copy=std::move(Moved);
    REQUIRE(Copy==UF6);

    Moved=Copy;
    REQUIRE(Moved==Copy);
    REQUIRE(Moved==UF6);

    //Iterators
    size_t counter=0;
    for(const Atom& ai: UF6)
        REQUIRE(ai==UF6[counter++]);
    counter=0;
    for(const Atom& ai: const_cast<const SetOfAtoms&>(UF6))
        REQUIRE(ai==UF6[counter++]);

    //Basis set stuff
    std::map<size_t,std::vector<BasisShell>> basis;
    basis[92].push_back(
                BasisShell(ShellType::CartesianGaussian,-1,2,
                    std::vector<double>({3.0,4.0}),
                    std::vector<double>({5.0,6.0,7.0,8.0})));
    auto UF6_with_basis=apply_basis_set("PRIMARY",basis,UF6);

    BasisSet corr_bs;
    corr_bs.add_shell(origin.data(),basis[92][0]);
    REQUIRE(corr_bs==get_general_basis("PRIMARY",UF6_with_basis));

    BasisSet corr_ungen_bs;
    corr_ungen_bs.add_shell(origin.data(),
        BasisShell(ShellType::CartesianGaussian,0,1,
                   std::vector<double>({3.0,4.0}),
                   std::vector<double>({5.0,6.0})));
    corr_ungen_bs.add_shell(origin.data(),
        BasisShell(ShellType::CartesianGaussian,1,1,
                   std::vector<double>({3.0,4.0}),
                   std::vector<double>({7.0,8.0})));
    REQUIRE(corr_ungen_bs==get_basis("PRIMARY",UF6_with_basis));
}
