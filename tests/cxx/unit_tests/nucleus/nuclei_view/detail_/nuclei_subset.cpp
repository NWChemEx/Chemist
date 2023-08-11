#include <catch2/catch.hpp>
#include <chemist/nucleus/nuclei_view/detail_/nuclei_subset.hpp>

using namespace chemist::detail_;

TEST_CASE("NucleiSubset") {
    // Create some Nucleus objects. Objects do not need to be physical, just
    // distinct.
    using nucleus_type = typename NucleiSubset::value_type;

    nucleus_type h0("H", 1ul, 0.0, 1.0, 2.0, 3.0, 4.0);
    nucleus_type h1("H", 1ul, 0.0, 5.0, 6.0, 7.0, 8.0);
    nucleus_type h2("H", 1ul, 0.0, 9.0, 8.0, 7.0, 6.0);

    // Create a Nuclei object
    using nuclei_type = typename NucleiSubset::nuclei_type;
    auto pnukes       = std::make_shared<nuclei_type>(nuclei_type{h0, h1, h2});

    // Create NucleiSubset objects
    using size_type = typename NucleiSubset::size_type;
    NucleiSubset defaulted;

    std::vector<size_type> indices{1, 2};
    NucleiSubset has_values(pnukes, indices.begin(), indices.end());

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.is_null());
        }

        SECTION("value") {
            REQUIRE_FALSE(has_values.is_null());
            REQUIRE(has_values.size() == 2);
            REQUIRE(has_values.get_nuke(0) == (*pnukes)[1]);
            REQUIRE(has_values.get_nuke(1) == (*pnukes)[2]);
        }

        SECTION("copy") {
            NucleiSubset defaulted_copy(defaulted);
            REQUIRE(defaulted_copy.size() == 0);
            REQUIRE(defaulted.is_null());

            NucleiSubset has_values_copy(has_values);
            REQUIRE_FALSE(has_values_copy.is_null());
            REQUIRE(has_values_copy.size() == 2);
            REQUIRE(has_values_copy.get_nuke(0) == (*pnukes)[1]);
            REQUIRE(has_values_copy.get_nuke(1) == (*pnukes)[2]);
        }
    }

    SECTION("clone") {
        auto pdefaulted = defaulted.clone();
        REQUIRE(pdefaulted->size() == 0);

        auto phas_value = has_values.clone();
        REQUIRE(phas_value->size() == 2);
        REQUIRE(phas_value->get_nuke(0) == (*pnukes)[1]);
        REQUIRE(phas_value->get_nuke(1) == (*pnukes)[2]);
    }

    SECTION("get_nuke") {
        REQUIRE(has_values.get_nuke(0) == (*pnukes)[1]);
        REQUIRE(has_values.get_nuke(1) == (*pnukes)[2]);
    }

    SECTION("get_nuke()const") {
        REQUIRE(std::as_const(has_values).get_nuke(0) == (*pnukes)[1]);
        REQUIRE(std::as_const(has_values).get_nuke(1) == (*pnukes)[2]);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(has_values.size() == 2);
    }

    SECTION("operator==") {
        // Defaulted == defaulted
        REQUIRE(defaulted == NucleiSubset{});

        // Defaulted != non-default
        REQUIRE_FALSE(defaulted == has_values);

        // Same everything
        NucleiSubset has_values2(pnukes, indices.begin(), indices.end());
        REQUIRE(has_values == has_values2);

        // Different instance of same supersystem
        auto pnukes1 = std::make_shared<nuclei_type>(nuclei_type{h0, h1, h2});
        NucleiSubset has_values3(pnukes1, indices.begin(), indices.end());
        REQUIRE(has_values == has_values3);

        // Different supersystem (same atoms)
        nucleus_type h3("H", 1ul, 0.0, 1.1, 2.2, 3.3, 4.4);
        auto pnukes2 = std::make_shared<nuclei_type>(nuclei_type{h3, h1, h2});
        NucleiSubset diff_ss(pnukes2, indices.begin(), indices.end());
        REQUIRE_FALSE(has_values == diff_ss);

        // Different atoms
        std::vector<size_type> indices2{0, 1};
        NucleiSubset diff_atoms(pnukes, indices2.begin(), indices2.end());
        REQUIRE_FALSE(has_values == diff_atoms);
    }
}