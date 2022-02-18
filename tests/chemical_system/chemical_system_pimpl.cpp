#include "chemist/chemical_system/chemical_system_pimpl.hpp"
#include <catch2/catch.hpp>

TEST_CASE("ChemicalSystemPIMPL") {
    using chem_sys_t       = chemist::ChemicalSystem;
    using chem_sys_pimpl_t = chemist::detail_::ChemicalSystemPIMPL;

    chemist::Molecule default_mol, h{chemist::Atom(1ul)};

    chemist::potentials::Electrostatic default_v, v;
    v.add_charge(chemist::PointCharge<double>());

    // For all of these typedefs we are just ensuring that the ChemicalSystem
    // and the ChemicalSystemPIMPL class are synched
    SECTION("Typedefs") {
        SECTION("chem_sys_t") {
            using t = typename chem_sys_pimpl_t::chem_sys_t;
            STATIC_REQUIRE(std::is_same_v<t, chem_sys_t>);
        }

        SECTION("pimpl_ptr_t") {
            using t    = typename chem_sys_pimpl_t::pimpl_ptr_t;
            using corr = typename chem_sys_t::pimpl_ptr_t;
        }

        SECTION("molecule_t") {
            using t    = typename chem_sys_pimpl_t::molecule_t;
            using corr = typename chem_sys_t::molecule_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("mol_ref_t") {
            using t    = typename chem_sys_pimpl_t::mol_ref_t;
            using corr = typename chem_sys_t::mol_ref_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("const_mol_ref_t") {
            using t    = typename chem_sys_pimpl_t::const_mol_ref_t;
            using corr = typename chem_sys_t::const_mol_ref_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("size_type") {
            using t    = typename chem_sys_pimpl_t::size_type;
            using corr = typename chem_sys_t::size_type;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("epot_t") {
            using t    = typename chem_sys_pimpl_t::epot_t;
            using corr = typename chem_sys_t::epot_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("epot_ref_t") {
            using t    = typename chem_sys_pimpl_t::epot_ref_t;
            using corr = typename chem_sys_t::epot_ref_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("const_epot_ref_t") {
            using t    = typename chem_sys_pimpl_t::const_epot_ref_t;
            using corr = typename chem_sys_t::const_epot_ref_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }
    }

    SECTION("Default ctor") {
        chem_sys_pimpl_t pimpl;
        REQUIRE(pimpl.molecule() == default_mol);
        REQUIRE(pimpl.nelectrons() == 0);
        REQUIRE(pimpl.external_electrostatic_potential() == default_v);
    }

    SECTION("Value ctor") {
        chem_sys_pimpl_t pimpl(h, 2, v);
        REQUIRE(pimpl.molecule() == h);
        REQUIRE(pimpl.nelectrons() == 2);
        REQUIRE(pimpl.external_electrostatic_potential() == v);
    }

    SECTION("clone") {
        SECTION("Default") {
            chem_sys_pimpl_t pimpl;
            auto copy = pimpl.clone();
            REQUIRE(*copy == pimpl);
        }

        SECTION("Non-default") {
            chem_sys_pimpl_t pimpl(h, 2, v);
            auto copy = pimpl.clone();
            REQUIRE(*copy == pimpl);
        }
    }

    SECTION("molecule()") {
        SECTION("Default") {
            chem_sys_pimpl_t pimpl;
            REQUIRE(pimpl.molecule() == default_mol);
        }

        SECTION("Has value") {
            chem_sys_pimpl_t pimpl(h, 2, v);
            REQUIRE(pimpl.molecule() == h);
        }

        SECTION("Is writeable") {
            chem_sys_pimpl_t pimpl;
            pimpl.molecule() = h;
            REQUIRE(pimpl.molecule() == h);
        }
    }

    SECTION("molecule() const") {
        SECTION("Default") {
            const chem_sys_pimpl_t pimpl;
            REQUIRE(pimpl.molecule() == default_mol);
        }

        SECTION("Has value") {
            const chem_sys_pimpl_t pimpl(h, 2, v);
            REQUIRE(pimpl.molecule() == h);
        }
    }

    SECTION("nelectrons") {
        chem_sys_pimpl_t pimpl(h, 2, v);
        REQUIRE(pimpl.nelectrons() == 2);
    }

    SECTION("nelectrons() const") {
        const chem_sys_pimpl_t pimpl(h, 2, v);
        REQUIRE(pimpl.nelectrons() == 2);
    }

    SECTION("external_electrostatic_potential()") {
        SECTION("default") {
            chem_sys_pimpl_t pimpl;
            chemist::potentials::Electrostatic corr;
            REQUIRE(pimpl.external_electrostatic_potential() == corr);
        }

        SECTION("Has value") {
            chem_sys_pimpl_t pimpl(h, 2, v);
            REQUIRE(pimpl.external_electrostatic_potential() == v);
        }

        SECTION("Is writeable") {
            chem_sys_pimpl_t pimpl;
            pimpl.external_electrostatic_potential() = v;
            REQUIRE(pimpl.external_electrostatic_potential() == v);
        }
    }

    SECTION("external_electrostatic_potential() const") {
        SECTION("default") {
            const chem_sys_pimpl_t pimpl;
            chemist::potentials::Electrostatic corr;
            REQUIRE(pimpl.external_electrostatic_potential() == corr);
        }

        SECTION("Has value") {
            const chem_sys_pimpl_t pimpl(h, 2, v);
            REQUIRE(pimpl.external_electrostatic_potential() == v);
        }
    }

    SECTION("hash") {
        SECTION("LHS is default") {
            using chemist::detail_::hash_objects;
            chem_sys_pimpl_t lhs;
            auto lhs_hash = hash_objects(lhs);

            SECTION("RHS is default") {
                chem_sys_pimpl_t rhs;
                REQUIRE(lhs_hash == hash_objects(rhs));
            }

            SECTION("RHS has a different molecule") {
                chem_sys_pimpl_t rhs(h, 0, default_v);
                REQUIRE_FALSE(lhs_hash == hash_objects(rhs));
            }

            SECTION("RHS has a different number of electrons") {
                chem_sys_pimpl_t rhs(default_mol, 2, default_v);
                REQUIRE_FALSE(lhs_hash == hash_objects(rhs));
            }

            SECTION("RHS has a different potential") {
                chem_sys_pimpl_t rhs(default_mol, 0, v);
                REQUIRE_FALSE(lhs_hash == hash_objects(rhs));
            }
        }
    }

    SECTION("operator==") {
        SECTION("LHS is default") {
            chem_sys_pimpl_t lhs;

            SECTION("RHS is default") {
                chem_sys_pimpl_t rhs;
                REQUIRE(lhs == rhs);
            }

            SECTION("RHS has a different molecule") {
                chem_sys_pimpl_t rhs(h, 0, default_v);
                REQUIRE_FALSE(lhs == rhs);
            }

            SECTION("RHS has a different number of electrons") {
                chem_sys_pimpl_t rhs(default_mol, 2, default_v);
                REQUIRE_FALSE(lhs == rhs);
            }

            SECTION("RHS has a different potential") {
                chem_sys_pimpl_t rhs(default_mol, 1, v);
                REQUIRE_FALSE(lhs == rhs);
            }
        }
    }
}
