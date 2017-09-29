#include "LibChemist/lut/AtomicInfo.hpp"
#include "TestHelpers.hpp"

using namespace LibChemist::detail_;


int main()
{
    Tester tester("Testing AtomicInfo class");

    //Ensure Z2sym_ and sym2Z_ are consistent
    for(const auto& x: Z2sym_)
        tester.test(x.second,sym2Z_.at(x.second)==x.first);

    //Spot check Z2sym_ and sym2Z_
    tester.test("Uranium symbol",Z2sym_.at(92)=="U");
    tester.test("Helium atomic number",sym2Z_.at("He")==2);

    //Spot check atomic_data_
    AtomicData corr_Os({76,"Os","Osmium",5,"D",190.23,190.2,190.26,
                   2.7212056206592723,0.0,
             {
               { 184, 183.952489, 183.95248, 183.952498, 0.0002, 0.0, 0.0004 },
               { 186, 185.95384, 185.95383, 185.95385, 0.0159, 0.0095, 0.0223 },
               { 187, 186.95575, 186.95574, 186.95576, 0.0196, 0.0179, 0.0213 },
               { 188, 187.95584, 187.95583, 187.95585, 0.1324, 0.1297, 0.1351 },
               { 189, 188.95814, 188.95812, 188.95816, 0.1615, 0.1592, 0.1638 },
               { 190, 189.95844, 189.95842, 189.95846, 0.2626, 0.2606, 0.2646 },
               { 192, 191.96148, 191.96146, 191.9615, 0.4078, 0.4046, 0.411 },
             },
         });
    const auto& Os=atomic_data_.at(76);
    tester.test("Z",corr_Os.Z==Os.Z);
    tester.test("Symbol",corr_Os.sym==Os.sym);
    tester.test("Name",corr_Os.name==Os.name);
    tester.test("Multiplicity",corr_Os.multiplicity==Os.multiplicity);
    tester.test("Term Symbol",corr_Os.termsym==Os.termsym);
    const std::string Iwm="Isotope weighted mass";
    tester.test(Iwm,corr_Os.mass==Os.mass);
    tester.test(Iwm+" lower bound",corr_Os.mass_low==Os.mass_low);
    tester.test(Iwm+" upper bound",corr_Os.mass_high==Os.mass_high);
    tester.test("Covalent radius",corr_Os.covradius==Os.covradius);
    tester.test("VDW radius",corr_Os.vdwradius==Os.vdwradius);
    tester.test("# of isotopes",corr_Os.isotopes.size()==Os.isotopes.size());
    for(size_t i=0;i<corr_Os.isotopes.size();++i)
    {
        const auto& corr_Isotope=corr_Os.isotopes[i];
        const auto& isotope=Os.isotopes[i];
        const std::string in="Isotope #"+std::to_string(i);
        tester.test(in+" number",corr_Isotope.isonum==isotope.isonum);
        tester.test(in+" mass",corr_Isotope.mass==isotope.mass);
        tester.test(in+" mass low",corr_Isotope.mass_low==isotope.mass_low);
        tester.test(in+" mass high",corr_Isotope.mass_high==isotope.mass_high);
        tester.test(in+" abundance",corr_Isotope.abund==isotope.abund);
        tester.test(in+" abund low",corr_Isotope.abund_low==isotope.abund_low);
        tester.test(in+" abund high",corr_Isotope.abund_high==isotope.abund_high);
    }
    //Spot check most_common_isotope
    tester.test("Most common isotope",most_common_isotope(76)==192);

    //Spot check isotope_mass
    tester.test("Isotope mass",isotope_mass(1,2)==2.0141017781);


    return tester.results();
}
