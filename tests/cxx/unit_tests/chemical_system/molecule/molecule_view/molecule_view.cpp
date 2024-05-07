#include "../../../test_helpers.hpp"
#include <chemist/chemical_system/molecule/molecule_view/molecule_view.hpp>

using namespace chemist;

using types2test = std::tuple<Molecule, const Molecule>;

TEMPLATE_LIST_TEST_CASE("MoleculeView", "", types2test) {
    using view_type              = MoleculeView<TestType>;
    using molecule_type          = typename view_type::molecule_type;
    using atom_type              = typename molecule_type::atom_type;
    using charge_type            = typename view_type::charge_type;
    using multiplicity_type      = typename view_type::multiplicity_type;
    using nuclei_reference       = typename view_type::nuclei_reference;
    using const_nuclei_reference = typename view_type::const_nuclei_reference;

    atom_type h("H", 1ul, 1.0, 0.0, 0.0, 0.0);
    atom_type he("He", 2ul, 4.0, 1.0, 2.0, 3.0);
    multiplicity_type m1{1}, m2{2};
    charge_type qm1{-1}, q0{0}, q1{1};
    molecule_type value_mol{h, he};

    view_type defaulted;
    view_type value(value_mol);

    SECTION("CTors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.nuclei() == nuclei_reference{});
            REQUIRE(defaulted.charge() == q0);
            REQUIRE(defaulted.multiplicity() == m1);
        }

        SECTION("From Molecule") {
            REQUIRE(value.size() == 2);
            REQUIRE(value[0] == value_mol[0]);
            REQUIRE(value[1] == value_mol[1]);
            REQUIRE(value.charge() == q0);
            REQUIRE(value.multiplicity() == m2);
        }

        SECTION("From pointers") {
            view_type value2(value_mol.nuclei(), &qm1, &m1);
            REQUIRE(value2.size() == 2);
            REQUIRE(value2[0] == value_mol[0]);
            REQUIRE(value2[1] == value_mol[1]);
            REQUIRE(value2.charge() == qm1);
            REQUIRE(value2.multiplicity() == m1);
        }

        SECTION("From mutable to read-only") {
            if constexpr(std::is_same_v<TestType, const Molecule>) {
                MoleculeView<Molecule> non_const(value_mol);
                view_type read_only(non_const);
                REQUIRE(read_only.size() == 2);
                REQUIRE(read_only[0] == value_mol[0]);
                REQUIRE(read_only[1] == value_mol[1]);
                REQUIRE(read_only.charge() == q0);
                REQUIRE(read_only.multiplicity() == m2);
            }
        }

        SECTION("Copy") {
            test_chemist::test_copy_ctor(defaulted);
            test_chemist::test_copy_ctor(value);
        }

        SECTION("Move") {}

        SECTION("Copy assignment") {}

        SECTION("Move assignment") {}
    }

    SECTION("nuclei()") {
        REQUIRE(defaulted.nuclei() == nuclei_reference{});
        REQUIRE(value.nuclei() == value_mol.nuclei());
    }

    SECTION("nuclei() const") {
        REQUIRE(std::as_const(defaulted).nuclei() == const_nuclei_reference{});
        REQUIRE(std::as_const(value).nuclei() == value_mol.nuclei());
    }

    SECTION("charge") {
        REQUIRE(defaulted.charge() == q0);
        REQUIRE(value.charge() == q0);
    }

    SECTION("charge_data() const") {
        REQUIRE(std::as_const(defaulted).charge_data() == nullptr);
        REQUIRE(std::as_const(value).charge_data() == value_mol.charge_data());
    }

    SECTION("multiplicity") {
        REQUIRE(defaulted.multiplicity() == m1);
        REQUIRE(value.multiplicity() == m2);
    }

    SECTION("multiplicity_data() const") {
        REQUIRE(std::as_const(defaulted).multiplicity_data() == nullptr);
        const auto& cvalue = value;
        REQUIRE(cvalue.multiplicity_data() == value_mol.multiplicity_data());
    }

    SECTION("swap") {
        view_type lhs_copy(defaulted);
        view_type rhs_copy(value);

        defaulted.swap(value);
        REQUIRE(defaulted == rhs_copy);
        REQUIRE(value == lhs_copy);
    }

    SECTION("operator==") {}

    SECTION("operator!=") {
        // Just negates operator== so spot check
        REQUIRE(defaulted != value);
        REQUIRE_FALSE(defaulted != view_type{});
    }
}
