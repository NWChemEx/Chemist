#include "libchemist/sparse_map/sparse_map/sparse_map.hpp"
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

TEMPLATE_LIST_TEST_CASE("SparseMapBase", "", index_list) {
    using ind_idx_t = std::tuple_element_t<0, TestType>;
    using dep_idx_t = std::tuple_element_t<1, TestType>;
    using derived_t = SparseMap<ind_idx_t, dep_idx_t>;
    using base_t    = SparseMapBase<derived_t, ind_idx_t, dep_idx_t>;
    using domain_t  = Domain<dep_idx_t>;

    ind_idx_t i0{}, i1{1}, i12{2}, i2{1, 2}, i22{2, 3};
    dep_idx_t d0{}, d1{1}, d12{2}, d2{1, 2}, d22{2, 3};

    std::map<std::string, derived_t> sms;
    sms["Empty"];
    sms["Ind == rank 0"] = derived_t{{i0, {d1, d12}}};
    sms["Ind == rank 1"] = derived_t{{i12, {}}, {i1, {d1}}};
    sms["Ind == rank 2"] = derived_t{{i2, {d2}}, {i22, {d22}}};
    derived_t temp(std::move(sms["No PIMPL"]));

    SECTION("CTors") {
        SECTION("Typedefs") {
            using traits = SparseMapTraits<derived_t>;

            SECTION("size_type") {
                using corr_t = typename traits::size_type;
                using the_t  = typename base_t::size_type;
                STATIC_REQUIRE(std::is_same_v<corr_t, the_t>);
            }

            SECTION("key_type") {
                using corr_t = typename traits::key_type;
                using the_t  = typename base_t::key_type;
                STATIC_REQUIRE(std::is_same_v<corr_t, the_t>);
            }

            SECTION("mapped_type") {
                using corr_t = typename traits::mapped_type;
                using the_t  = typename base_t::mapped_type;
                STATIC_REQUIRE(std::is_same_v<corr_t, the_t>);
            }

            SECTION("const_iterator") {
                using corr_t =
                  typename traits::template const_iterator<derived_t>;
                using the_t = typename base_t::const_iterator;
                STATIC_REQUIRE(std::is_same_v<corr_t, the_t>);
            }
        }

        SECTION("Default Ctor") {
            auto& sm = sms.at("Empty");
            REQUIRE(sm.size() == 0);
            REQUIRE(sm.empty());
            REQUIRE(sm.ind_rank() == 0);
            REQUIRE(sm.dep_rank() == 0);
        }

        SECTION("Initializer list") {
            SECTION("Empty") {
                base_t sm_empty({});
                REQUIRE(sm_empty == sms.at("Empty"));
            }

            SECTION("Ind == rank 0") {
                auto& sm0 = sms.at("Ind == rank 0");
                REQUIRE(sm0.size() == 1);
                REQUIRE_FALSE(sm0.empty());
                REQUIRE(sm0.ind_rank() == 0);
                REQUIRE(sm0.dep_rank() == 1);
            }

            SECTION("Ind == rank 1") {
                auto& sm1 = sms.at("Ind == rank 1");
                REQUIRE(sm1.size() == 1);
                REQUIRE_FALSE(sm1.empty());
                REQUIRE(sm1.ind_rank() == 1);
                REQUIRE(sm1.dep_rank() == 1);
            }

            SECTION("Ind == rank 2") {
                auto& sm2 = sms.at("Ind == rank 2");
                REQUIRE(sm2.size() == 2);
                REQUIRE_FALSE(sm2.empty());
                REQUIRE(sm2.ind_rank() == 2);
                REQUIRE(sm2.dep_rank() == 2);
            }
        }

        SECTION("Copy ctor") {
            for(auto [k, v] : sms) {
                SECTION(k) {
                    base_t copy(v);
                    REQUIRE(copy == v);
                }
            }
        }

        SECTION("Move ctor") {
            for(auto [k, v] : sms) {
                SECTION(k) {
                    base_t corr(v);
                    base_t moved2(std::move(v));
                    REQUIRE(moved2 == corr);
                }
            }
        }

        SECTION("Copy assignment") {
            for(auto [k, v] : sms) {
                SECTION(k) {
                    base_t copy;
                    auto pcopy = &(copy = v);
                    SECTION("Value") { REQUIRE(copy == v); }
                    SECTION("Returns *this") { REQUIRE(pcopy == &copy); }
                }
            }
        }

        SECTION("Move ctor") {
            for(auto [k, v] : sms) {
                SECTION(k) {
                    base_t corr(v);
                    base_t moved2;
                    auto pmoved = &(moved2 = std::move(v));
                    SECTION("Value") { REQUIRE(moved2 == corr); }
                    SECTION("Returns *this") { REQUIRE(pmoved == &moved2); }
                }
            }
        }

    } // SECTION("CTORS")

    SECTION("swap") {
        for(auto& [lhs_k, lhs_v] : sms) {
            for(auto& [rhs_k, rhs_v] : sms) {
                SECTION(lhs_k + " swapped with " + rhs_k) {
                    base_t corr_lhs(rhs_v);
                    base_t corr_rhs(lhs_v);
                    lhs_v.swap(rhs_v);
                    REQUIRE(lhs_v == corr_lhs);
                    REQUIRE(rhs_v == corr_rhs);
                }
            }
        }
    }

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

        SECTION("No PIMPL") {
            auto& mf = sms.at("No PIMPL");
            REQUIRE(mf.size() == 0);
        }
    }

    SECTION("empty") {
        for(auto [k, v] : sms) {
            SECTION(k) {
                bool is_empty = (v.size() == 0);
                REQUIRE(v.empty() == is_empty);
            }
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

        SECTION("No PIMPL") { REQUIRE_FALSE(sms.at("No PIMPL").count(i0)); }
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

        SECTION("No PIMPL") {
            auto& mf = sms.at("No PIMPL");
            REQUIRE(mf.ind_rank() == 0);
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

        SECTION("No PIMPL") {
            auto& mf = sms.at("No PIMPL");
            REQUIRE(mf.dep_rank() == 0);
        }
    }

    SECTION("add_to_domain") {
        SECTION("Empty") {
            auto& sm = sms.at("Empty");
            sm.add_to_domain(i0, d0);
            base_t corr{{i0, {d0}}};
            REQUIRE(sm == corr);
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
                base_t corr{{i0, {d1, d12, dep_idx_t{3}}}};
                REQUIRE(sm0 == corr);
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
                sm1.add_to_domain(i12, d12);
                base_t corr{{i1, {d1}}, {i12, {d12}}};
                REQUIRE(sm1 == corr);
            }
            SECTION("Add to non-existing independent index") {
                sm1.add_to_domain(ind_idx_t{4}, d12);
                base_t corr{{i12, {}}, {i1, {d1}}, {ind_idx_t{4}, {d12}}};
                REQUIRE(sm1 == corr);
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
                base_t corr{{i2, {d2, dep_idx_t{3, 4}}}, {i22, {d22}}};
                REQUIRE(sm2 == corr);
            }
            SECTION("Add to non-existing independent index") {
                sm2.add_to_domain(ind_idx_t{3, 4}, d2);
                base_t corr{{i2, {d2}}, {i22, {d22}}, {ind_idx_t{3, 4}, {d2}}};
                REQUIRE(sm2 == corr);
            }
        }

        SECTION("No PIMPL") {
            auto& mf = sms.at("No PIMPL");
            mf.add_to_domain(i0, d0);
            REQUIRE(mf == base_t{{i0, {d0}}});
        }
    }

    SECTION("operator[] const") {
        SECTION("Empty") {
            const auto& sm = sms.at("Empty");
            REQUIRE_THROWS_AS(sm[i0], std::out_of_range);
        }

        SECTION("Ind == rank 0") {
            const auto& sm = sms.at("Ind == rank 0");
            SECTION("Throws if wrong ind rank") {
                REQUIRE_THROWS_AS(sm[i1], std::runtime_error);
            }
            SECTION("Value") { REQUIRE(sm[i0] == domain_t{d1, d12}); }
        }

        SECTION("Ind == rank 1") {
            const auto& sm = sms.at("Ind == rank 1");
            SECTION("Throws if wrong ind rank") {
                REQUIRE_THROWS_AS(sm[i0], std::runtime_error);
            }
            SECTION("Throws if value is not present") {
                REQUIRE_THROWS_AS(sm[ind_idx_t{4}], std::out_of_range);
            }
            SECTION("Value") { REQUIRE(sm[i1] == domain_t{d1}); }
        }

        SECTION("Ind == rank 2") {
            const auto& sm = sms.at("Ind == rank 2");
            SECTION("Throws if wrong ind rank") {
                REQUIRE_THROWS_AS(sm[i1], std::runtime_error);
            }
            SECTION("Throws if value is not present") {
                ind_idx_t i23{3, 4};
                REQUIRE_THROWS_AS(sm[i23], std::out_of_range);
            }
            SECTION("Value") { REQUIRE(sm[i2] == domain_t{d2}); }
        }

        SECTION("No PIMPL") {
            const auto& sm = sms.at("No PIMPL");
            REQUIRE_THROWS_AS(sm[i0], std::out_of_range);
        }
    } // SECTION("operator[]const")

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

        SECTION("No PIMPL") {
            const auto& sm = sms.at("No PIMPL");
            REQUIRE_THROWS_AS(sm.at(i0), std::out_of_range);
        }
    } // SECTION("operator[]const")

    SECTION("direct_product") {
        SECTION("LHS == Empty") {
            auto& lhs = sms.at("Empty");
            derived_t corr(lhs);

            for(auto [key, rhs] : sms) {
                SECTION("RHS == " + key) {
                    auto result = lhs.direct_product(rhs);
                    REQUIRE(result == corr);
                }
            }
        }

        SECTION("LHS == rank 0") {
            auto& lhs = sms.at("Ind == rank 0");

            SECTION("RHS == Empty") {
                auto& rhs   = sms.at("Empty");
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == rhs);
            }

            SECTION("RHS == Ind == rank 0") {
                auto& rhs = sms.at("Ind == rank 0");
                derived_t corr{{i0,
                                {dep_idx_t{1, 1}, dep_idx_t{1, 2},
                                 dep_idx_t{2, 1}, dep_idx_t{2, 2}}}};
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == corr);
            }

            SECTION("RHS == Ind == rank 1") {
                auto& rhs = sms.at("Ind == rank 1");
                derived_t corr{{i1, {dep_idx_t{1, 1}, dep_idx_t{2, 1}}}};
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == corr);
            }

            SECTION("RHS == Ind == rank 2") {
                auto& rhs = sms.at("Ind == rank 2");
                derived_t corr{{i2, {dep_idx_t{1, 1, 2}, dep_idx_t{2, 1, 2}}},
                               {i22, {dep_idx_t{1, 2, 3}, dep_idx_t{2, 2, 3}}}};
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == corr);
            }

            SECTION("RHS == No PIMPL") {
                auto& rhs   = sms.at("No PIMPL");
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == rhs);
            }
        }

        SECTION("LHS == rank 1") {
            auto& lhs = sms.at("Ind == rank 1");

            SECTION("RHS == empty") {
                auto& rhs   = sms.at("Empty");
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == rhs);
            }

            SECTION("RHS == Ind == rank 0") {
                auto& rhs = sms.at("Ind == rank 0");
                derived_t corr{{i1, {dep_idx_t{1, 1}, dep_idx_t{1, 2}}}};
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == corr);
            }

            SECTION("RHS == Ind == rank 1") {
                auto& rhs = sms.at("Ind == rank 1");
                derived_t corr{{ind_idx_t{1, 1}, {dep_idx_t{1, 1}}},
                               {ind_idx_t{2, 1}, {}},
                               {ind_idx_t{2, 2}, {}}};
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == corr);
            }

            SECTION("RHS == Ind == rank 2") {
                auto& rhs = sms.at("Ind == rank 2");
                derived_t corr{{ind_idx_t{1, 1, 2}, {dep_idx_t{1, 1, 2}}},
                               {ind_idx_t{1, 2, 3}, {dep_idx_t{1, 2, 3}}},
                               {ind_idx_t{2, 1, 2}, {}},
                               {ind_idx_t{2, 2, 3}, {}}};
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == corr);
            }

            SECTION("RHS == No PIMPL") {
                auto& rhs   = sms.at("No PIMPL");
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == rhs);
            }
        }

        SECTION("LHS == rank 2") {
            auto& lhs = sms.at("Ind == rank 2");

            SECTION("RHS == empty") {
                auto& rhs   = sms.at("Empty");
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == rhs);
            }

            SECTION("RHS == Ind == rank 0") {
                auto& rhs = sms.at("Ind == rank 0");
                derived_t corr{{i2, {dep_idx_t{1, 2, 1}, dep_idx_t{1, 2, 2}}},
                               {i22, {dep_idx_t{2, 3, 1}, dep_idx_t{2, 3, 2}}}};
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == corr);
            }

            SECTION("RHS == Ind == rank 1") {
                auto& rhs = sms.at("Ind == rank 1");
                derived_t corr{{ind_idx_t{1, 2, 1}, {dep_idx_t{1, 2, 1}}},
                               {ind_idx_t{1, 2, 2}, {}},
                               {ind_idx_t{2, 3, 1}, {dep_idx_t{2, 3, 1}}},
                               {ind_idx_t{2, 3, 2}, {}}};
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == corr);
            }

            SECTION("RHS == Ind == rank 2") {
                auto& rhs = sms.at("Ind == rank 2");
                derived_t corr{
                  {ind_idx_t{1, 2, 1, 2}, {dep_idx_t{1, 2, 1, 2}}},
                  {ind_idx_t{1, 2, 2, 3}, {dep_idx_t{1, 2, 2, 3}}},
                  {ind_idx_t{2, 3, 1, 2}, {dep_idx_t{2, 3, 1, 2}}},
                  {ind_idx_t{2, 3, 2, 3}, {dep_idx_t{2, 3, 2, 3}}}};
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == corr);
            }

            SECTION("RHS == No PIMPL") {
                auto& rhs   = sms.at("No PIMPL");
                auto result = lhs.direct_product(rhs);
                REQUIRE(result == rhs);
            }
        }

        SECTION("LHS == No PIMPL") {
            auto& lhs = sms.at("No PIMPL");
            derived_t corr(lhs);

            for(auto [key, rhs] : sms) {
                SECTION("RHS == " + key) {
                    auto result = lhs.direct_product(rhs);
                    REQUIRE(result == corr);
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
                derived_t rhs;

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
                derived_t rhs{{ind_idx_t{1}, {dep_idx_t{2}}}};

                SECTION("lhs *= rhs") {
                    auto plhs = &(lhs *= rhs);
                    SECTION("Value") { REQUIRE(lhs == derived_t{}); }
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
            derived_t lhs{{ind_idx_t{1}, {dep_idx_t{1}}}};

            SECTION("RHS same independent, single element domain") {
                derived_t rhs{{ind_idx_t{1}, {dep_idx_t{2}}}};

                SECTION("lhs *= rhs") {
                    derived_t corr{{ind_idx_t{1}, {dep_idx_t{1, 2}}}};
                    auto plhs = &(lhs *= rhs);
                    SECTION("Value") { REQUIRE(lhs == corr); }
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                }

                SECTION("rhs *= lhs") {
                    derived_t corr{{ind_idx_t{1}, {dep_idx_t{2, 1}}}};
                    auto prhs = &(rhs *= lhs);
                    SECTION("Value") { REQUIRE(rhs == corr); }
                    SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                }
            }

            SECTION("RHS same independent, two element domain") {
                derived_t rhs{{ind_idx_t{1}, {dep_idx_t{2}, dep_idx_t{3}}}};

                SECTION("lhs *= rhs") {
                    derived_t corr{
                      {ind_idx_t{1}, {dep_idx_t{1, 2}, dep_idx_t{1, 3}}}};
                    auto plhs = &(lhs *= rhs);
                    SECTION("Value") { REQUIRE(lhs == corr); }
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                }

                SECTION("rhs *= lhs") {
                    derived_t corr{
                      {ind_idx_t{1}, {dep_idx_t{2, 1}, dep_idx_t{3, 1}}}};
                    auto prhs = &(rhs *= lhs);
                    SECTION("Value") { REQUIRE(rhs == corr); }
                    SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                }
            }

            SECTION("RHS different independent, single element domain") {
                derived_t rhs{{ind_idx_t{2}, {dep_idx_t{2}}}};

                SECTION("lhs *= rhs") {
                    auto plhs = &(lhs *= rhs);
                    SECTION("Value") { REQUIRE(lhs == derived_t{}); }
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                }

                SECTION("rhs *= lhs") {
                    auto prhs = &(rhs *= lhs);
                    SECTION("Value") { REQUIRE(rhs == derived_t{}); }
                    SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                }
            }

            SECTION("RHS multiple independent") {
                derived_t rhs{{ind_idx_t{1}, {dep_idx_t{2}}},
                              {ind_idx_t{2}, {dep_idx_t{2}}}};

                SECTION("lhs * rhs") {
                    auto plhs = &(lhs *= rhs);
                    derived_t corr{{ind_idx_t{1}, {dep_idx_t{1, 2}}}};
                    SECTION("Value") { REQUIRE(lhs == corr); }
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                }

                SECTION("rhs * lhs") {
                    auto prhs = &(rhs *= lhs);
                    derived_t corr{{ind_idx_t{1}, {dep_idx_t{2, 1}}}};
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

    SECTION("operator*") {
        auto& lhs = sms.at("Ind == rank 2");
        auto& rhs = sms.at("Ind == rank 2");
        derived_t corr{{ind_idx_t{1, 2}, {dep_idx_t{1, 2, 1, 2}}},
                       {ind_idx_t{2, 3}, {dep_idx_t{2, 3, 2, 3}}}};
        auto r = lhs * rhs;
        REQUIRE(r == corr);
    }

    /* With respect to union operator+= does all the work and operator+
     * simply calls operator+= on a copy. Therefore we test operator+= in depth
     * and make sure operator+ works for one scenario.
     */
    SECTION("operator+=") {
        SECTION("Empty / Empty") {
            derived_t sm, sm2;
            auto psm = &(sm += sm2);
            SECTION("Value") { REQUIRE(sm == sm2); }
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
        }

        SECTION("Empty / Non-empty") {
            derived_t sm;
            derived_t sm2{{ind_idx_t{1}, {dep_idx_t{0}, dep_idx_t{3}}},
                          {ind_idx_t{2}, {dep_idx_t{1}, dep_idx_t{2}}}};
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
            derived_t sm{{ind_idx_t{1}, {dep_idx_t{0}, dep_idx_t{3}}},
                         {ind_idx_t{2}, {dep_idx_t{1}, dep_idx_t{2}}}};

            SECTION("Compatible") {
                derived_t sm2{{ind_idx_t{0}, {dep_idx_t{0}, dep_idx_t{3}}},
                              {ind_idx_t{1}, {dep_idx_t{1}, dep_idx_t{2}}},
                              {ind_idx_t{2}, {dep_idx_t{1}, dep_idx_t{2}}},
                              {ind_idx_t{3}, {dep_idx_t{1}, dep_idx_t{2}}}};
                derived_t corr{
                  {ind_idx_t{0}, {dep_idx_t{0}, dep_idx_t{3}}},
                  {ind_idx_t{1},
                   {dep_idx_t{0}, dep_idx_t{1}, dep_idx_t{2}, dep_idx_t{3}}},
                  {ind_idx_t{2}, {dep_idx_t{1}, dep_idx_t{2}}},
                  {ind_idx_t{3}, {dep_idx_t{1}, dep_idx_t{2}}}};
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
                derived_t incompatible{
                  {ind_idx_t{1, 2}, {dep_idx_t{0}, dep_idx_t{3}}},
                  {ind_idx_t{2, 3}, {dep_idx_t{1}, dep_idx_t{2}}}};
                REQUIRE_THROWS_AS(sm += incompatible, std::runtime_error);
            }

            SECTION("Incompatible dependent indices") {
                derived_t incompatible{
                  {ind_idx_t{1}, {dep_idx_t{0, 1}, dep_idx_t{3, 4}}},
                  {ind_idx_t{2}, {dep_idx_t{1, 2}, dep_idx_t{2, 3}}}};
                REQUIRE_THROWS_AS(sm += incompatible, std::runtime_error);
            }
        }
    }

    SECTION("operator+") {
        auto& lhs = sms.at("Empty");
        auto& rhs = sms.at("Ind == rank 0");
        auto r    = lhs + rhs;
        REQUIRE(r == rhs);
    }

    SECTION("operator^=") {
        SECTION("Empty / Empty") {
            derived_t sm;
            auto psm = &(sm ^= sm);
            SECTION("Value") { REQUIRE(sm == derived_t{}); }
            SECTION("Returns *this") { REQUIRE(psm == &sm); }
        }

        SECTION("Empty / Non-empty") {
            derived_t sm;
            derived_t sm2{{ind_idx_t{1}, {dep_idx_t{0}, dep_idx_t{3}}},
                          {ind_idx_t{2}, {dep_idx_t{1}, dep_idx_t{2}}}};

            SECTION("sm ^= sm2") {
                auto psm = &(sm ^= sm2);
                SECTION("Value") { REQUIRE(sm == derived_t{}); }
                SECTION("Returns *this") { REQUIRE(psm == &sm); }
            }

            SECTION("sm2 ^= sm") {
                auto psm2 = &(sm2 ^= sm);
                SECTION("Value") { REQUIRE(sm2 == derived_t{}); }
                SECTION("Returns *this") { REQUIRE(psm2 == &sm2); }
            }
        }

        SECTION("Non-empty / Non-empty") {
            derived_t sm{{ind_idx_t{1}, {dep_idx_t{0}, dep_idx_t{3}}},
                         {ind_idx_t{2}, {dep_idx_t{1}, dep_idx_t{2}}}};
            derived_t sm2{{ind_idx_t{0}, {dep_idx_t{0}, dep_idx_t{3}}},
                          {ind_idx_t{1}, {dep_idx_t{1}, dep_idx_t{2}}},
                          {ind_idx_t{2}, {dep_idx_t{1}, dep_idx_t{2}}},
                          {ind_idx_t{3}, {dep_idx_t{1}, dep_idx_t{2}}}};

            derived_t corr{{ind_idx_t{2}, {dep_idx_t{1}, dep_idx_t{2}}}};

            SECTION("sm ^= sm2") {
                auto psm = &(sm ^= sm2);
                SECTION("Value") { REQUIRE(sm == corr); }
                SECTION("Returns *this") { REQUIRE(psm == &sm); }
            }

            SECTION("sm2 ^= sm") {
                auto psm2 = &(sm2 ^= sm);
                SECTION("Value") { REQUIRE(sm2 == corr); }
                SECTION("Returns *this") { REQUIRE(psm2 == &sm2); }
            }

            SECTION("sm ^= corr") {
                auto psm = &(sm ^= corr);
                SECTION("Value") { REQUIRE(sm == corr); }
                SECTION("Returns *this") { REQUIRE(psm == &sm); }
            }

            SECTION("different ranks") {
                auto psm = &(sm ^= sms.at("Ind == rank 2"));
                SECTION("Value") { REQUIRE(sm == derived_t{}); }
                SECTION("Returns *this") { REQUIRE(psm == &sm); }
            }
        }
    }

    SECTION("operator^") {
        auto& sm1 = sms.at("Ind == rank 1");
        auto r    = sm1 ^ sm1;
        REQUIRE(r == sm1);
    }

    SECTION("inverse") {
        SECTION("Empty") {
            derived_t sm;
            SparseMap<dep_idx_t, ind_idx_t> corr;
            REQUIRE(sm.inverse() == corr);
        }

        SECTION("Non-empty") {
            derived_t sm{{ind_idx_t{1}, {dep_idx_t{0}, dep_idx_t{3}}},
                         {ind_idx_t{2}, {dep_idx_t{1}, dep_idx_t{2}}}};
            SparseMap<dep_idx_t, ind_idx_t> corr{
              {dep_idx_t{0}, {ind_idx_t{1}}},
              {dep_idx_t{3}, {ind_idx_t{1}}},
              {dep_idx_t{1}, {ind_idx_t{2}}},
              {dep_idx_t{2}, {ind_idx_t{2}}}};
            REQUIRE(sm.inverse() == corr);
            REQUIRE(sm.inverse().inverse() == sm);
        }
    }

    SECTION("chain") {
        using new_idx_t = ElementIndex;
        using rhs_t     = SparseMap<dep_idx_t, new_idx_t>;
        using result_t  = SparseMap<ind_idx_t, new_idx_t>;

        derived_t lsm1{{ind_idx_t{1}, {dep_idx_t{0}, dep_idx_t{3}}},
                       {ind_idx_t{2}, {dep_idx_t{1}, dep_idx_t{2}}}};
        rhs_t rsm1{{dep_idx_t{1}, {new_idx_t{0}, new_idx_t{3}}},
                   {dep_idx_t{2}, {new_idx_t{1}, new_idx_t{2}}}};

        SECTION("Empty / Empty") {
            derived_t sm;
            rhs_t rhs;
            result_t corr;
            REQUIRE(sm.chain(rhs) == corr);
        }

        SECTION("Empty / Non-empty") {
            derived_t sm;
            REQUIRE_THROWS_AS(sm.chain(rsm1), std::runtime_error);
        }

        SECTION("Non-empty / Non-empty") {
            rhs_t rsm2{{dep_idx_t{0}, {new_idx_t{0}, new_idx_t{3}}},
                       {dep_idx_t{1}, {new_idx_t{1}, new_idx_t{2}}},
                       {dep_idx_t{2}, {new_idx_t{1}, new_idx_t{2}}},
                       {dep_idx_t{3}, {new_idx_t{1}, new_idx_t{2}}}};
            result_t corr{
              {ind_idx_t{1},
               {new_idx_t{0}, new_idx_t{1}, new_idx_t{2}, new_idx_t{3}}},
              {ind_idx_t{2}, {new_idx_t{1}, new_idx_t{2}}}};
            REQUIRE(lsm1.chain(rsm2) == corr);
        }

        SECTION("Non-empty / incompatible") {
            rhs_t incompatible{{dep_idx_t{1, 2}, {new_idx_t{0}, new_idx_t{3}}},
                               {dep_idx_t{2, 3}, {new_idx_t{1}, new_idx_t{2}}}};
            REQUIRE_THROWS_AS(lsm1.chain(incompatible), std::runtime_error);
        }
    }

    SECTION("comparisons") {
        SECTION("Empty == Empty") {
            REQUIRE(sms.at("Empty") == derived_t{});
            REQUIRE_FALSE(sms.at("Empty") != derived_t{});
        }

        SECTION("Empty == No PIMPL") {
            REQUIRE(sms.at("Empty") == sms.at("No PIMPL"));
            REQUIRE_FALSE(sms.at("Empty") != sms.at("No PIMPL"));
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
            derived_t copy(lhs);
            REQUIRE(lhs == copy);
            REQUIRE_FALSE(lhs != copy);
        }

        SECTION("Domain is subset/superset") {
            auto& lhs = sms.at("Ind == rank 0");
            derived_t copy(lhs);
            copy.add_to_domain(i0, dep_idx_t{3});
            REQUIRE_FALSE(lhs == copy);
            REQUIRE(lhs != copy);
        }

        SECTION("Different independent indices") {
            auto& lhs = sms.at("Ind == rank 1");
            derived_t copy(lhs);
            copy.add_to_domain(ind_idx_t{3}, dep_idx_t{3});
            REQUIRE_FALSE(lhs == copy);
            REQUIRE(lhs != copy);
        }

        SECTION("No PIMPL != non-empty") {
            auto& lhs = sms.at("No PIMPL");
            for(std::size_t i = 0; i < 3; ++i) {
                std::string key = "Ind == rank " + std::to_string(i);
                auto& rhs       = sms.at(key);
                SECTION(key) {
                    REQUIRE_FALSE(lhs == rhs);
                    REQUIRE(lhs != rhs);
                }
            }
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
            auto h  = pluginplay::hash_objects(sms.at("Empty"));
            auto h2 = pluginplay::hash_objects(derived_t{});
            REQUIRE(h == h2);
        }

        SECTION("Empty == No PIMPL") {
            auto h  = pluginplay::hash_objects(sms.at("Empty"));
            auto h2 = pluginplay::hash_objects(sms.at("No PIMPL"));
            REQUIRE(h == h2);
        }

        SECTION("Empty != non-empty") {
            auto h = pluginplay::hash_objects(sms.at("Empty"));
            for(std::size_t i = 0; i < 3; ++i) {
                std::string key = "Ind == rank " + std::to_string(i);
                auto& rhs       = sms.at(key);
                SECTION(key) {
                    auto h2 = pluginplay::hash_objects(rhs);
                    REQUIRE(h != h2);
                }
            }
        }

        SECTION("Same non-empty") {
            auto& lhs = sms.at("Ind == rank 0");
            auto h    = pluginplay::hash_objects(lhs);
            auto h2   = pluginplay::hash_objects(derived_t(lhs));
            REQUIRE(h == h2);
        }

        SECTION("Domain is subset/superset") {
            auto& lhs = sms.at("Ind == rank 0");
            auto h    = pluginplay::hash_objects(lhs);
            derived_t copy(lhs);
            copy.add_to_domain(i0, dep_idx_t{3});
            auto h2 = pluginplay::hash_objects(copy);
            REQUIRE(h != h2);
        }

        SECTION("Different independent indices") {
            auto& lhs = sms.at("Ind == rank 1");
            auto h    = pluginplay::hash_objects(lhs);
            derived_t copy(lhs);
            copy.add_to_domain(ind_idx_t{3}, dep_idx_t{3});
            auto h2 = pluginplay::hash_objects(copy);
            REQUIRE(h != h2);
        }

        SECTION("No PIMPL != non-empty") {
            auto& lhs = sms.at("No PIMPL");
            auto h    = pluginplay::hash_objects(lhs);
            for(std::size_t i = 0; i < 3; ++i) {
                std::string key = "Ind == rank " + std::to_string(i);
                auto& rhs       = sms.at(key);
                auto h2         = pluginplay::hash_objects(rhs);
                SECTION(key) { REQUIRE(h != h2); }
            }
        }
    }
}

/* operator<< just calls SparseMap::print. So as long as that works, this will
 *  work too. We just test an empty SparseMap to make sure it gets forwarded
 *  correctly and the ostream is returend.
 */
TEST_CASE("operator<<(std::ostream, SparseMapBase)") {
    std::stringstream ss;
    SparseMap<TileIndex, TileIndex> sm;
    auto pss = &(ss << sm);
    REQUIRE(pss == &ss);
    REQUIRE(ss.str() == "{}");
}
