#include "libchemist/managers/basis_set_manager.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;

void load_basis(BasisSetManager& bsm) {
    BasisSetManager::ao_basis_map basis_map;

    Center<double> z1_center(0.0, 0.0, 0.0);
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

    Center<double> z2_center(0.0, 0.0, 0.0);
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

    Center<double> z4_center(0.0, 0.0, 0.0);
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

TEST_CASE("Get/Set Bases")
{
    BasisSetManager bsm;

    SECTION("Retrieve nonexistant basis set") {
        REQUIRE_THROWS(bsm.get_basis("nonexistant_basis", 1));
    }

    SECTION("Retrieve nonexistant AO basis") {
        load_basis(bsm);

        REQUIRE_THROWS(bsm.get_basis("test_basis", 3));
    }

    SECTION("Retrieve existing basis")
    {
        // Correct basis
        Center<double> corr(0.0, 0.0, 0.0);
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

TEST_CASE("Comparisons") {
    BasisSetManager blank_bsm;
    BasisSetManager blank_bsm_2;
    BasisSetManager filled_bsm;
    BasisSetManager filled_bsm_2;

    // Fill the first bsm
    load_basis(filled_bsm);

    // Fill the second bsm with only one center
    BasisSetManager::ao_basis_map basis_map;

    Center<double> z1_center(0.0, 0.0, 0.0);
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
        // Empty BasisSetManagers; same object
        REQUIRE_THROWS(blank_bsm == blank_bsm);
        REQUIRE_THROWS(blank_bsm_2 == blank_bsm_2);

        // Empty BasisSetManagers; different objects
        REQUIRE_THROWS(blank_bsm == blank_bsm_2);
        REQUIRE_THROWS(blank_bsm_2 == blank_bsm);

        // Filled BasisSetManagers; same object
        REQUIRE(filled_bsm == filled_bsm);
        REQUIRE(filled_bsm_2 == filled_bsm_2);

        // Inequality with Blank
        REQUIRE_THROWS(filled_bsm != blank_bsm);
        REQUIRE_THROWS(blank_bsm != filled_bsm);

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

TEST_CASE("Copy/Move")
{
    BasisSetManager bsm;
    load_basis(bsm);

    SECTION("Copy Ctor")
    {
        BasisSetManager bsm2(bsm);

        REQUIRE(bsm == bsm2);
    }

    SECTION("Move Ctor")
    {
        BasisSetManager bsm2(std::move(bsm));
        BasisSetManager corr;

        load_basis(corr);

        // Compare to copy with same data values
        REQUIRE(bsm2 == corr);

        // bsm should no longer have a PIMPL, and will throw
        REQUIRE_THROWS(bsm == bsm2);
    }

    SECTION("Copy Assignment")
    {
        BasisSetManager bsm2;

        bsm2 = bsm;

        REQUIRE(bsm == bsm2);
    }

    SECTION("Move Assignment")
    {
        BasisSetManager bsm2;
        BasisSetManager corr;

        load_basis(corr);

        bsm2 = std::move(bsm);

        // Compare to copy with same data values
        REQUIRE(bsm2 == corr);

        // bsm should no longer have a PIMPL, and will throw
        REQUIRE_THROWS(bsm == bsm2);
    }
}
