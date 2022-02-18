#include "chemist/managers/basis_set_manager.hpp"
#include <catch2/catch.hpp>
#include <string>

using namespace chemist;

using Catch::Matchers::Message;

void load_basis(BasisSetManager& bsm);

TEST_CASE("BasisSetManager Copy/Move") {
    BasisSetManager bsm;
    load_basis(bsm);

    SECTION("Copy Ctor") {
        BasisSetManager bsm2(bsm);

        REQUIRE(bsm == bsm2);
    }

    SECTION("Move Ctor") {
        BasisSetManager bsm2(std::move(bsm));
        BasisSetManager corr;
        std::string msg = "PIMPL is not set";

        load_basis(corr);

        // Compare to copy with same data values
        REQUIRE(bsm2 == corr);

        // bsm should no longer have a PIMPL, and will throw
        REQUIRE_THROWS_MATCHES(bsm == bsm2, std::runtime_error, Message(msg));
    }

    SECTION("Copy Assignment") {
        BasisSetManager bsm2;

        bsm2 = bsm;

        REQUIRE(bsm == bsm2);
    }

    SECTION("Move Assignment") {
        BasisSetManager bsm2;
        BasisSetManager corr;
        std::string msg = "PIMPL is not set";

        load_basis(corr);

        bsm2 = std::move(bsm);

        // Compare to copy with same data values
        REQUIRE(bsm2 == corr);

        // bsm should no longer have a PIMPL, and will throw
        REQUIRE_THROWS_MATCHES(bsm == bsm2, std::runtime_error, Message(msg));
    }
}

TEST_CASE("BasisSetManager Comparisons") {
    BasisSetManager blank_bsm;
    BasisSetManager blank_bsm_2;
    BasisSetManager filled_bsm;
    BasisSetManager filled_bsm_2;

    // Fill the first bsm
    load_basis(filled_bsm);

    // Fill the second bsm with only one center
    BasisSetManager::ao_basis_map basis_map;

    AtomicBasisSet<double> z1_center;
    z1_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{4.5000000000e+01});
    z1_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{7.5000000000e+00});
    z1_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{1.5000000000e+00});
    z1_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{3.0000000000e-01});

    basis_map.emplace((long)1, z1_center);

    filled_bsm_2.insert("test_basis", basis_map);

    SECTION("Equivalence") {
        std::string msg = "PIMPL is not set";

        // Empty BasisSetManagers; same object
        REQUIRE_THROWS_MATCHES(blank_bsm == blank_bsm, std::runtime_error,
                               Message(msg));
        REQUIRE_THROWS_MATCHES(blank_bsm_2 == blank_bsm_2, std::runtime_error,
                               Message(msg));

        // Empty BasisSetManagers; different objects
        REQUIRE_THROWS_MATCHES(blank_bsm == blank_bsm_2, std::runtime_error,
                               Message(msg));
        REQUIRE_THROWS_MATCHES(blank_bsm_2 == blank_bsm, std::runtime_error,
                               Message(msg));

        // Filled BasisSetManagers; same object
        REQUIRE(filled_bsm == filled_bsm);
        REQUIRE(filled_bsm_2 == filled_bsm_2);

        // Inequality with Blank
        REQUIRE_THROWS_MATCHES(filled_bsm != blank_bsm, std::runtime_error,
                               Message(msg));
        REQUIRE_THROWS_MATCHES(blank_bsm != filled_bsm, std::runtime_error,
                               Message(msg));

        // Inequality with Filled
        REQUIRE(filled_bsm != filled_bsm_2);
        REQUIRE(filled_bsm_2 != filled_bsm);

        // Fetching bases
        REQUIRE(filled_bsm.get_basis("test_basis", 1) ==
                filled_bsm_2.get_basis("test_basis", 1));
        REQUIRE(filled_bsm.get_basis("test_basis", 4) !=
                filled_bsm_2.get_basis("test_basis", 1));
    }
}

TEST_CASE("BasisSetManager::insert") {
    BasisSetManager bsm;

    SECTION("Repeat basis set") {
        load_basis(bsm);
        std::string msg = "Basis set already exists named test_basis";

        REQUIRE_THROWS_MATCHES(load_basis(bsm), std::runtime_error,
                               Message(msg));
    }
}

TEST_CASE("BasisSetManager::get_basis") {
    BasisSetManager bsm;

    SECTION("Empty BasisSetManager") {
        std::string msg = "PIMPL is not set";

        REQUIRE_THROWS_MATCHES(bsm.get_basis("nonexistant_basis", 1),
                               std::runtime_error, Message(msg));
    }

    SECTION("Retrieve nonexistant basis set") {
        std::string msg = "No basis set named nonexistant_basis";

        load_basis(bsm);

        REQUIRE_THROWS_MATCHES(bsm.get_basis("nonexistant_basis", 1),
                               std::out_of_range, Message(msg));
    }

    SECTION("Retrieve nonexistant AO basis") {
        std::string msg =
          "Basis set test_basis does not contain AOs for atomic number 3";

        load_basis(bsm);

        REQUIRE_THROWS_MATCHES(bsm.get_basis("test_basis", 3),
                               std::out_of_range, Message(msg));
    }

    SECTION("Retrieve existing basis") {
        // Correct basis
        AtomicBasisSet<double> corr;
        corr.add_shell(ShellType::pure, 0,
                       std::vector<double>{1.0000000000e+00},
                       std::vector<double>{5.4000000000e+01});
        corr.add_shell(ShellType::pure, 0,
                       std::vector<double>{1.0000000000e+00},
                       std::vector<double>{9.0000000000e+00});
        corr.add_shell(ShellType::pure, 0,
                       std::vector<double>{1.0000000000e+00},
                       std::vector<double>{1.8000000000e+00});
        corr.add_shell(ShellType::pure, 0,
                       std::vector<double>{1.0000000000e+00},
                       std::vector<double>{3.6000000000e-01});

        // Load dummy basis set
        load_basis(bsm);

        REQUIRE(bsm.get_basis("test_basis", 2) == corr);
    }
}

void load_basis(BasisSetManager& bsm) {
    BasisSetManager::ao_basis_map basis_map;

    AtomicBasisSet<double> z1_center;
    z1_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{4.5000000000e+01});
    z1_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{7.5000000000e+00});
    z1_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{1.5000000000e+00});
    z1_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{3.0000000000e-01});

    basis_map.emplace(1, z1_center);

    AtomicBasisSet<double> z2_center;
    z2_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{5.4000000000e+01});
    z2_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{9.0000000000e+00});
    z2_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{1.8000000000e+00});
    z2_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{3.6000000000e-01});

    basis_map.emplace(2, z2_center);

    AtomicBasisSet<double> z4_center;
    z4_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{5.1200000000e+02});
    z4_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{1.0300000000e+02});
    z4_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{2.5600000000e+01});
    z4_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{6.4000000000e+00});
    z4_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{1.6000000000e+00});
    z4_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{4.0000000000e-01});
    z4_center.add_shell(ShellType::pure, 1,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{4.0000000000e-01});
    z4_center.add_shell(ShellType::pure, 0,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{1.1000000000e-01});
    z4_center.add_shell(ShellType::pure, 1,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{1.1000000000e-01});
    z4_center.add_shell(ShellType::pure, 2,
                        std::vector<double>{1.0000000000e+00},
                        std::vector<double>{4.0000000000e-01});

    basis_map.emplace(4, z4_center);

    bsm.insert("test_basis", basis_map);
}