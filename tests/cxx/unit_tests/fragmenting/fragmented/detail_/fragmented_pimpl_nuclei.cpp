#include <catch2/catch.hpp>
#include <chemist/fragmenting/fragmented/detail_/fragmented_pimpl_nuclei.hpp>

using namespace chemist;
using namespace fragmenting::detail_;

TEST_CASE("FragmentedPIMPL<Nuclei>"){
    using pimpl_type   = FragmentedPIMPL<Nuclei>;
    using nuclei_type  = typename pimpl_type::value_type;
    using nucleus_type = typename nuclei_type::value_type;
    
    // Make some Nucleus objects
    nucleus_type h0("H", 1, 1.00, 0.0, 0.0, 0.0);
    nucleus_type h1("H", 1, 1.00, 0.0, 0.0, 0.98);
    nucleus_type d("D", 1, 2.00, 0.0, 0.0, 0.98);

    // Make some Nuclei objects
    Nuclei empty;
    Nuclei h2{h0, h1};


    // Make some PIMPLs. N.B. they have no fragments yet
    pimpl_type empty_pimpl(std::make_shared<nuclei_type>(empty));
    pimpl_type h2_pimpl(std::make_shared<nuclei_type>(h2));

    SECTION("Constructors"){
        SECTION("No caps"){
            REQUIRE(empty_pimpl.size() == 0);
            REQUIRE(h2_pimpl.size() == 0);
        }
    }
}