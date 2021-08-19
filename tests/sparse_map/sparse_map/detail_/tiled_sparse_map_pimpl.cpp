#include "libchemist/sparse_map/index.hpp"
#include "libchemist/sparse_map/sparse_map/detail_/tiled_sparse_map_pimpl.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::sparse_map;
using namespace libchemist::sparse_map::detail_;

using index_types = std::tuple<TileIndex, ElementIndex>;

TEMPLATE_LIST_TEST_CASE("TiledSparseMapPIMPL", "", index_types) {
    using base_pimpl = SparseMapPIMPL<TileIndex, TestType>;
    using pimpl_t    = TiledSparseMapPIMPL<base_pimpl>;
    using domain_t   = typename pimpl_t::mapped_type;

    TileIndex i1{1}, i11{1, 1}, i12{1, 2}, i1212{1, 2, 1, 2}, i1223{1, 2, 2, 3},
      i2{2}, i23{2, 3}, i2312{2, 3, 1, 2}, i2323{2, 3, 2, 3};
    TestType d1{1}, d11{1, 1}, d12{1, 2}, d23{2, 3}, d1212{1, 2, 1, 2},
      d1223{1, 2, 2, 3}, d2312{2, 3, 1, 2}, d2323{2, 3, 2, 3};

    TA::TiledRange empty;
    TA::TiledRange tr2{{0, 2, 4, 6}, {0, 2, 4, 6, 8}};

    std::map<std::string, pimpl_t> sms;
    sms["empty"];
    sms["Ind == rank 1"].add_to_domain(i1, d1);
    sms["Ind == rank 2"].set_trange(tr2);
    sms["Ind == rank 2"].add_to_domain(i12, d12);
    sms["Ind == rank 2"].add_to_domain(i23, d23);

    SECTION("CTors") {
        SECTION("empty") {
            auto& sm = sms.at("empty");
            REQUIRE(sm.trange() == empty);
        }

        SECTION("clone") {
            for(const auto& [k, v] : sms) {
                SECTION(k) {
                    auto copy = v.clone();
                    REQUIRE(*copy == v);
                }
            }
        }
    } // SECTION("CTors")

    SECTION("trange") {
        REQUIRE(sms.at("empty").trange() == empty);
        REQUIRE(sms.at("Ind == rank 1").trange() == empty);
        REQUIRE(sms.at("Ind == rank 2").trange() == tr2);
    }

    SECTION("set_trange") {
        SECTION("empty") {
            auto& sm = sms.at("empty");
            sm.set_trange(tr2);
            REQUIRE(sm.trange() == tr2);
        }

        SECTION("filled set to valid TiledRange") {
            auto& sm = sms.at("Ind == rank 1");
            TA::TiledRange tr1{{0, 2, 4}};
            sm.set_trange(tr1);
            REQUIRE(sm.trange() == tr1);
        }

        SECTION("filled set to invalid TiledRange") {
            auto& sm = sms.at("Ind == rank 1");
            REQUIRE_THROWS_AS(sm.set_trange(tr2), std::out_of_range);
        }
    }

    SECTION("add_to_domain") {
        SECTION("No TiledRange") {
            auto& sm = sms.at("Ind == rank 1");
            sm.add_to_domain(i2, d1);
            REQUIRE(sm.at(1).first == i2);
            REQUIRE(sm.at(1).second == Domain<TestType>{d1});
        }

        SECTION("TiledRange") {
            auto& sm = sms.at("Ind == rank 2");
            SECTION("Good index") {
                sm.add_to_domain(i12, d23);
                REQUIRE(sm.at(0).first == i12);
                REQUIRE(sm.at(0).second == Domain<TestType>{d12, d23});
            }
            SECTION("Bad index") {
                REQUIRE_THROWS_AS(sm.add_to_domain(TileIndex{9, 9}, d23),
                                  std::out_of_range);
            }
        }
    } // SECTION("add_to_domain")

    SECTION("direct_product_assign") {
        SECTION("Both don't have TiledRange") {
            auto& sm = sms.at("Ind == rank 1");
            auto psm = &(sm.direct_product_assign(sm));
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
            SECTION("Value") {
                pimpl_t corr;
                corr.add_to_domain(i11, d11);
                REQUIRE(sm == corr);
            }
        }

        SECTION("Both have TiledRange") {
            auto& sm = sms.at("Ind == rank 2");
            auto psm = &(sm.direct_product_assign(sm));
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
            SECTION("Value") {
                TA::TiledRange tr4{
                  {0, 2, 4, 6}, {0, 2, 4, 6, 8}, {0, 2, 4, 6}, {0, 2, 4, 6, 8}};
                pimpl_t corr;
                corr.set_trange(tr4);
                corr.add_to_domain(i1212, d1212);
                corr.add_to_domain(i1223, d1223);
                corr.add_to_domain(i2312, d2312);
                corr.add_to_domain(i2323, d2323);
                REQUIRE(sm == corr);
            }
        }

        SECTION("LHS has TiledRange, RHS doesn't") {
            auto& lhs = sms.at("Ind == rank 2");
            pimpl_t rhs;
            rhs.add_to_domain(i12, d12);
            rhs.add_to_domain(i23, d23);
            REQUIRE_THROWS_AS(lhs.direct_product_assign(rhs),
                              std::runtime_error);
        }

        SECTION("RHS has TiledRange, LHS doesn't") {
            auto& rhs = sms.at("Ind == rank 2");
            pimpl_t lhs;
            lhs.add_to_domain(i12, d12);
            lhs.add_to_domain(i23, d23);
            REQUIRE_THROWS_AS(lhs.direct_product_assign(rhs),
                              std::runtime_error);
        }

        SECTION("RHS isn't TiledSparseMapPIMPL") {
            auto& lhs = sms.at("Ind == rank 2");
            base_pimpl rhs;
            REQUIRE_THROWS_AS(lhs.direct_product_assign(rhs),
                              std::runtime_error);
        }
    } // SECTION("direct_product_assign")

    SECTION("operator*=") {
        SECTION("LHS is empty, RHS isn't") {
            auto& lhs = sms.at("empty");
            auto& rhs = sms.at("Ind == rank 2");
            auto plhs = &(lhs *= rhs);
            SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
            SECTION("Value") { REQUIRE(lhs == pimpl_t{}); }
        }

        SECTION("LHS is non-empty, RHS is empty") {
            auto& rhs = sms.at("empty");
            auto& lhs = sms.at("Ind == rank 2");
            auto plhs = &(lhs *= rhs);
            SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
            SECTION("Value") { REQUIRE(lhs == pimpl_t{}); }
        }

        SECTION("Both don't have TiledRange") {
            auto& sm = sms.at("Ind == rank 1");
            auto psm = &(sm *= sm);
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
            SECTION("Value") {
                pimpl_t corr;
                corr.add_to_domain(i1, d11);
                REQUIRE(sm == corr);
            }
        }

        SECTION("Both have TiledRange") {
            auto& sm = sms.at("Ind == rank 2");
            auto psm = &(sm *= sm);
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
            SECTION("Value") {
                pimpl_t corr;
                corr.set_trange(tr2);
                corr.add_to_domain(i12, d1212);
                corr.add_to_domain(i23, d2323);
                REQUIRE(sm == corr);
            }
        }

        SECTION("LHS has TiledRange, RHS doesn't") {
            auto& lhs = sms.at("Ind == rank 2");
            pimpl_t rhs;
            rhs.add_to_domain(i12, d12);
            rhs.add_to_domain(i23, d23);
            REQUIRE_THROWS_AS(lhs *= rhs, std::runtime_error);
        }

        SECTION("RHS has TiledRange, LHS doesn't") {
            auto& rhs = sms.at("Ind == rank 2");
            pimpl_t lhs;
            lhs.add_to_domain(i12, d12);
            lhs.add_to_domain(i23, d23);
            REQUIRE_THROWS_AS(lhs *= rhs, std::runtime_error);
        }

        SECTION("RHS is not a TiledSparseMapPIMPL") {
            auto& lhs = sms.at("Ind == rank 2");
            base_pimpl rhs;
            REQUIRE_THROWS_AS(lhs *= rhs, std::runtime_error);
        }

    } // SECTION("operator*=")

    SECTION("operator+=") {
        SECTION("LHS is empty, RHS isn't") {
            auto& lhs = sms.at("empty");
            auto& rhs = sms.at("Ind == rank 2");
            auto plhs = &(lhs += rhs);
            SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
            SECTION("Value") { REQUIRE(lhs == rhs); }
        }

        SECTION("LHS is non-empty, RHS is empty") {
            auto& rhs = sms.at("empty");
            auto& lhs = sms.at("Ind == rank 2");
            auto plhs = &(lhs += rhs);
            SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
            SECTION("Value") {
                pimpl_t corr;
                corr.set_trange(tr2);
                corr.add_to_domain(i12, d12);
                corr.add_to_domain(i23, d23);
                REQUIRE(lhs == corr);
            }
        }

        SECTION("Both don't have TiledRange") {
            auto& sm = sms.at("Ind == rank 1");
            auto psm = &(sm += sm);
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
            SECTION("Value") {
                pimpl_t corr;
                corr.add_to_domain(i1, d1);
                REQUIRE(sm == corr);
            }
        }

        SECTION("Both have TiledRange") {
            auto& sm = sms.at("Ind == rank 2");
            auto psm = &(sm += sm);
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
            SECTION("Value") {
                pimpl_t corr;
                corr.set_trange(tr2);
                corr.add_to_domain(i12, d12);
                corr.add_to_domain(i23, d23);
                REQUIRE(sm == corr);
            }
        }

        SECTION("LHS has TiledRange, RHS doesn't") {
            auto& lhs = sms.at("Ind == rank 2");
            pimpl_t rhs;
            rhs.add_to_domain(i12, d12);
            rhs.add_to_domain(i23, d23);
            REQUIRE_THROWS_AS(lhs += rhs, std::runtime_error);
        }

        SECTION("RHS has TiledRange, LHS doesn't") {
            auto& rhs = sms.at("Ind == rank 2");
            pimpl_t lhs;
            lhs.add_to_domain(i12, d12);
            lhs.add_to_domain(i23, d23);
            REQUIRE_THROWS_AS(lhs += rhs, std::runtime_error);
        }

        SECTION("RHS is not a TiledSparseMapPIMPL") {
            auto& lhs = sms.at("Ind == rank 2");
            base_pimpl rhs;
            REQUIRE_THROWS_AS(lhs += rhs, std::runtime_error);
        }

    } // SECTION("operator+=")

    SECTION("operator^=") {
        SECTION("LHS is empty, RHS isn't") {
            auto& lhs = sms.at("empty");
            auto& rhs = sms.at("Ind == rank 2");
            auto plhs = &(lhs ^= rhs);
            SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
            SECTION("Value") { REQUIRE(lhs == pimpl_t{}); }
        }

        SECTION("LHS is non-empty, RHS is empty") {
            auto& rhs = sms.at("empty");
            auto& lhs = sms.at("Ind == rank 2");
            auto plhs = &(lhs ^= rhs);
            SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
            SECTION("Value") { REQUIRE(lhs == pimpl_t{}); }
        }

        SECTION("Both don't have TiledRange") {
            auto& sm = sms.at("Ind == rank 1");
            auto psm = &(sm ^= sm);
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
            SECTION("Value") {
                pimpl_t corr;
                corr.add_to_domain(i1, d1);
                REQUIRE(sm == corr);
            }
        }

        SECTION("Both have TiledRange") {
            auto& sm = sms.at("Ind == rank 2");
            auto psm = &(sm ^= sm);
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
            SECTION("Value") {
                pimpl_t corr;
                corr.set_trange(tr2);
                corr.add_to_domain(i12, d12);
                corr.add_to_domain(i23, d23);
                REQUIRE(sm == corr);
            }
        }

        SECTION("LHS has TiledRange, RHS doesn't") {
            auto& lhs = sms.at("Ind == rank 2");
            pimpl_t rhs;
            rhs.add_to_domain(i12, d12);
            rhs.add_to_domain(i23, d23);
            REQUIRE_THROWS_AS(lhs ^= rhs, std::runtime_error);
        }

        SECTION("RHS has TiledRange, LHS doesn't") {
            auto& rhs = sms.at("Ind == rank 2");
            pimpl_t lhs;
            lhs.add_to_domain(i12, d12);
            lhs.add_to_domain(i23, d23);
            REQUIRE_THROWS_AS(lhs ^= rhs, std::runtime_error);
        }

        SECTION("RHS is not a TiledSparseMapPIMPL") {
            auto& lhs = sms.at("Ind == rank 2");
            base_pimpl rhs;
            REQUIRE_THROWS_AS(lhs ^= rhs, std::runtime_error);
        }
    } // SECTION("operator^=")

    /* Testing strategy:
     *
     * We know the base class's equal_ function works so we only worry about:
     *
     * - testing the comparison between the LHS and RHS's TiledRange instances.
     * - testing when one side is not a TiledSparseMapPIMPL.
     */
    SECTION("operator==") {
        SECTION("LHS does not have a TiledRange") {
            const auto& sm = sms.at("empty");

            SECTION("RHS is also empty") { REQUIRE(sm == pimpl_t{}); }

            SECTION("RHS has a TiledRange") {
                pimpl_t p;
                p.set_trange(tr2);
                REQUIRE_FALSE(sm == p);
            }
        }

        SECTION("LHS has a TiledRange") {
            const auto& sm = sms.at("Ind == rank 2");
            pimpl_t p;

            SECTION("Is the same") {
                p.set_trange(tr2);
                p.add_to_domain(i12, d12);
                p.add_to_domain(i23, d23);
                REQUIRE(sm == p);
            }

            SECTION("Same, but no TiledRange") {
                p.add_to_domain(i12, d12);
                p.add_to_domain(i23, d23);
                REQUIRE_FALSE(sm == p);
            }

            SECTION("Same TiledRange, but different indices") {
                p.set_trange(tr2);
                p.add_to_domain(i12, d12);
                REQUIRE_FALSE(sm == p);
            }
        }

        SECTION("LHS is not a TiledSparseMapPIMPL") {
            pimpl_t sm;
            sm.add_to_domain(i12, d12);
            sm.add_to_domain(i23, d23);
            REQUIRE_FALSE(sms.at("Ind == rank 2") == sm);
        }
    }
}
