#include <catch2/catch.hpp>
#include <chemist/nucleus/nuclei_view/detail_/nuclei_subset.hpp>

using namespace chemist::detail_;

TEST_CASE("NucleiSubset"){
    // Create some Nucleus objects. Objects do not need to be physical, just
    // distinct.
    using nucleus_type = typename NucleiSubset::value_type;

    nucleus_type h0("H", 1ul, 0.0, 1.0, 2.0, 3.0, 4.0);
    nucleus_type h1("H", 1ul, 0.0, 5.0, 6.0, 7.0, 8.0);
    nucleus_type h2("H", 1ul, 0.0, 9.0, 8.0, 7.0, 6.0);

    // Create a Nuclei object
    using nuclei_type = typename NucleiSubset::nuclei_type;
    auto pnukes = std::make_shared<nuclei_type>({h0, h1, h2});

    // Create NucleiSubset objects
    using size_type = typename NucleiSubset::size_type;
    NucleiSubset defaulted;

    std::vector<size_type> indices{1, 2};
    NucleiSubset has_values(pnukes, indices.begin(), indices.end());

    SECTION("Ctors"){
        SECTION("Default"){
            // N.B. PIMPL doesn't bounds check, so can only call size
            REQUIRE(defautled.size() == 0);
        }

        SECTION("value"){
            REQUIRE(has_values.size() == 2);
            REQUIRE(has_values.get_nuke(0) == *pnukes[1]);
            REQUIRE(has_values.get_nuke(1) == *pnukes[2]);
        }
    }
}