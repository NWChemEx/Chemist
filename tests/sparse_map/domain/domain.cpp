#include "libchemist/sparse_map/domain/domain.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::sparse_map;

TEST_CASE("Domain<ElementIndex>") {
    SECTION("Conversion from Domain<TileIndex>"){
        SECTION("Empty TileDomain") {
            Domain<TileIndex> td;
            Domain<ElementIndex> ed(td);
            REQUIRE(ed.empty());
        }

        SECTION("Rank 1 TileDomain") {
            TA::TiledRange trange{{0, 2, 4}};
            Domain<TileIndex> td(trange, {TileIndex{1}});
            Domain<ElementIndex> ed(td);
            Domain<ElementIndex> corr{ElementIndex{2}, ElementIndex{3}};
            REQUIRE(ed == corr);
        }

        SECTION("Rank 2 TileDomain") {
            TA::TiledRange trange{{0, 2, 4}, {0, 2, 4}};
            Domain<TileIndex> td(trange, {TileIndex{0, 1}, TileIndex{1, 0}});
            Domain<ElementIndex> ed(td);
            Domain<ElementIndex> corr{ElementIndex{0, 2}, ElementIndex{0, 3},
                                      ElementIndex{1, 2}, ElementIndex{1, 3},
                                      ElementIndex{2, 0}, ElementIndex{2, 1},
                                      ElementIndex{3, 0}, ElementIndex{3, 1}};
            REQUIRE(ed == corr);
        }
    }
}

TEST_CASE("Domain<TileIndex>") {
    using domain_t = TileDomain;
    TA::TiledRange empty, nonempty{{0, 1, 2}, {0, 2}};
    std::map<std::string, domain_t> ds{
      {"Default", domain_t{}},
      {"TiledRange", domain_t(empty)},
      {"Init list", domain_t(nonempty, {TileIndex{0, 0}, TileIndex{1, 0}})},
      {"Element domain", domain_t(nonempty, ElementDomain{ElementIndex{1, 1}})}
    };

    SECTION("Ctors") {

        SECTION("TA::TiledRange") {
            REQUIRE(ds.at("TiledRange").trange() == empty);
        }

        SECTION("TA::TiledRange, std::initializer_list"){
            SECTION("All good indices") {
                const auto& d = ds.at("Init list");
                REQUIRE(d.trange() == nonempty);
                REQUIRE(d.size() == 2);
                REQUIRE(d[0] == TileIndex{0, 0});
                REQUIRE(d[1] == TileIndex{1, 0});
            }
            SECTION("Bad indices") {
                REQUIRE_THROWS_AS(domain_t(nonempty, {TileIndex{2, 2}}),
                                  std::out_of_range);
            }
        }

        SECTION("TA::TiledRange, Domain<ElementIndex>") {
            SECTION("Valid indices") {
                const auto& d = ds.at("Element domain");
                REQUIRE(d.trange() == nonempty);
                REQUIRE(d.size() == 1);
                REQUIRE(d[0] == TileIndex{1, 0});
            }
            SECTION("Invalid index") {
                ElementDomain ed{ElementIndex{2, 2}};
                REQUIRE_THROWS_AS(domain_t(nonempty, ed), std::out_of_range);
            }
        }

    } // SECTION("Ctors")

    SECTION("set_trange") {
        SECTION("Good trange") {
          domain_t d{TileIndex{0,0}};
          d.set_trange(nonempty);
          REQUIRE(d.trange() == nonempty);
        }

        SECTION("Bad trange") {
            domain_t d{TileIndex{5,5}};
            REQUIRE_THROWS_AS(d.set_trange(nonempty), std::out_of_range);
        }
    }

    SECTION("trange") {
        SECTION("No trange"){
            const domain_t& d = ds.at("Default");
            REQUIRE(d.trange() == empty);
        }

        SECTION("Empty trange") {
            REQUIRE(ds.at("TiledRange").trange() == empty);
        }

        SECTION("Non-empty trange") {
            REQUIRE(ds.at("Init list").trange() == nonempty);
        }
    }

}
