#include "libchemist/sparse_map/sparse_map/sparse_map.hpp"
#include "libchemist/sparse_map/index.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::sparse_map;

TEST_CASE("SparseMap<ElementIndex, ElementIndex>") {
    using sm_t = SparseMap<ElementIndex, ElementIndex>;

    ElementIndex e0{0}, e1{1}, e2{2}, e3{3};
    TileIndex t0{0}, t1{1}, t2{2};

    SECTION("CTors") {
        SECTION("From SparseMap<ElementIndex, TileIndex>") {
            SparseMap<ElementIndex, TileIndex> sm;
            TA::TiledRange tr{{0, 2, 4}};
            sm.add_to_domain(e0, t1);

            SECTION("TiledRange is set") {
                sm.set_domain_trange(tr);

                sm_t sm2(sm);
                sm_t corr;
                corr.add_to_domain(e0, e2);
                corr.add_to_domain(e0, e3);
                REQUIRE(sm2 == corr);
            }

            SECTION("TiledRange is not set") {
                REQUIRE_THROWS_AS(sm_t(sm), std::runtime_error);
            }
        }

        SECTION("From SparseMap<TileIndex, ElementIndex>") {
            SparseMap<TileIndex, ElementIndex> sm;
            sm.add_to_domain(t0, e1);

            SECTION("TiledRange is set") {
                TA::TiledRange tr{{0, 2, 4}};
                sm.set_trange(tr);
                sm_t sm2(sm);
                sm_t corr;
                corr.add_to_domain(e0, e1);
                corr.add_to_domain(e1, e1);
                REQUIRE(sm2 == corr);
            }

            SECTION("TiledRange is not set") {
                REQUIRE_THROWS_AS(sm_t(sm), std::runtime_error);
            }
        }

        SECTION("From SparseMap<TileIndex, TileIndex") {
            SparseMap<TileIndex, TileIndex> sm;
            sm.add_to_domain(t1, t0);

            SECTION("TiledRanges are set") {
                TA::TiledRange tr{{0, 2, 4}};
                sm.set_domain_trange(tr);
                sm.set_trange(tr);

                sm_t sm2(sm);
                sm_t corr;
                corr.add_to_domain(e2, e0);
                corr.add_to_domain(e2, e1);
                corr.add_to_domain(e3, e0);
                corr.add_to_domain(e3, e1);
                REQUIRE(sm2 == corr);
            }

            SECTION("Independent TiledRange is not set") {
                TA::TiledRange tr{{0, 2, 4}};
                sm.set_domain_trange(tr);
                REQUIRE_THROWS_AS(sm_t(sm), std::runtime_error);
            }

            SECTION("Dependent TiledRange is not set") {
                TA::TiledRange tr{{0, 2, 4}};
                sm.set_trange(tr);
                REQUIRE_THROWS_AS(sm_t(sm), std::runtime_error);
            }
        }
    }
}

TEST_CASE("SparseMap<ElementIndex, TileIndex>") {
    using sm_t = SparseMap<ElementIndex, TileIndex>;
    ElementIndex e0{0}, e1{1}, e2{2}, e3{3};
    TileIndex t0{0}, t1{1}, t2{2};

    SECTION("set_domain_trange") {
        sm_t sm;
        TA::TiledRange tr{{0, 2, 4}};
        sm.add_to_domain(e0, t0);
        sm.add_to_domain(e1, t1);
        sm.set_domain_trange(tr);
        REQUIRE(sm.at(e0).trange() == tr);
        REQUIRE(sm.at(e1).trange() == tr);
    }
}

TEST_CASE("SparseMap<TileIndex, ElementIndex>") {
    using sm_t = SparseMap<TileIndex, ElementIndex>;
    ElementIndex e0{0}, e1{1}, e2{2}, e3{3};
    TileIndex t0{0}, t1{1}, t2{2};
    TA::TiledRange tr{{0, 2, 4}};

    SECTION("Ctors") {
        SECTION("From SparseMap<TileIndex, TileIndex>") {
            SparseMap<TileIndex, TileIndex> sm;
            sm.add_to_domain(t1, t0);

            SECTION("TiledRanges are set") {
                sm.set_trange(tr);
                sm.set_domain_trange(tr);

                sm_t sm2(sm);
                sm_t corr;
                corr.set_trange(tr);
                corr.add_to_domain(t1, e0);
                corr.add_to_domain(t1, e1);
                REQUIRE(sm2 == corr);
            }

            SECTION("Only Dependent is set") {
                sm.set_domain_trange(tr);

                sm_t sm2(sm);
                sm_t corr;
                corr.add_to_domain(t1, e0);
                corr.add_to_domain(t1, e1);
                REQUIRE(sm2 == corr);
            }

            SECTION("Only independent is set") {
                sm.set_trange(tr);
                REQUIRE_THROWS_AS(sm_t(sm), std::runtime_error);
            }
        }

        SECTION("From SparseMap<ElementIndex, ElementIndex>") {
            SparseMap<ElementIndex, ElementIndex> sm;
            SECTION("Good indices") {
                sm.add_to_domain(e0, e1);
                sm_t sm2(tr, sm);
                sm_t corr;
                corr.set_trange(tr);
                corr.add_to_domain(t0, e1);
                REQUIRE(sm2 == corr);
            }

            SECTION("TiledRange rank does not match independent rank") {
                sm.add_to_domain(ElementIndex{1, 1}, e0);
                REQUIRE_THROWS_AS(sm_t(tr, sm), std::runtime_error);
            }
        }
    } // SECTION("Ctors")

    SECTION("set_trange") {
        sm_t sm;
        sm.add_to_domain(t0, e0);
        sm.set_trange(tr);
        REQUIRE(sm.trange() == tr);
    }

    SECTION("trange"){
        SECTION("No trange") {
            sm_t sm;
            REQUIRE(sm.trange() == TA::TiledRange());
        }
        SECTION("Has trnange") {
            sm_t sm;
            sm.set_trange(tr);
            REQUIRE(sm.trange() == tr);
        }
    }
}

TEST_CASE("SparseMap<TileIndex, TileIndex>") {
    using sm_t = SparseMap<TileIndex, TileIndex>;

    TA::TiledRange tr{{0, 2, 4}};
    ElementIndex e0{0}, e1{1}, e2{2}, e3{3};
    TileIndex t0{0}, t1{1}, t2{2};

    SECTION("set_trange") {
        sm_t sm;
        sm.set_trange(tr);
        REQUIRE(sm.trange() == tr);
    }

    SECTION("trange") {
        sm_t sm;

        SECTION("No trange") {
            REQUIRE(sm.trange() == TA::TiledRange{});
        }

        SECTION("Has a trange") {
            sm.set_trange(tr);
            REQUIRE(sm.trange() == tr);
        }
    }

    SECTION("set_domain_trange") {
        sm_t sm;
        sm.add_to_domain(t0, t0);
        sm.add_to_domain(t1, t1);
        sm.set_domain_trange(tr);
        REQUIRE(sm.at(t0).trange() == tr);
        REQUIRE(sm.at(t1).trange() == tr);
    }
}
