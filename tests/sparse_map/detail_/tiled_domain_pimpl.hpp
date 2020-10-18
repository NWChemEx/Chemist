#include "libchemist/sparse_map/detail_/tiled_domain_pimpl.hpp"
#include "libchemist/sparse_map/index.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::sparse_map;
using namespace libchemist::sparse_map::detail_;

/* Testing strategy:
 *
 * - We assume the base class works and is tested
 *   - Hence we only need to test new functions
 * - If an instance has no trange it behaves the same as the base class
 *   - Since TA doesn't support rank 0 tensors, we test this behavior with an
 *     Domain instance holding a rank 0 index
 */
TEST_CASE("TiledDomainPIMPL"){
    using pimpl_type = TiledDomainPIMPL<DomainPIMPL<TileIndex>>;

    TileIndex e, e0{0}, e1{1};

    TA::TiledRange tr1{{0, 2, 4}};
    std::map<std::string, pimpl_type> ps;
    ps["empty"];
    ps["1 rank 0 index"].insert(e);
    ps["1 rank 1 index"] = pimpl_type(tr1);
    ps["1 rank 1 index"].insert(e1);

    SECTION("Typedefs") {
        using pimpl_t = typename pimpl_type::value_type;
        STATIC_REQUIRE(std::is_same_v<pimpl_t, TileIndex>);
    }

    SECTION("Ctors") {
        SECTION("Default") {
            auto& p = ps.at("empty");
            REQUIRE(p.size()   == 0);
            REQUIRE(p.rank()   == 0);
            REQUIRE(p.trange() == TA::TiledRange{});
        }

        SECTION("value"){
            auto& p = ps.at("1 rank 1 index");
            REQUIRE(p.size() == 1);
            REQUIRE(p.rank() == 1);
            REQUIRE(p.trange() == tr1);
        }

        SECTION("clone"){
            for(const auto& [k, v] : ps){
                SECTION(k){ REQUIRE(*v.clone() == v); }
            }
        }
    }

    SECTION("insert") {
        SECTION("empty") {
            auto& p = ps.at("empty");
            p.insert(e);
            REQUIRE(p.at(0) == e);
        }

        SECTION("good tiled index") {
            auto& p = ps.at("1 rank 1 index");
            p.insert(e0);
            REQUIRE(p.at(0) == e0);
            REQUIRE(p.at(1) == e1);
        }

        SECTION("bad tiled index") {
            auto& p = ps.at("1 rank 1 index");
            REQUIRE_THROWS_AS(p.insert(TileIndex{9}), std::out_of_range);
        }
    }

    SECTION("operator==") {
        SECTION("empty") {
            auto& p = ps.at("empty");
            SECTION("both no TiledRange") {
                REQUIRE(p == pimpl_type{});
            }

            SECTION("one has TiledRange, one doesn't") {
                pimpl_type p2(tr1);
                REQUIRE_FALSE(p == p2);
            }
        }

        SECTION("1 rank 1 index") {
            auto& p = ps.at("1 rank 1 index");
            SECTION("Same TiledRange, different indices") {
                REQUIRE(p == pimpl_type{tr1});
            }

            SECTION("Different TiledRange, same indices") {
                pimpl_type p2(TA::TiledRange{{0, 2, 4, 6}});
                p2.insert(e1);
                REQUIRE_FALSE(p == p2);
            }

            SECTION("one has TiledRange, one doesn't") {
                pimpl_type p2;
                p2.insert(e1);
                REQUIRE_FALSE(p == p2);
            }

            SECTION("Not a TiledDomainPIMPL instance") {
                DomainPIMPL<TileIndex> p2;
                p2.insert(e1);
                REQUIRE_FALSE(p == p2);
            }
        }
    }

    SECTION("set_trange") {
        SECTION("empty pimpl") {
            auto& p = ps.at("empty");
            p.set_trange(tr1);
            REQUIRE(p.trange() == tr1);
        }

        SECTION("bad trange"){
            auto& p = ps.at("1 rank 0 index");
            REQUIRE_THROWS_AS(p.set_trange(tr1), std::out_of_range);
        }

        SECTION("good trnage, already has indices") {
            TA::TiledRange temp{{0, 2, 4, 8}};
            auto& p = ps.at("1 rank 1 index");
            p.set_trange(temp);
            REQUIRE(p.trange() == temp);
        }
    }

    SECTION("trange") {
        SECTION("empty") {
            REQUIRE(ps.at("empty").trange() == TA::TiledRange{});
        }

        SECTION("1 rank 0 index") {
            REQUIRE(ps.at("1 rank 0 index").trange() == TA::TiledRange{});
        }

        SECTION("1 rank 1 index") {
            REQUIRE(ps.at("1 rank 1 index").trange() == tr1);
        }
    }
}
