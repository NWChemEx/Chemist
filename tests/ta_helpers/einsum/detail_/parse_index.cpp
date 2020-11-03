#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/einsum/detail_/detail_.hpp>

using libchemist::ta_helpers::einsum::detail_::parse_index;
using idx_vector = libchemist::ta_helpers::einsum::types::index_set;

TEST_CASE("parse_index") {
    SECTION("No indices") { REQUIRE(parse_index("") == idx_vector{}); }

    SECTION("One index") {
        SECTION("Single letter") {
            REQUIRE(parse_index("i") == idx_vector{"i"});
        }

        SECTION("Multiple letters") {
            REQUIRE(parse_index("ijk") == idx_vector{"ijk"});
        }

        SECTION("Contains a space (probably shouldn't do this...)") {
            REQUIRE(parse_index("i j") == idx_vector{"ij"});
        }
    } // one index

    SECTION("Two indices") {
        SECTION("Single letter per index") {
            idx_vector corr{"i", "j"};
            REQUIRE(parse_index("i,j") == corr);
        }

        SECTION("Multiple letters per index") {
            idx_vector corr{"ij", "jk"};
            REQUIRE(parse_index("ij,jk") == corr);
        }

        SECTION("Mixed size indices") {
            idx_vector corr{"ij", "j"};
            REQUIRE(parse_index("ij,j") == corr);
        }

        SECTION("Space after comma") {
            idx_vector corr{"ij", "j"};
            REQUIRE(parse_index("ij, j") == corr);
        }

        SECTION("Space before comma") {
            idx_vector corr{"ij", "j"};
            REQUIRE(parse_index("ij ,j") == corr);
        }

        SECTION("Space on both sides of the comma") {
            idx_vector corr{"ij", "j"};
            REQUIRE(parse_index("ij , j") == corr);
        }

        SECTION("Repeated index") {
            idx_vector corr{"j", "j"};
            REQUIRE(parse_index("j ,j") == corr);
        }
    } // two indices

    SECTION("Three indices") {
        SECTION("Single letter per index") {
            idx_vector corr{"i", "j", "k"};
            REQUIRE(parse_index("i,j,k") == corr);
        }

        SECTION("Multiple letters per index") {
            idx_vector corr{"ij", "jk", "ik"};
            REQUIRE(parse_index("ij,jk,ik") == corr);
        }

        SECTION("Mixed size indices") {
            idx_vector corr{"ijk", "ij", "i"};
            REQUIRE(parse_index("ijk,ij,i") == corr);
        }

        SECTION("Spaces after commas") {
            idx_vector corr{"ijk", "ij", "i"};
            REQUIRE(parse_index("ijk, ij, i") == corr);
        }
    } // Three indices
} // parse_index