#include "libchemist/sparse_map/sparse_map/detail_/sparse_map_pimpl.hpp"
#include "libchemist/sparse_map/index.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::sparse_map;
using namespace libchemist::sparse_map::detail_;

using index_list = std::tuple<
  std::pair<ElementIndex, ElementIndex>, std::pair<ElementIndex, TileIndex>,
  std::pair<TileIndex, ElementIndex>, std::pair<TileIndex, TileIndex>>;

/* General notes on testing:
 *
 * - We know that the Domain class works from unit testing it. We use a variety
 *   of Domains in these unit tests, but do not attempt to be exhaustive for
 *   that reason. For testing, what matters more is the interaction of the
 *   Domain with the SparseMap class.
 *
 */

TEMPLATE_LIST_TEST_CASE("SparseMapPIMPL", "", index_list) {
    using ind_idx_t = std::tuple_element_t<0, TestType>;
    using dep_idx_t = std::tuple_element_t<1, TestType>;
    using pimpl_t   = SparseMapPIMPL<ind_idx_t, dep_idx_t>;
    using domain_t  = Domain<dep_idx_t>;

    ind_idx_t i0{}, i1{1}, i12{2}, i2{1, 2}, i22{2, 3};
    dep_idx_t d0{}, d1{1}, d12{2}, d2{1, 2}, d22{2, 3};

    std::map<std::string, pimpl_t> sms;
    sms["Empty"];
    sms["Ind == rank 0"].add_to_domain(i0, d1);
    sms["Ind == rank 0"].add_to_domain(i0, d12);
    sms["Ind == rank 1"].add_to_domain(i1, d1);
    sms["Ind == rank 2"].add_to_domain(i2, d2);
    sms["Ind == rank 2"].add_to_domain(i22, d22);

    SECTION("Typedefs") {
        using traits = SparseMapTraits<SparseMap<ind_idx_t, dep_idx_t>>;

        SECTION("size_type") {
            using corr_t = typename traits::size_type;
            using the_t  = typename pimpl_t::size_type;
            STATIC_REQUIRE(std::is_same_v<corr_t, the_t>);
        }

        SECTION("key_type") {
            using corr_t = typename traits::key_type;
            using the_t  = typename pimpl_t::key_type;
            STATIC_REQUIRE(std::is_same_v<corr_t, the_t>);
        }

        SECTION("mapped_type") {
            using corr_t = typename traits::mapped_type;
            using the_t  = typename pimpl_t::mapped_type;
            STATIC_REQUIRE(std::is_same_v<corr_t, the_t>);
        }
    }

    SECTION("CTors") {

        SECTION("Default Ctor") {
            auto& sm = sms.at("Empty");
            REQUIRE(sm.size() == 0);
            REQUIRE(sm.ind_rank() == 0);
            REQUIRE(sm.dep_rank() == 0);
        }

        SECTION("clone") {
            for(const auto& [k, v] : sms) {
                SECTION(k) {
                    auto psm = v.clone();
                    REQUIRE(*psm == v);
                }
            }
        }
    } // SECTION("CTORS")

    SECTION("size") {
        SECTION("Empty") { REQUIRE(sms.at("Empty").size() == 0); }

        SECTION("Ind == rank 0") {
            auto& sm0 = sms.at("Ind == rank 0");
            REQUIRE(sm0.size() == 1);
        }

        SECTION("Ind == rank 1") {
            auto& sm1 = sms.at("Ind == rank 1");
            REQUIRE(sm1.size() == 1);
        }

        SECTION("Ind == rank 2") {
            auto& sm2 = sms.at("Ind == rank 2");
            REQUIRE(sm2.size() == 2);
        }
    }

    SECTION("count") {
        SECTION("Empty") { REQUIRE_FALSE(sms.at("Empty").count(i0)); }

        SECTION("Ind == rank 0") {
            auto& sm0 = sms.at("Ind == rank 0");
            SECTION("Has index") { REQUIRE(sm0.count(i0)); }
            SECTION("Doesn't have") { REQUIRE_FALSE(sm0.count(i1)); }
        }

        SECTION("Ind == rank 1") {
            auto& sm1 = sms.at("Ind == rank 1");
            SECTION("Has index") { REQUIRE(sm1.count(i1)); }
            SECTION("Doesn't have") { REQUIRE_FALSE(sm1.count(i0)); }
        }

        SECTION("Ind == rank 2") {
            auto& sm2 = sms.at("Ind == rank 2");
            SECTION("Has index") { REQUIRE(sm2.count(i2)); }
            SECTION("Doesn't have") { REQUIRE_FALSE(sm2.count(i0)); }
        }
    }

    SECTION("ind_rank") {
        SECTION("Empty") { REQUIRE(sms.at("Empty").ind_rank() == 0); }

        SECTION("Ind == rank 0") {
            auto& sm0 = sms.at("Ind == rank 0");
            REQUIRE(sm0.ind_rank() == 0);
        }

        SECTION("Ind == rank 1") {
            auto& sm1 = sms.at("Ind == rank 1");
            REQUIRE(sm1.ind_rank() == 1);
        }

        SECTION("Ind == rank 2") {
            auto& sm2 = sms.at("Ind == rank 2");
            REQUIRE(sm2.ind_rank() == 2);
        }
    }

    SECTION("dep_rank") {
        SECTION("Empty") { REQUIRE(sms.at("Empty").dep_rank() == 0); }

        SECTION("Ind == rank 0") {
            auto& sm0 = sms.at("Ind == rank 0");
            REQUIRE(sm0.dep_rank() == 1);
        }

        SECTION("Ind == rank 1") {
            auto& sm1 = sms.at("Ind == rank 1");
            REQUIRE(sm1.dep_rank() == 1);
        }

        SECTION("Ind == rank 2") {
            auto& sm2 = sms.at("Ind == rank 2");
            REQUIRE(sm2.dep_rank() == 2);
        }
    }

    SECTION("add_to_domain") {
        SECTION("Empty") {
            auto& sm = sms.at("Empty");
            sm.add_to_domain(i0, d0);
            REQUIRE(sm.size() == 1);
            REQUIRE(sm.ind_rank() == 0);
            REQUIRE(sm.dep_rank() == 0);
            REQUIRE(sm.at(0).first == i0);
            REQUIRE(sm.at(0).second == domain_t{d0});
        }

        SECTION("Ind == rank 0") {
            auto& sm0 = sms.at("Ind == rank 0");
            SECTION("Throws if independent rank is wrong") {
                REQUIRE_THROWS_AS(sm0.add_to_domain(i1, d1),
                                  std::runtime_error);
            }
            SECTION("Throws if dependent rank is wrong") {
                REQUIRE_THROWS_AS(sm0.add_to_domain(i0, d0),
                                  std::runtime_error);
            }
            SECTION("Add to existing independent index") {
                sm0.add_to_domain(i0, dep_idx_t{3});
                REQUIRE(sm0.size() == 1);
                REQUIRE(sm0.ind_rank() == 0);
                REQUIRE(sm0.dep_rank() == 1);
                REQUIRE(sm0.at(0).first == i0);
                REQUIRE(sm0.at(0).second == domain_t{d1, d12, dep_idx_t{3}});
            }
        }

        SECTION("Ind == rank 1") {
            auto& sm1 = sms.at("Ind == rank 1");
            SECTION("Throws if independent rank is wrong") {
                REQUIRE_THROWS_AS(sm1.add_to_domain(i0, d1),
                                  std::runtime_error);
            }
            SECTION("Throws if dependent rank is wrong") {
                REQUIRE_THROWS_AS(sm1.add_to_domain(i1, d0),
                                  std::runtime_error);
            }
            SECTION("Add to existing independent index") {
                sm1.add_to_domain(i1, d12);
                REQUIRE(sm1.size() == 1);
                REQUIRE(sm1.ind_rank() == 1);
                REQUIRE(sm1.dep_rank() == 1);
                REQUIRE(sm1.at(0).first == i1);
                REQUIRE(sm1.at(0).second == domain_t{d1, d12});
            }
            SECTION("Add to non-existing independent index") {
                sm1.add_to_domain(ind_idx_t{4}, d12);
                REQUIRE(sm1.size() == 2);
                REQUIRE(sm1.ind_rank() == 1);
                REQUIRE(sm1.dep_rank() == 1);
                REQUIRE(sm1.at(0).first == i1);
                REQUIRE(sm1.at(0).second == domain_t{d1});
                REQUIRE(sm1.at(1).first == ind_idx_t{4});
                REQUIRE(sm1.at(1).second == domain_t{d12});
            }
        }

        SECTION("Ind == rank 2") {
            auto& sm2 = sms.at("Ind == rank 2");
            SECTION("Throws if independent rank is wrong") {
                REQUIRE_THROWS_AS(sm2.add_to_domain(i0, d2),
                                  std::runtime_error);
            }
            SECTION("Throws if dependent rank is wrong") {
                REQUIRE_THROWS_AS(sm2.add_to_domain(i2, d0),
                                  std::runtime_error);
            }
            SECTION("Add to existing independent index") {
                sm2.add_to_domain(i2, dep_idx_t{3, 4});
                REQUIRE(sm2.size()       == 2);
                REQUIRE(sm2.ind_rank()   == 2);
                REQUIRE(sm2.dep_rank()   == 2);
                REQUIRE(sm2.at(0).first  == i2);
                REQUIRE(sm2.at(0).second == domain_t{d2, dep_idx_t{3, 4}});
                REQUIRE(sm2.at(1).first  == i22);
                REQUIRE(sm2.at(1).second == domain_t{d22});
            }
            SECTION("Add to non-existing independent index") {
                sm2.add_to_domain(ind_idx_t{3, 4}, d2);
                pimpl_t corr;
                corr.add_to_domain(i2, d2);
                corr.add_to_domain(i22, d22);
                corr.add_to_domain(ind_idx_t{3, 4}, d2);
                REQUIRE(sm2.size() == 3);
                REQUIRE(sm2.ind_rank() == 2);
                REQUIRE(sm2.dep_rank() == 2);
                REQUIRE(sm2.at(0).first == i2);
                REQUIRE(sm2.at(0).second == domain_t{d2});
                REQUIRE(sm2.at(1).first  == i22);
                REQUIRE(sm2.at(1).second == domain_t{d22});
                REQUIRE(sm2.at(2).first == ind_idx_t{3, 4});
                REQUIRE(sm2.at(2).second == domain_t{d2});
            }
        }
    }

    SECTION("at() const") {
        SECTION("Empty") {
            const auto& sm = sms.at("Empty");
            REQUIRE_THROWS_AS(sm.at(i0), std::out_of_range);
        }

        SECTION("Ind == rank 0") {
            const auto& sm = sms.at("Ind == rank 0");
            SECTION("Throws if wrong ind rank") {
                REQUIRE_THROWS_AS(sm.at(i1), std::runtime_error);
            }
            SECTION("Value") { REQUIRE(sm.at(i0) == domain_t{d1, d12}); }
        }

        SECTION("Ind == rank 1") {
            const auto& sm = sms.at("Ind == rank 1");
            SECTION("Throws if wrong ind rank") {
                REQUIRE_THROWS_AS(sm.at(i0), std::runtime_error);
            }
            SECTION("Throws if value is not present") {
                REQUIRE_THROWS_AS(sm.at(ind_idx_t{4}), std::out_of_range);
            }
            SECTION("Value") { REQUIRE(sm.at(i1) == domain_t{d1}); }
        }

        SECTION("Ind == rank 2") {
            const auto& sm = sms.at("Ind == rank 2");
            SECTION("Throws if wrong ind rank") {
                REQUIRE_THROWS_AS(sm.at(i1), std::runtime_error);
            }
            SECTION("Throws if value is not present") {
                ind_idx_t i23{3, 4};
                REQUIRE_THROWS_AS(sm.at(i23), std::out_of_range);
            }
            SECTION("Value") { REQUIRE(sm.at(i2) == domain_t{d2}); }
        }

    } // SECTION("at() const")

    SECTION("direct_product_assign") {
        SECTION("LHS == Empty") {
            auto& lhs = sms.at("Empty");
            pimpl_t corr;

            for(auto& [key, rhs] : sms) {
                SECTION("RHS == " + key) {
                    auto plhs = &(lhs.direct_product_assign(rhs));
                    SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                    SECTION("Value") { REQUIRE(lhs == corr); }
                }
            }
        }

        SECTION("LHS == rank 0") {
            auto& lhs = sms.at("Ind == rank 0");

            SECTION("RHS == Empty") {
                auto& rhs   = sms.at("Empty");
                auto plhs   = &(lhs.direct_product_assign(rhs));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == rhs); }
            }

            SECTION("RHS == Ind == rank 0") {
                auto& rhs = sms.at("Ind == rank 0");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_t corr;
                    corr.add_to_domain(i0, dep_idx_t{1, 1});
                    corr.add_to_domain(i0, dep_idx_t{1, 2});
                    corr.add_to_domain(i0, dep_idx_t{2, 1});
                    corr.add_to_domain(i0, dep_idx_t{2, 2});
                    REQUIRE(lhs == corr);
                }
            }

            SECTION("RHS == Ind == rank 1") {
                auto& rhs = sms.at("Ind == rank 1");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") {
                    pimpl_t corr;
                    corr.add_to_domain(i1, dep_idx_t{1, 1});
                    corr.add_to_domain(i1, dep_idx_t{2, 1});
                    REQUIRE(lhs == corr);
                }
            }

            SECTION("RHS == Ind == rank 2") {
                auto& rhs = sms.at("Ind == rank 2");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") {
                    pimpl_t corr;
                    corr.add_to_domain(i2, dep_idx_t{1, 1, 2});
                    corr.add_to_domain(i2, dep_idx_t{2, 1, 2});
                    corr.add_to_domain(i22, dep_idx_t{1, 2, 3});
                    corr.add_to_domain(i22, dep_idx_t{2, 2, 3});
                    REQUIRE(lhs == corr);
                }
            }
        }

        SECTION("LHS == rank 1") {
            auto& lhs = sms.at("Ind == rank 1");

            SECTION("RHS == empty") {
                auto& rhs = sms.at("Empty");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") { REQUIRE(lhs == rhs); }
            }

            SECTION("RHS == Ind == rank 0") {
                auto& rhs = sms.at("Ind == rank 0");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") {
                    pimpl_t corr;
                    corr.add_to_domain(i1, dep_idx_t{1, 1});
                    corr.add_to_domain(i1, dep_idx_t{1, 2});
                    REQUIRE(lhs == corr);
                }
            }

            SECTION("RHS == Ind == rank 1") {
                auto& rhs = sms.at("Ind == rank 1");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") {
                    pimpl_t corr;
                    corr.add_to_domain(ind_idx_t{1, 1}, dep_idx_t{1, 1});
                    REQUIRE(lhs == corr);
                }
            }

            SECTION("RHS == Ind == rank 2") {
                auto& rhs = sms.at("Ind == rank 2");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") {
                    pimpl_t corr;
                    corr.add_to_domain(ind_idx_t{1, 1, 2}, dep_idx_t{1, 1, 2});
                    corr.add_to_domain(ind_idx_t{1, 2, 3}, dep_idx_t{1, 2, 3});
                    REQUIRE(lhs == corr);
                }
            }
        }

        SECTION("LHS == rank 2") {
            auto& lhs = sms.at("Ind == rank 2");

            SECTION("RHS == empty") {
                auto& rhs = sms.at("Empty");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") { REQUIRE(lhs == rhs); }
            }

            SECTION("RHS == Ind == rank 0") {
                auto& rhs = sms.at("Ind == rank 0");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") {
                    pimpl_t corr;
                    corr.add_to_domain(i2, dep_idx_t{1, 2, 1});
                    corr.add_to_domain(i2, dep_idx_t{1, 2, 2});
                    corr.add_to_domain(i22, dep_idx_t{2, 3, 1});
                    corr.add_to_domain(i22, dep_idx_t{2, 3, 2});
                    REQUIRE(lhs == corr);
                }
            }

            SECTION("RHS == Ind == rank 1") {
                auto& rhs = sms.at("Ind == rank 1");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") {
                    pimpl_t corr;
                    corr.add_to_domain(ind_idx_t{1, 2, 1}, dep_idx_t{1, 2, 1});
                    corr.add_to_domain(ind_idx_t{2, 3, 1}, dep_idx_t{2, 3, 1});
                    REQUIRE(lhs == corr);
                }
            }

            SECTION("RHS == Ind == rank 2") {
                auto& rhs = sms.at("Ind == rank 2");
                auto plhs = &(lhs.direct_product_assign(rhs));
                SECTION("returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") {
                    pimpl_t corr;
                    corr.add_to_domain(ind_idx_t{1, 2, 1, 2}, dep_idx_t{1, 2, 1, 2});
                    corr.add_to_domain(ind_idx_t{1, 2, 2, 3}, dep_idx_t{1, 2, 2, 3});
                    corr.add_to_domain(ind_idx_t{2, 3, 1, 2}, dep_idx_t{2, 3, 1, 2});
                    corr.add_to_domain(ind_idx_t{2, 3, 2, 3}, dep_idx_t{2, 3, 2, 3});
                    REQUIRE(lhs == corr);
                }
            }
        }

    }

    /* With respect to direct product operator*= does all the work and
     * operator* simply calls operator*= on a copy. Therefore we test
     * operator*= in depth and simply make sure operator* works for one
     * scenario.
     */
    SECTION("operator*=") {
        SECTION("LHS == empty") {
            auto& lhs = sms.at("Empty");

            SECTION("RHS == empty") {
                pimpl_t rhs;

                SECTION("lhs *= rhs") {
                    auto plhs = &(lhs *= rhs);
                    SECTION("Value") { REQUIRE(lhs == rhs); }
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                }

                SECTION("rhs *= lhs") {
                    auto prhs = &(rhs *= lhs);
                    SECTION("Value") { REQUIRE(rhs == lhs); }
                    SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                }
            }

            SECTION("RHS == non-empty") {
                pimpl_t rhs;
                rhs.add_to_domain(ind_idx_t{1}, dep_idx_t{2});

                SECTION("lhs *= rhs") {
                    auto plhs = &(lhs *= rhs);
                    SECTION("Value") { REQUIRE(lhs == pimpl_t{}); }
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                }

                SECTION("rhs *= lhs") {
                    auto prhs = &(rhs *= lhs);
                    SECTION("Value") { REQUIRE(rhs == lhs); }
                    SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                }
            }
        }

        SECTION("LHS == non-empty") {
            pimpl_t lhs;
            lhs.add_to_domain(ind_idx_t{1}, dep_idx_t{1});

            SECTION("RHS same independent, single element domain") {
                pimpl_t rhs;
                rhs.add_to_domain(ind_idx_t{1}, dep_idx_t{2});

                SECTION("lhs *= rhs") {
                    pimpl_t corr;
                    corr.add_to_domain(ind_idx_t{1}, dep_idx_t{1, 2});
                    auto plhs = &(lhs *= rhs);
                    SECTION("Value") { REQUIRE(lhs == corr); }
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                }

                SECTION("rhs *= lhs") {
                    pimpl_t corr;
                    corr.add_to_domain(ind_idx_t{1}, dep_idx_t{2, 1});
                    auto prhs = &(rhs *= lhs);
                    SECTION("Value") { REQUIRE(rhs == corr); }
                    SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                }
            }

            SECTION("RHS same independent, two element domain") {
                pimpl_t rhs;
                rhs.add_to_domain(ind_idx_t{1}, dep_idx_t{2});
                rhs.add_to_domain(ind_idx_t{1}, dep_idx_t{3});

                SECTION("lhs *= rhs") {
                    pimpl_t corr;
                    corr.add_to_domain(ind_idx_t{1}, dep_idx_t{1, 2});
                    corr.add_to_domain(ind_idx_t{1}, dep_idx_t{1, 3});
                    auto plhs = &(lhs *= rhs);
                    SECTION("Value") { REQUIRE(lhs == corr); }
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                }

                SECTION("rhs *= lhs") {
                    pimpl_t corr;
                    corr.add_to_domain(ind_idx_t{1}, dep_idx_t{2, 1});
                    corr.add_to_domain(ind_idx_t{1}, dep_idx_t{3, 1});
                    auto prhs = &(rhs *= lhs);
                    SECTION("Value") { REQUIRE(rhs == corr); }
                    SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                }
            }

            SECTION("RHS different independent, single element domain") {
                pimpl_t rhs;
                rhs.add_to_domain(ind_idx_t{2}, dep_idx_t{2});

                SECTION("lhs *= rhs") {
                    auto plhs = &(lhs *= rhs);
                    SECTION("Value") { REQUIRE(lhs == pimpl_t{}); }
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                }

                SECTION("rhs *= lhs") {
                    auto prhs = &(rhs *= lhs);
                    SECTION("Value") { REQUIRE(rhs == pimpl_t{}); }
                    SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                }
            }

            SECTION("RHS multiple independent") {
                pimpl_t rhs;
                rhs.add_to_domain(ind_idx_t{1}, dep_idx_t{2});
                rhs.add_to_domain(ind_idx_t{2}, dep_idx_t{2});

                SECTION("lhs * rhs") {
                    auto plhs = &(lhs *= rhs);
                    pimpl_t corr;
                    corr.add_to_domain(ind_idx_t{1}, dep_idx_t{1, 2});
                    SECTION("Value") { REQUIRE(lhs == corr); }
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                }

                SECTION("rhs * lhs") {
                    auto prhs = &(rhs *= lhs);
                    pimpl_t corr;
                    corr.add_to_domain(ind_idx_t{1}, dep_idx_t{2, 1});
                    SECTION("Value") { REQUIRE(rhs == corr); }
                    SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                }
            }
        }

        SECTION("Incompatible independent indices") {
            auto& lhs = sms.at("Ind == rank 1");
            auto& rhs = sms.at("Ind == rank 2");
            REQUIRE_THROWS_AS(lhs *= rhs, std::runtime_error);
        }
    }


    /* With respect to union operator+= does all the work and operator+
     * simply calls operator+= on a copy. Therefore we test operator+= in depth
     * and make sure operator+ works for one scenario.
     */
    SECTION("operator+=") {
        SECTION("Empty / Empty") {
            pimpl_t sm, sm2;
            auto psm = &(sm += sm2);
            SECTION("Value") { REQUIRE(sm == sm2); }
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
        }

        SECTION("Empty / Non-empty") {
            pimpl_t sm;
            pimpl_t sm2;
            sm2.add_to_domain(ind_idx_t{1}, dep_idx_t{0});
            sm2.add_to_domain(ind_idx_t{1}, dep_idx_t{3});
            sm2.add_to_domain(ind_idx_t{2}, dep_idx_t{1});
            sm2.add_to_domain(ind_idx_t{2}, dep_idx_t{2});
            SECTION("sm += sm2") {
                auto psm = &(sm += sm2);
                REQUIRE(sm == sm2);
                SECTION("Returns *this") { REQUIRE(psm == &sm); }
            }

            SECTION("sm += sm2") {
                auto psm = &(sm += sm2);
                REQUIRE(sm == sm2);
                SECTION("Returns *this") { REQUIRE(psm == &sm); }
            }
        }

        SECTION("Non-empty / Non-empty") {
            pimpl_t sm;
            sm.add_to_domain(ind_idx_t{1}, dep_idx_t{0});
            sm.add_to_domain(ind_idx_t{1}, dep_idx_t{3});
            sm.add_to_domain(ind_idx_t{2}, dep_idx_t{1});
            sm.add_to_domain(ind_idx_t{2}, dep_idx_t{2});

            SECTION("Compatible") {
                pimpl_t sm2;
                sm2.add_to_domain(ind_idx_t{0}, dep_idx_t{0});
                sm2.add_to_domain(ind_idx_t{0}, dep_idx_t{3});
                sm2.add_to_domain(ind_idx_t{1}, dep_idx_t{1});
                sm2.add_to_domain(ind_idx_t{1}, dep_idx_t{2});
                sm2.add_to_domain(ind_idx_t{2}, dep_idx_t{1});
                sm2.add_to_domain(ind_idx_t{2}, dep_idx_t{2});
                sm2.add_to_domain(ind_idx_t{3}, dep_idx_t{1});
                sm2.add_to_domain(ind_idx_t{3}, dep_idx_t{2});

                pimpl_t corr;
                corr.add_to_domain(ind_idx_t{0}, dep_idx_t{0});
                corr.add_to_domain(ind_idx_t{0}, dep_idx_t{3});
                corr.add_to_domain(ind_idx_t{1}, dep_idx_t{0});
                corr.add_to_domain(ind_idx_t{1}, dep_idx_t{1});
                corr.add_to_domain(ind_idx_t{1}, dep_idx_t{2});
                corr.add_to_domain(ind_idx_t{1}, dep_idx_t{3});
                corr.add_to_domain(ind_idx_t{2}, dep_idx_t{1});
                corr.add_to_domain(ind_idx_t{2}, dep_idx_t{2});
                corr.add_to_domain(ind_idx_t{3}, dep_idx_t{1});
                corr.add_to_domain(ind_idx_t{3}, dep_idx_t{2});
                SECTION("sm += sm2") {
                    auto psm = &(sm += sm2);
                    SECTION("Value") { REQUIRE(sm == corr); }
                    SECTION("Returns *this") { REQUIRE(psm == &sm); }
                }
                SECTION("sm += sm2") {
                    auto psm2 = &(sm2 += sm);
                    SECTION("Value") { REQUIRE(sm2 == corr); }
                    SECTION("Returns *this") { REQUIRE(psm2 == &sm2); }
                }
                SECTION("sm += corr") {
                    auto psm = &(sm += corr);
                    SECTION("Value") { REQUIRE(sm == corr); }
                    SECTION("Returns *this") { REQUIRE(psm == &sm); }
                }
            }

            SECTION("Incompatible independent indices") {
                pimpl_t incompatible;
                incompatible.add_to_domain(ind_idx_t{1, 2}, dep_idx_t{0});
                incompatible.add_to_domain(ind_idx_t{1, 2}, dep_idx_t{3});
                incompatible.add_to_domain(ind_idx_t{2, 3}, dep_idx_t{1});
                incompatible.add_to_domain(ind_idx_t{2, 3}, dep_idx_t{2});
                REQUIRE_THROWS_AS(sm += incompatible,std::runtime_error);
            }

            SECTION("Incompatible dependent indices") {
                pimpl_t incompatible;
                incompatible.add_to_domain(ind_idx_t{1}, dep_idx_t{0, 1});
                incompatible.add_to_domain(ind_idx_t{1}, dep_idx_t{3, 4});
                incompatible.add_to_domain(ind_idx_t{2}, dep_idx_t{1, 2});
                incompatible.add_to_domain(ind_idx_t{2}, dep_idx_t{2, 3});
                REQUIRE_THROWS_AS(sm += incompatible, std::runtime_error);
            }
        }
    }

    SECTION("operator^=") {
        SECTION("Empty / Empty") {
            pimpl_t sm;
            auto psm = &(sm ^= sm);
            SECTION("Value") { REQUIRE(sm == pimpl_t{}); }
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
        }

        SECTION("Empty / Non-empty") {
            pimpl_t sm;
            pimpl_t sm2;
            sm2.add_to_domain(ind_idx_t{1}, dep_idx_t{0});
            sm2.add_to_domain(ind_idx_t{1}, dep_idx_t{3});
            sm2.add_to_domain(ind_idx_t{2}, dep_idx_t{1});
            sm2.add_to_domain(ind_idx_t{2}, dep_idx_t{2});

            SECTION("sm ^= sm2") {
                auto psm = &(sm ^= sm2);
                SECTION("Value") { REQUIRE(sm == pimpl_t{}); }
                SECTION("Returns *this") { REQUIRE(psm == &sm); }
            }

            SECTION("sm2 ^= sm") {
                auto psm2 = &(sm2 ^= sm);
                SECTION("Value") { REQUIRE(sm2 == pimpl_t{}); }
                SECTION("Returns *this") { REQUIRE(psm2 == &sm2); }
            }
        }

        SECTION("Non-empty / Non-empty") {
            pimpl_t sm;
            sm.add_to_domain(ind_idx_t{1}, dep_idx_t{0});
            sm.add_to_domain(ind_idx_t{1}, dep_idx_t{3});
            sm.add_to_domain(ind_idx_t{2}, dep_idx_t{1});
            sm.add_to_domain(ind_idx_t{2}, dep_idx_t{2});
            pimpl_t sm2;
            sm2.add_to_domain(ind_idx_t{0}, dep_idx_t{0});
            sm2.add_to_domain(ind_idx_t{0}, dep_idx_t{3});
            sm2.add_to_domain(ind_idx_t{1}, dep_idx_t{1});
            sm2.add_to_domain(ind_idx_t{1}, dep_idx_t{2});
            sm2.add_to_domain(ind_idx_t{2}, dep_idx_t{1});
            sm2.add_to_domain(ind_idx_t{2}, dep_idx_t{2});
            sm2.add_to_domain(ind_idx_t{3}, dep_idx_t{1});
            sm2.add_to_domain(ind_idx_t{3}, dep_idx_t{2});

            pimpl_t corr;
            corr.add_to_domain(ind_idx_t{2}, dep_idx_t{1});
            corr.add_to_domain(ind_idx_t{2}, dep_idx_t{2});

            SECTION("sm ^= sm2"){
                auto psm = &(sm ^= sm2);
                SECTION("Value") { REQUIRE(sm == corr); }
                SECTION("Returns *this"){ REQUIRE(psm == &sm); }
            }

            SECTION("sm2 ^= sm"){
                auto psm2 = &(sm2 ^= sm);
                SECTION("Value") { REQUIRE(sm2 == corr); }
                SECTION("Returns *this"){ REQUIRE(psm2 == &sm2); }
            }

            SECTION("sm ^= corr"){
                auto psm = &(sm ^= corr);
                SECTION("Value") { REQUIRE(sm == corr); }
                SECTION("Returns *this"){ REQUIRE(psm == &sm); }
            }

            SECTION("different ranks") {
                auto psm = &(sm ^= sms.at("Ind == rank 2"));
                SECTION("Value") { REQUIRE(sm == pimpl_t{}); }
                SECTION("Returns *this") { REQUIRE(psm == &sm); }
            }
        }
    }

    SECTION("comparisons") {
        SECTION("Empty == Empty") {
            REQUIRE(sms.at("Empty") == pimpl_t{});
            REQUIRE_FALSE(sms.at("Empty") != pimpl_t{});
        }

        SECTION("Empty != non-empty") {
            auto& lhs = sms.at("Empty");
            for(std::size_t i = 0; i < 3; ++i) {
                std::string key = "Ind == rank " + std::to_string(i);
                auto& rhs       = sms.at(key);
                SECTION(key) {
                    REQUIRE_FALSE(lhs == rhs);
                    REQUIRE(lhs != rhs);
                }
            }
        }

        SECTION("Same non-empty") {
            auto& lhs = sms.at("Ind == rank 0");
            auto copy = lhs.clone();
            REQUIRE(lhs == *copy);
            REQUIRE_FALSE(lhs != *copy);
        }

        SECTION("Domain is subset/superset") {
            auto& lhs = sms.at("Ind == rank 0");
            auto copy = lhs.clone();
            copy->add_to_domain(i0, dep_idx_t{3});
            REQUIRE_FALSE(lhs == *copy);
            REQUIRE(lhs != *copy);
        }

        SECTION("Different independent indices") {
            auto& lhs = sms.at("Ind == rank 1");
            auto copy = lhs.clone();
            copy->add_to_domain(ind_idx_t{3}, dep_idx_t{3});
            REQUIRE_FALSE(lhs == *copy);
            REQUIRE(lhs != *copy);
        }
    }

    SECTION("print") {
        std::stringstream ss;

        SECTION("empty") {
            auto pss = &(sms.at("Empty").print(ss));
            SECTION("Value") { REQUIRE(ss.str() == "{}"); }
            SECTION("Returns ostream") { REQUIRE(pss == &ss); }
        }

        SECTION("Non-empty") {
            auto pss = &(sms.at("Ind == rank 0").print(ss));
            SECTION("Value") {
                std::string corr = "{({} : {{1}, {2}})}";
                REQUIRE(ss.str() == corr);
            }
            SECTION("Returns ostream") { REQUIRE(pss == &ss); }
        }
    }

    SECTION("hash") {
        SECTION("Empty == Empty") {
            auto h  = sde::hash_objects(sms.at("Empty"));
            auto h2 = sde::hash_objects(pimpl_t{});
            REQUIRE(h == h2);
        }

        SECTION("Empty != non-empty") {
            auto h = sde::hash_objects(sms.at("Empty"));
            for(std::size_t i = 0; i < 3; ++i) {
                std::string key = "Ind == rank " + std::to_string(i);
                auto& rhs       = sms.at(key);
                SECTION(key) {
                    auto h2 = sde::hash_objects(rhs);
                    REQUIRE(h != h2);
                }
            }
        }

        SECTION("Same non-empty") {
            auto& lhs = sms.at("Ind == rank 0");
            auto h    = sde::hash_objects(lhs);
            auto h2   = sde::hash_objects(*(lhs.clone()));
            REQUIRE(h == h2);
        }

        SECTION("Domain is subset/superset") {
            auto& lhs = sms.at("Ind == rank 0");
            auto h    = sde::hash_objects(lhs);
            auto copy = lhs.clone();
            copy->add_to_domain(i0, dep_idx_t{3});
            auto h2 = sde::hash_objects(*copy);
            REQUIRE(h != h2);
        }

        SECTION("Different independent indices") {
            auto& lhs = sms.at("Ind == rank 1");
            auto h    = sde::hash_objects(lhs);
            auto copy = lhs.clone();
            copy->add_to_domain(ind_idx_t{3}, dep_idx_t{3});
            auto h2 = sde::hash_objects(copy);
            REQUIRE(h != h2);
        }
    }
}

/* operator<< just calls SparseMap::print. So as long as that works, this will
 *  work too. We just test an empty SparseMap to make sure it gets forwarded
 *  correctly and the ostream is returend.
 */
TEST_CASE("operator<<(std::ostream, SparseMapPIMPL)") {
    std::stringstream ss;
    SparseMapPIMPL<TileIndex, TileIndex> sm;
    auto pss = &(ss << sm);
    REQUIRE(pss == &ss);
    REQUIRE(ss.str() == "{}");
}
