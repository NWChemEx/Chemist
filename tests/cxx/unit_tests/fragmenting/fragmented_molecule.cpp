#include "../test_helpers.hpp"
#include <chemist/fragmenting/fragmented_molecule.hpp>

using types2test = std::tuple<Molecule, const Molecule>;

TEMPLATE_LIST_TEST_CASE("FragmentedMolecule", "", types2test) {
    using class_type = FragmentedMolecule<TestType>;

    class_type defaulted;

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.is_null());
            REQUIRE(defaulted.size() == 0);
        }
    }
}
