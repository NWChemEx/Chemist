#include "../../../../test_helpers.hpp"
#include <chemist/chemical_system/chemical_system/chemical_system_view/detail_/chemical_system_view_pimpl.hpp>
using namespace chemist;

using types2test = std::tuple<ChemicalSystem, const ChemicalSystem>;

TEMPLATE_LIST_TEST_CASE("ChemicalSystemViewPIMPL", "", types2test) {
    // Are we testing an alias of a read-only ChemicalSystem?
    constexpr bool is_const_alias =
      std::is_same_v<TestType, std::remove_cv_t<TestType>>;

    using pimpl_type         = detail_::ChemicalSystemViewPIMPL<TestType>;
    using molecule_reference = typename pimpl_type::molecule_reference;
    using molecule_type      = typename molecule_reference::molecule_type;
    using atom_type          = typename molecule_type::atom_type;

    atom_type h("H", 1ul, 1.0, 0.0, 0.0, 0.0);
    atom_type he("He", 2ul, 4.0, 1.0, 2.0, 3.0);
    molecule_type empty_mol;
    molecule_type values_mol(0, 2, {h, he});

    pimpl_type empty(empty_mol);
    pimpl_type values(values_mol);

    SECTION("CTors") {
        SECTION("Value") {
            REQUIRE(empty.molecule() == empty_mol);
            REQUIRE(values.molecule() == values_mol);
        }

        SECTION("Copy") {
            test_chemist::test_copy_ctor(empty);
            test_chemist::test_copy_ctor(values);
        }
    }

    SECTION("operator==") {
        SECTION("Empty vs. empty") {
            pimpl_type empty2(empty_mol);
            REQUIRE(empty == empty2);
        }

        SECTION("Empty vs. non-empty") { REQUIRE_FALSE(empty == values); }

        SECTION("Same values") {
            pimpl_type values2(values_mol);
            REQUIRE(values == values2);
        }

        SECTION("Different values") {
            molecule_type values_mol2(1, 1, {h, he});
            pimpl_type values2(values_mol2);
            REQUIRE_FALSE(values == values2);
        }
    }
}
