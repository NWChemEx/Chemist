#include "../../../test_helpers.hpp"
#include <chemist/chemical_system/chemical_system/chemical_system_view/chemical_system_view.hpp>
using namespace chemist;

using types2test = std::tuple<ChemicalSystem, const ChemicalSystem>;

TEMPLATE_LIST_TEST_CASE("ChemicalSystemView", "", types2test) {
    // Are we testing an alias of a read-only ChemicalSystem?
    constexpr bool is_mutable_alias =
      std::is_same_v<TestType, std::remove_cv_t<TestType>>;

    using class_type           = ChemicalSystemView<TestType>;
    using chemical_system_type = typename class_type::chemical_system_type;
    using molecule_reference   = typename class_type::molecule_reference;
    using molecule_type        = typename molecule_reference::molecule_type;
    using atom_type            = typename molecule_type::atom_type;

    atom_type h("H", 1ul, 1.0, 0.0, 0.0, 0.0);
    atom_type he("He", 2ul, 4.0, 1.0, 2.0, 3.0);
    molecule_type empty_mol;
    molecule_type values_mol(0, 2, {h, he});

    chemical_system_type defaulted_cs;
    chemical_system_type values_cs(values_mol);

    class_type defaulted;
    class_type empty(empty_mol);
    class_type values(values_mol);

    SECTION("CTors") {
        SECTION("Defaulted") { REQUIRE(defaulted.molecule() == empty_mol); }

        SECTION("From ChemicalSystem") {
            REQUIRE(class_type(defaulted_cs) == defaulted);
            REQUIRE(class_type(values_cs) == values);
        }

        SECTION("Value") {
            REQUIRE(empty.molecule() == empty_mol);
            REQUIRE(values.molecule() == values_mol);
        }

        SECTION("Copy") {
            test_chemist::test_copy_ctor(empty);
            test_chemist::test_copy_ctor(values);
        }

        SECTION("Move") {
            test_chemist::test_move_ctor(empty);
            test_chemist::test_move_ctor(values);
        }

        SECTION("Copy assignment") {
            test_chemist::test_copy_assignment(empty);
            test_chemist::test_copy_assignment(values);
        }

        SECTION("Move assignment") {
            test_chemist::test_move_assignment(empty);
            test_chemist::test_move_assignment(values);
        }
    }

    SECTION("molecule") {
        REQUIRE(empty.molecule() == empty_mol);
        REQUIRE(values.molecule() == values_mol);

        if constexpr(is_mutable_alias) {
            // Verify it's writeable
            values.molecule()[0].x() = 1.0;
            REQUIRE(values.molecule()[0].x() == 1.0);
            REQUIRE(values_mol[0].x() == 1.0);
        }
    }

    SECTION("molecule() const") {
        REQUIRE(std::as_const(empty).molecule() == empty_mol);
        REQUIRE(std::as_const(values).molecule() == values_mol);
    }

    SECTION("swap") {
        class_type lhs_copy(defaulted);
        class_type rhs_copy(values);

        defaulted.swap(values);

        REQUIRE(defaulted == rhs_copy);
        REQUIRE(values == lhs_copy);
    }

    SECTION("operator==") {
        SECTION("Empty vs. empty") {
            class_type empty2(empty_mol);
            REQUIRE(empty == empty2);
        }

        SECTION("Empty vs. non-empty") { REQUIRE_FALSE(empty == values); }

        SECTION("Same values") {
            class_type values2(values_mol);
            REQUIRE(values == values2);
        }

        SECTION("Different values") {
            molecule_type values_mol2(1, 1, {h, he});
            class_type values2(values_mol2);
            REQUIRE_FALSE(values == values2);
        }

        SECTION("Compared to ChemicalSystem") {
            // N.b. this just dispatches to the same back end as
            // operator==(ChemicalSystemView) so spot-checking suffices

            REQUIRE(defaulted == defaulted_cs);
            REQUIRE(values == values_cs);
            REQUIRE_FALSE(defaulted == values_cs);

            // Can call symmetrically
            REQUIRE(defaulted_cs == defaulted);
            REQUIRE(values_cs == values);
            REQUIRE_FALSE(values_cs == defaulted);
        }
    }

    SECTION("operator!=") {
        // N.b. these just negate operator==, so suffices to spot-check
        REQUIRE(defaulted != values);
        REQUIRE_FALSE(defaulted != class_type{});

        SECTION("Compared to ChemicalSystem") {
            REQUIRE(defaulted != values_cs);
            REQUIRE_FALSE(defaulted != defaulted_cs);

            // Can call symmetrically
            REQUIRE(values_cs != defaulted);
            REQUIRE_FALSE(defaulted_cs != defaulted);
        }
    }
}
