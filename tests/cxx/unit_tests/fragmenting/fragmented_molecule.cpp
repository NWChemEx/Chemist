#include "../test_helpers.hpp"
#include <chemist/fragmenting/fragmented_molecule.hpp>

using namespace chemist;
using namespace chemist::fragmenting;

using types2test = std::tuple<Molecule, const Molecule>;

TEMPLATE_LIST_TEST_CASE("FragmentedMolecule", "", types2test) {
    using class_type             = FragmentedMolecule<TestType>;
    using molecule_type          = typename class_type::supersystem_type;
    using atom_type              = typename molecule_type::atom_type;
    using fragmented_nuclei_type = typename class_type::fragmented_nuclei_type;
    using charges_container      = typename class_type::charge_container;
    using multiplicities_container =
      typename class_type::multiplicity_container;

    atom_type h("H", 1ul, 1.0, 0.0, 0.0, 0.0);
    atom_type he("He", 2ul, 4.0, 1.0, 2.0, 3.0);

    molecule_type empty_mol;
    molecule_type value_mol{h, he};
    molecule_type cation(1, 1, {h, he});
    fragmented_nuclei_type frags(value_mol.nuclei());
    frags.insert({0});
    frags.insert({1});

    charges_container qs{1, 0};
    multiplicities_container ms{1, 1};

    class_type defaulted;
    class_type empty(empty_mol);
    class_type value_no_frags(value_mol);
    class_type value_frags(frags, value_mol.charge(), value_mol.multiplicity());
    class_type value(frags, 1, 1, qs, ms);

    molecule_type frag0(0, 2, {h});
    molecule_type frag1(0, 1, {he});
    molecule_type cation0(1, 1, {h});
    molecule_type cation1(0, 1, {he});

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.supersystem() == empty_mol);
            REQUIRE(defaulted.size() == 0);
        }

        SECTION("Molecule") {
            REQUIRE(empty.supersystem() == empty_mol);
            REQUIRE(empty.size() == 0);

            REQUIRE(value_no_frags.supersystem() == value_mol);
            REQUIRE(value_no_frags.size() == 0);
        }

        SECTION("Frags, charge, multiplicity") {
            REQUIRE(value_frags.supersystem() == value_mol);
            REQUIRE(value_frags.size() == 2);
            REQUIRE(value_frags[0] == frag0);
            REQUIRE(value_frags[1] == frag1);

            REQUIRE_THROWS_AS(class_type(frags, 0, 1), std::runtime_error);
        }

        SECTION("Frags, charge, multiplicity, charges, multiplicities") {
            REQUIRE(value.supersystem() == cation);
            REQUIRE(value.size() == 2);
            REQUIRE(value[0] == cation0);
            REQUIRE(value[1] == cation1);
        }

        SECTION("Copy") {
            test_chemist::test_copy_ctor(defaulted);
            test_chemist::test_copy_ctor(empty);
            test_chemist::test_copy_ctor(value_no_frags);
            test_chemist::test_copy_ctor(value_frags);
            test_chemist::test_copy_ctor(value);
        }

        SECTION("Move") {
            test_chemist::test_move_ctor(defaulted);
            test_chemist::test_move_ctor(empty);
            test_chemist::test_move_ctor(value_no_frags);
            test_chemist::test_move_ctor(value_frags);
            test_chemist::test_move_ctor(value);
        }

        SECTION("Copy assignment") {
            test_chemist::test_copy_assignment(defaulted);
            test_chemist::test_copy_assignment(empty);
            test_chemist::test_copy_assignment(value_no_frags);
            test_chemist::test_copy_assignment(value_frags);
            test_chemist::test_copy_assignment(value);
        }

        SECTION("Move assignment") {
            test_chemist::test_move_assignment(defaulted);
            test_chemist::test_move_assignment(empty);
            test_chemist::test_move_assignment(value_no_frags);
            test_chemist::test_move_assignment(value_frags);
            test_chemist::test_move_assignment(value);
        }
    }

    SECTION("operator[]") {
        REQUIRE(value_frags[0] == frag0);
        REQUIRE(value_frags[1] == frag1);
        REQUIRE(value[0] == cation0);
        REQUIRE(value[1] == cation1);
    }

    SECTION("operator[]() const") {
        REQUIRE(std::as_const(value_frags)[0] == frag0);
        REQUIRE(std::as_const(value_frags)[1] == frag1);
        REQUIRE(std::as_const(value)[0] == cation0);
        REQUIRE(std::as_const(value)[1] == cation1);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(empty.size() == 0);
        REQUIRE(value_no_frags.size() == 0);
        REQUIRE(value_frags.size() == 2);
        REQUIRE(value.size() == 2);
    }

    SECTION("supersystem()") {
        REQUIRE(defaulted.supersystem() == empty_mol);
        REQUIRE(empty.supersystem() == empty_mol);
        REQUIRE(value_no_frags.supersystem() == value_mol);
        REQUIRE(value_frags.supersystem() == value_mol);
        REQUIRE(value.supersystem() == cation);
    }

    SECTION("supersystem() const") {
        REQUIRE(std::as_const(defaulted).supersystem() == empty_mol);
        REQUIRE(std::as_const(empty).supersystem() == empty_mol);
        REQUIRE(std::as_const(value_no_frags).supersystem() == value_mol);
        REQUIRE(std::as_const(value_frags).supersystem() == value_mol);
        REQUIRE(std::as_const(value).supersystem() == cation);
    }

    SECTION("swap") {
        class_type lhs_copy(defaulted);
        class_type rhs_copy(value);
        defaulted.swap(value);
        REQUIRE(defaulted == rhs_copy);
        REQUIRE(value == lhs_copy);
    }

    SECTION("operator==") {
        SECTION("Defaulted vs. defaulted") {
            class_type defaulted2;
            REQUIRE(defaulted == defaulted2);
        }

        SECTION("Defaulted vs. empty") { REQUIRE(defaulted == empty); }

        SECTION("Defaulted vs. no frags") {
            REQUIRE_FALSE(defaulted == value_no_frags);
        }

        SECTION("Defaulted vs. frags") {
            REQUIRE_FALSE(defaulted == value_frags);
        }

        SECTION("empty vs. empty") {
            class_type empty2(empty_mol);
            REQUIRE(empty == empty2);
        }

        SECTION("empty vs. no frags") {
            REQUIRE_FALSE(empty == value_no_frags);
        }

        SECTION("empty vs. frags") { REQUIRE_FALSE(empty == value_frags); }

        SECTION("no_frags vs. no frags") {
            class_type value_no_frags2(value_mol);
            REQUIRE(value_no_frags == value_no_frags2);
        }

        SECTION("no frags vs. frags") {
            REQUIRE_FALSE(value_no_frags == value_frags);
        }

        SECTION("same fragments") {
            class_type value_frags2(frags, 0, 2);
            REQUIRE(value_frags == value_frags2);
        }
        SECTION("different fragments") {
            fragmented_nuclei_type frags2(value_mol.nuclei());
            frags2.insert({0, 1});
            class_type value_frags2(frags2, 0, 2);
            REQUIRE_FALSE(value_frags == value_frags2);
        }

        // N.b. I can't come up with a physically meaningful scenario where the
        // overall charges or multiplicities are different without also having
        // to
        //  change the fragment's charges or multiplicities

        SECTION("different fragment charges") {
            charges_container charges2{0, 1};
            atom_type h1("h", 1ul, 1.0, 3.0, 4.0, 5.0);
            molecule_type h2(1, 2, {h, h1});
            fragmented_nuclei_type frags2(h2.nuclei());
            frags2.insert({0});
            frags2.insert({1});
            class_type lhs(frags2, 1, 2, qs, ms);
            class_type rhs(frags2, 1, 2, charges2, ms);
            REQUIRE_FALSE(lhs == rhs);
        }
        SECTION("different fragment multiplicities") {
            multiplicities_container ms2{2, 4};
            class_type value_frags2(frags, 0, 2, qs, ms2);
            REQUIRE_FALSE(value_frags == value_frags2);
        }
    }

    SECTION("operator!=") {
        // N.b. this operator just negates operator==, okay to spot check
        REQUIRE(empty != value_frags);
        REQUIRE_FALSE(defaulted != empty);
    }
}
