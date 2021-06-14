#include "libchemist/chemical_system/chemical_system_pimpl.hpp"
#include <catch2/catch.hpp>

TEST_CASE("ChemicalSystemPIMPL") {
    using chem_sys_t       = libchemist::ChemicalSystem;
    using chem_sys_pimpl_t = libchemist::detail_::ChemicalSystemPIMPL;

    libchemist::Molecule default_mol, h(libchemist::Atom(1ul));

    libchemist::AOBasisSet<double> default_aos, aos;
    aos.add_center(libchemist::Center<double>(1.0, 2.0, 3.0));

    libchemist::potentials::Electrostatic default_v, v;
    v.add_charge(libchemist::PointCharge<double>());

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

        SECTION("ao_basis_t") {
            using t    = typename chem_sys_pimpl_t::ao_basis_t;
            using corr = typename chem_sys_t::ao_basis_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("ao_basis_ref_t") {
            using t    = typename chem_sys_pimpl_t::ao_basis_ref_t;
            using corr = typename chem_sys_t::ao_basis_ref_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("const_ao_basis_ref_t") {
            using t    = typename chem_sys_pimpl_t::const_ao_basis_t;
            using corr = typename chem_sys_t::const_ao_basis_t;
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
        REQUIRE(pimpl.basis_set() == default_aos);
        REQUIRE(pimpl.external_electrostatic_potential() ==
                default_v);
    }

    SECTION("Value ctor") {
        SECTION("Defaulted basis and potential") {
            chem_sys_pimpl_t pimpl(h);
            REQUIRE(pimpl.molecule() == h);
            REQUIRE(pimpl.basis_set() == default_aos);
            REQUIRE(pimpl.external_electrostatic_potential() ==
                    default_v);
        }

        SECTION("Defaulted potential") {
            chem_sys_pimpl_t pimpl(h, aos);
            REQUIRE(pimpl.molecule() == h);
            REQUIRE(pimpl.basis_set() == aos);
            REQUIRE(pimpl.external_electrostatic_potential() ==
                    default_v);
        }

        SECTION("All values set") {
            chem_sys_pimpl_t pimpl(h, aos, v);
            REQUIRE(pimpl.molecule() == h);
            REQUIRE(pimpl.basis_set() == aos);
            REQUIRE(pimpl.external_electrostatic_potential() == v);
        }
    }

    SECTION("clone") {
        SECTION("Default") {
            chem_sys_pimpl_t pimpl;
            auto copy = pimpl.clone();
            REQUIRE(*copy == pimpl);
        }

        SECTION("Non-default") {
            chem_sys_pimpl_t pimpl(h, bs, v);
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
            chem_sys_pimpl_t pimpl(h);
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
            const chem_sys_pimpl_t pimpl(h);
            REQUIRE(pimpl.molecule() == h);
        }
    }

    SECTION("basis_set()") {
        SECTION("Default") {
            chem_sys_pimpl_t pimpl;
            REQUIRE(pimpl.basis_set() == default_aos);
        }

        SECTION("Has value") {
            chem_sys_pimpl_t pimpl(h, aos);
            REQUIRE(pimpl.basis_set() == aos);
        }

        SECTION("Is writeable") {
            chem_sys_pimpl_t pimpl;
            pimpl.basis_set() = aos;
            REQUIRE(pimpl.basis_set() == aos);
        }
    }

    SECTION("basis_set() const") {
        SECTION("Default") {
            const chem_sys_pimpl_t pimpl;
            REQUIRE(pimpl.basis_set() == default_aos);
        }

        SECTION("Has value") {
            const chem_sys_pimpl_t pimpl(h, aos);
            REQUIRE(pimpl.basis_set() == aos);
        }
    }


    SECTION("external_electrostatic_potential()") {
        SECTION("default") {
            chem_sys_pimpl_t pimpl;
            libchemist::potentials::Electrostatic corr;
            REQUIRE(pimpl.external_electrostatic_potential() == corr);
        }

        SECTION("Has value") {
            chem_sys_pimpl_t pimpl(h, v);
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
            libchemist::potentials::Electrostatic corr;
            REQUIRE(pimpl.external_electrostatic_potential() == corr);
        }

        SECTION("Has value") {
            const chem_sys_pimpl_t pimpl(h, v);
            REQUIRE(pimpl.external_electrostatic_potential() == v);
        }
    }

    SECTION("hash") {
        SECTION("LHS is default") {
            chem_sys_pimpl_t lhs;
            auto lhs_hash = sde::hash_objects(lhs);

            SECTION("RHS is default") {
                chem_sys_pimpl_t rhs;
                REQUIRE(lhs_hash == sde::hash_objects(rhs));
            }

            SECTION("RHS has a different molecule") {
                chem_sys_pimpl_t rhs(h);
                REQUIRE_FALSE(lhs_hash == sde::hash_objects(rhs));
            }

            SECTION("RHS has a different basis set") {
                chem_sys_pimpl_t rhs(default_mol, aos);
                REQUIRE_FALSE(lhs_hash == sde::hash_objects(rhs));
            }

            SECTION("RHS has a different potential") {
                chem_sys_pimpl_t rhs(default_mol, default_aos, v);
                REQUIRE_FALSE(lhs_hash == sde::hash_objects(rhs));
            }
        }

        SECTION("LHS has values") {
            chem_sys_pimpl_t lhs(h, aos, v);
            auto lhs_hash = sde::hash_objects(lhs);

            SECTION("RHS is default") {
                chem_sys_pimpl_t rhs;
                REQUIRE_FALSE(lhs_hash == sde::hash_objects(rhs));
            }

            SECTION("RHS has a different molecule") {
                chem_sys_pimpl_t rhs(default_mol, aos, v);
                REQUIRE_FALSE(lhs_hash == sde::hash_objects(rhs));
            }

            SECTION("RHS has a different basis") {
                chem_sys_pimpl_t rhs(h, default_aos, v);
                REQUIRE_FALSE(lhs_hash == sde::hash_objects(rhs));
            }

            SECTION("RHS has a different potential") {
                chem_sys_pimpl_t rhs(h, aos, default_v);
                REQUIRE_FALSE(lhs_hash == sde::hash_objects(rhs));
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
                chem_sys_pimpl_t rhs(h);
                REQUIRE_FALSE(lhs == rhs);
            }

            SECTION("RHS has a different basis"){
                chem_sys_pimpl_t rhs(default_mol, aos);
                REQUIRE_FALSE(lhs == rhs);
            }

            SECTION("RHS has a different potential") {
                chem_sys_pimpl_t rhs(default_mol, v);
                REQUIRE_FALSE(lhs == rhs);
            }

        }

        SECTION("LHS has values") {
            chem_sys_pimpl_t lhs(h, aos, v);

            SECTION("RHS is default") {
                chem_sys_pimpl_t rhs;
                REQUIRE_FALSE(lhs == rhs);
            }

            SECTION("RHS has a different molecule") {
                chem_sys_pimpl_t rhs(default_mol, aos, v);
                REQUIRE_FALSE(lhs == rhs);
            }

            SECTION("RHS has different aos"){
                chem_sys_pimpl_t rhs(h, default_aos, v);
                REQUIRE_FALSE(lhs == rhs);
            }

            SECTION("RHS has a different potential") {
                chem_sys_pimpl_t rhs(h,
                                     default_v);
                REQUIRE_FALSE(lhs == rhs);
            }
        }
    }
}
