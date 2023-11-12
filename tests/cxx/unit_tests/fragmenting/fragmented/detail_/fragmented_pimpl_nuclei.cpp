#include <catch2/catch.hpp>
#include <chemist/fragmenting/fragmented/detail_/fragmented_pimpl_nuclei.hpp>

using namespace chemist;
using namespace fragmenting::detail_;

TEST_CASE("FragmentedPIMPL<Nuclei>"){
    using pimpl_type   = FragmentedPIMPL<Nuclei>;
    using nuclei_type  = typename pimpl_type::value_type;
    using size_type    = typename pimpl_type::size_type;
    using cap_set_type = typename pimpl_type::cap_set_type;
    using cap_type     = typename cap_set_type::value_type;
    using nucleus_type = typename nuclei_type::value_type;

    // The PIMPL shouldn't care what container is used to input the indices, so
    // we just use std::vector<size_type>
    using index_container = std::vector<size_type>;

    // Make some Nucleus objects
    nucleus_type h0("H", 1, 1.00, 0.0, 0.0, 0.0);
    nucleus_type h1("H", 1, 1.00, 0.0, 0.0, 0.98);

    // Make some Nuclei objects
    Nuclei empty;
    Nuclei h2{h0, h1};
    
    // And some caps
    cap_set_type caps{cap_type(0, 1, h1), cap_type(1, 0, h0)};

    std::vector frags{index_container{0}, index_container{1}};

    // Make some PIMPLs.
    pimpl_type null_pimpl(nullptr);
    pimpl_type empty_pimpl(std::make_shared<nuclei_type>(empty));
    
    pimpl_type h2_pimpl(std::make_shared<nuclei_type>(h2));
    
    pimpl_type h2_w_caps(std::make_shared<nuclei_type>(h2), caps);

    pimpl_type h2_w_frags(
        std::make_shared<nuclei_type>(h2), frags.begin(), frags.end()
    );

    pimpl_type h2_w_frags_and_caps(
        std::make_shared<nuclei_type>(h2), frags.begin(), frags.end(), caps
    );

    SECTION("Constructors"){
        SECTION("No fragments, no caps"){
            REQUIRE(empty_pimpl.size() == 0);
            REQUIRE(h2_pimpl.size() == 0);
        }
        SECTION("No fragments"){
            REQUIRE(h2_w_caps.size() == 0);
        }
        SECTION("Fragments no caps"){
            REQUIRE(h2_w_frags.size() == 2);
        }
        SECTION("Fragments with caps"){
            REQUIRE(h2_w_frags_and_caps.size() == 2);
        }
    }

    SECTION("operator=="){
        SECTION("*this == null"){
            // Compare null to null
            REQUIRE(pimpl_type{nullptr} == null_pimpl);

            // Null to empty
            REQUIRE_FALSE(null_pimpl == empty_pimpl);

            // Null to no fragments and no caps
            REQUIRE_FALSE(null_pimpl == h2_pimpl);

            // Null to with caps
            REQUIRE_FALSE(null_pimpl == h2_w_caps);

            // Null to with fragments
            REQUIRE_FALSE(null_pimpl == h2_w_frags);

            // Null to with frags and caps
            REQUIRE_FALSE(null_pimpl == h2_w_frags_and_caps);
        }

    }
}