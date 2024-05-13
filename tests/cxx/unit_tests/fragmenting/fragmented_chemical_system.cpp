#include "../test_helpers.hpp"
#include <chemist/fragmenting/fragmented_chemical_system.hpp>

using namespace chemist;
using namespace chemist::fragmenting;

using types2check = std::tuple<ChemicalSystem, const ChemicalSystem>;

TEMPLATE_LIST_TEST_CASE("FragmentedChemicalSystem", "", types2check) {
    using class_type           = FragmentedChemicalSystem<TestType>;
    using chemical_system_type = typename class_type::supersystem_type;
    using frags_type           = typename class_type::fragmented_molecule_type;
    using frag_nuclei_type     = typename frags_type::fragmented_nuclei_type;
    using molecule_type        = typename frags_type::supersystem_type;
    using atom_type            = typename molecule_type::atom_type;

    atom_type h("H", 1ul, 1.0, 0.0, 0.0, 0.0);
    atom_type he("He", 2ul, 4.0, 1.0, 2.0, 3.0);
    molecule_type heh{he, h};
    frag_nuclei_type nuclei_frags(heh.nuclei());
    nuclei_frags.insert({0});
    nuclei_frags.insert({1});
    frags_type empty_frags;
    frags_type value_frags(nuclei_frags, 0, 2);

    chemical_system_type empty_cs;
    chemical_system_type value_cs(heh);

    class_type defaulted;
    // class_type empty(empty_frags);
    // class_type value(value_frags);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.supersystem() == empty_cs);
        }
    }

    SECTION("operator==") {
        SECTION("Default vs default") {
            class_type other;
            REQUIRE(defaulted == other);
        }
    }
}
