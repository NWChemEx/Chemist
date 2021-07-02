#include "libchemist/chemical_system/chemical_system.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;

TEST_CASE("ChemicalSystem") {
    SECTION("Typedefs") {
        SECTION("pimpl_t") {
            using pimpl_t = typename ChemicalSystem::pimpl_t;
            using corr_t  = detail_::ChemicalSystemPIMPL;
            STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
        }

        SECTION("pimpl_ptr_t") {
            using pimpl_ptr_t = typename ChemicalSystem::pimpl_ptr_t;
            using corr_t      = std::unique_ptr<detail_::ChemicalSystemPIMPL>;
            STATIC_REQUIRE(std::is_same_v<pimpl_ptr_t, corr_t>);
        }

        SECTION("molecule_t") {
            using molecule_t = typename ChemicalSystem::molecule_t;
            using corr_t     = Molecule;
            STATIC_REQUIRE(std::is_same_v<molecule_t, corr_t>);
        }

        SECTION("mol_ref_t") {
            using mol_ref_t = typename ChemicalSystem::mol_ref_t;
            using corr_t    = Molecule&;
            STATIC_REQUIRE(std::is_same_v<mol_ref_t, corr_t>);
        }

        SECTION("const_mol_ref_t") {
            using const_mol_ref_t = typename ChemicalSystem::const_mol_ref_t;
            using corr_t          = const Molecule&;
            STATIC_REQUIRE(std::is_same_v<const_mol_ref_t, corr_t>);
        }

        SECTION("epot_t") {
            using epot_t = typename ChemicalSystem::epot_t;
            using corr_t = potentials::Electrostatic;
            STATIC_REQUIRE(std::is_same_v<epot_t, corr_t>);
        }

        SECTION("epot_ref_t") {
            using epot_ref_t = typename ChemicalSystem::epot_ref_t;
            using corr_t     = potentials::Electrostatic&;
            STATIC_REQUIRE(std::is_same_v<epot_ref_t, corr_t>);
        }

        SECTION("const_epot_ref_t") {
            using const_epot_ref_t = typename ChemicalSystem::const_epot_ref_t;
            using corr_t           = const potentials::Electrostatic&;
            STATIC_REQUIRE(std::is_same_v<const_epot_ref_t, corr_t>);
        }
    }

    SECTION("Default ctor") {
        ChemicalSystem sys;
        REQUIRE(sys.molecule() == Molecule{});
        potentials::Electrostatic corr_v;
        REQUIRE(sys.external_electrostatic_potential() == corr_v);
    }

    libchemist::Molecule h(libchemist::Atom(1ul));

    libchemist::potentials::Electrostatic v;
    v.add_charge(libchemist::PointCharge<double>());

    SECTION("Copy ctor") {
        ChemicalSystem sys(h, v);
        ChemicalSystem copy(sys);
        REQUIRE(sys == copy);
        REQUIRE(copy.molecule() == h);
        REQUIRE(copy.external_electrostatic_potential() == v);
    }

    SECTION("Move ctor") {
        ChemicalSystem sys(h, v);
        ChemicalSystem moved(std::move(sys));
        REQUIRE(moved.molecule() == h);
        REQUIRE(moved.external_electrostatic_potential() == v);
    }

    SECTION("value ctor") {
        SECTION("Default potential") {
            ChemicalSystem sys(h);
            REQUIRE(sys.molecule() == h);
            potentials::Electrostatic corr_v;
            REQUIRE(sys.external_electrostatic_potential() == corr_v);
        }

        SECTION("Set both") {
            ChemicalSystem sys(h, v);
            REQUIRE(sys.molecule() == h);
            REQUIRE(sys.external_electrostatic_potential() == v);
        }
    }

    SECTION("copy assignment") {
        ChemicalSystem sys(h, v);
        ChemicalSystem copy;
        auto pcopy = &(copy = sys);
        REQUIRE(sys == copy);
        REQUIRE(pcopy == &copy);
        REQUIRE(copy.molecule() == h);
        REQUIRE(copy.external_electrostatic_potential() == v);
    }

    SECTION("move assignment") {
        ChemicalSystem sys(h, v);
        ChemicalSystem moved;
        auto pmoved = &(moved = std::move(sys));
        REQUIRE(pmoved == &moved);
        REQUIRE(moved.molecule() == h);
        REQUIRE(moved.external_electrostatic_potential() == v);
    }

    SECTION("molecule()") {
        ChemicalSystem sys(h);

        SECTION("value") { REQUIRE(sys.molecule() == h); }

        SECTION("is read/write") {
            sys.molecule() = Molecule{};
            REQUIRE(sys.molecule() == Molecule{});
        }

        SECTION("Allocates new PIMPL if no PIMPL") {
            ChemicalSystem buffer(std::move(sys));
            REQUIRE(sys.molecule() == Molecule{});
        }
    }

    SECTION("molecule() const") {
        ChemicalSystem sys(h);

        SECTION("value") { REQUIRE(std::as_const(sys).molecule() == h); }

        SECTION("Throws if no PIMPL") {
            ChemicalSystem buffer(std::move(sys));
            const auto& csys = sys;
            REQUIRE_THROWS_AS(csys.molecule(), std::runtime_error);
        }
    }

    SECTION("external_electrostatic_potential()") {
        ChemicalSystem sys(h, v);

        SECTION("value") {
            REQUIRE(sys.external_electrostatic_potential() == v);
        }

        SECTION("is read/write") {
            potentials::Electrostatic new_v;
            sys.external_electrostatic_potential() = new_v;
            REQUIRE(sys.external_electrostatic_potential() == new_v);
        }

        SECTION("Allocates new PIPML if no PIMPL") {
            ChemicalSystem buffer(std::move(sys));
            potentials::Electrostatic corr;
            REQUIRE(sys.external_electrostatic_potential() == corr);
        }
    }

    SECTION("external_electrostatic_potential() const") {
        ChemicalSystem sys(h, v);

        SECTION("value") {
            REQUIRE(std::as_const(sys).external_electrostatic_potential() == v);
        }

        SECTION("Throws if no PIMPL") {
            ChemicalSystem buffer(std::move(sys));
            const ChemicalSystem& csys = sys;
            REQUIRE_THROWS_AS(csys.external_electrostatic_potential(),
                              std::runtime_error);
        }
    }

    SECTION("hash") {
        SECTION("LHS is default") {
            ChemicalSystem lhs;
            auto lhs_hash = runtime::hash_objects(lhs);

            SECTION("RHS is default") {
                ChemicalSystem rhs;
                REQUIRE(lhs_hash == runtime::hash_objects(rhs));
            }

            SECTION("RHS has different molecule") {
                ChemicalSystem rhs(h);
                REQUIRE(lhs_hash != runtime::hash_objects(rhs));
            }

            SECTION("RHS has different potential") {
                ChemicalSystem rhs(Molecule(), v);
                REQUIRE(lhs_hash != runtime::hash_objects(rhs));
            }

            SECTION("RHS is completely different") {
                ChemicalSystem rhs(h, v);
                REQUIRE(lhs_hash != runtime::hash_objects(rhs));
            }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS is default") {
            ChemicalSystem lhs;

            SECTION("RHS is default") {
                ChemicalSystem rhs;
                REQUIRE(lhs == rhs);
                REQUIRE_FALSE(lhs != rhs);
            }

            SECTION("RHS has different molecule") {
                ChemicalSystem rhs(h);
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }

            SECTION("RHS has different potential") {
                ChemicalSystem rhs(Molecule(), v);
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }

            SECTION("RHS is completely different") {
                ChemicalSystem rhs(h, v);
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }
        }
    }
}