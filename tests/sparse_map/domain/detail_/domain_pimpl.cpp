#include "libchemist/sparse_map/domain/detail_/domain_pimpl.hpp"
#include "libchemist/sparse_map/index.hpp"
#include <catch2/catch.hpp>
#include <tuple>

using namespace libchemist::sparse_map;
using index_types = std::tuple<ElementIndex, TileIndex>;

/* Testing notes:
 *
 * - The DomainPIMPL class contains the internal m_mode_map_ member which needs
 *   to be updated any time the contents of the Domain change. This member is an
 *   implementation detail affecting result_extents/result_index and if it's not
 *   updated correctly those functions will break. Thus all unit tests which
 *   change the state of the DomainPIMPL should test that they also correctly
 *   update result_extents/result_index
 *
 */

TEMPLATE_LIST_TEST_CASE("DomainPIMPL", "", index_types) {
    using namespace libchemist::sparse_map::detail_;
    using pimpl_type = DomainPIMPL<TestType>;

    TestType e, e0{0}, e00{0, 0}, e01{0, 1}, e1{1}, e10{1, 0}, e11{1, 1};
    TestType e12{1, 2}, e2{2}, e21{2, 1}, e22{2, 2}, e23{2, 3};
    
    std::map<std::string, pimpl_type> ps;
    ps["empty"];

    ps["1 rank 0 index"].insert(e);

    ps["1 rank 1 index"].insert(e1);

    ps["2 rank 1 indices"].insert(e1);
    ps["2 rank 1 indices"].insert(e2);

    ps["1 rank 2 index"].insert(e12);

    ps["2 rank 2 indices"].insert(e12);
    ps["2 rank 2 indices"].insert(e23);

    SECTION("Typedefs"){
        using traits =
          libchemist::sparse_map::detail_::DomainTraits<Domain<TestType>>;

        SECTION("size_type"){
            using pimpl_t = typename pimpl_type::size_type;
            using corr_t  = typename traits::size_type;
            STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
        }

        SECTION("value_type"){
            using pimpl_t = typename pimpl_type::value_type;
            using corr_t  = typename traits::value_type;
            STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
        }

        SECTION("const_reference"){
            using pimpl_t = typename pimpl_type::const_reference;
            using corr_t  = typename traits::const_reference;
            STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
        }
        
        SECTION("extents_type"){
            using pimpl_t = typename pimpl_type::extents_type;
            using corr_t  = typename traits::extents_type;
            STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
        }
        
    } // typedefs

    SECTION("Ctors") {
        SECTION("Default Ctor") {
            const auto& p0 = ps.at("empty");
            SECTION("rank") { REQUIRE(p0.rank() == 0); }
            SECTION("size") { REQUIRE(p0.size() == 0); }
        }

        SECTION("clone()"){
            for(const auto& [k, v] : ps){
                SECTION(k) {
                    auto clone = v.clone();
                    REQUIRE(*clone == v);
                }
            }
        }
    } // SECTION("Ctors")

    SECTION("count()") {
        SECTION("Empty"){
            const auto& p0 = ps.at("empty");
            REQUIRE_FALSE(p0.count(e));
        }

        SECTION("1 rank 0 index"){
            const auto& p0 = ps.at("1 rank 0 index");
            SECTION("Has value"){ REQUIRE(p0.count(e)); }
            SECTION("Does not have value"){
                REQUIRE_FALSE(p0.count(e1));
            }
        }

        SECTION("1 rank 1 index"){
            const auto& p1 = ps.at("1 rank 1 index");
            SECTION("Has value"){REQUIRE(p1.count(e1)); }
            SECTION("Does not have value"){
                SECTION("Same rank") { REQUIRE_FALSE(p1.count(e2)); }
                SECTION("Other rank"){ REQUIRE_FALSE(p1.count(e)); }
            }
        }

        SECTION("2 rank 1 indices"){
            const auto& p1 = ps.at("2 rank 1 indices");
            SECTION("Has value"){
                REQUIRE(p1.count(e1));
                REQUIRE(p1.count(e2));
            }
            SECTION("Does not have value"){
                SECTION("Same rank") { REQUIRE_FALSE(p1.count(TestType{3})); }
                SECTION("Other rank"){ REQUIRE_FALSE(p1.count(e)); }
            }
        }

        SECTION("1 rank 2 index"){
            const auto& p2 = ps.at("1 rank 2 index");
            SECTION("Has value"){ REQUIRE(p2.count(e12)); }
            SECTION("Does not have value"){
                SECTION("Same rank") {
                    REQUIRE_FALSE(p2.count(e23));
                }
                SECTION("Other rank"){ REQUIRE_FALSE(p2.count(e)); }
            }
        }

        SECTION("2 rank 2 indices"){
            const auto& p2 = ps.at("2 rank 2 indices");
            SECTION("Has value"){
                REQUIRE(p2.count(e12));
                REQUIRE(p2.count(e23));
            }
            SECTION("Does not have value"){
                SECTION("Same rank") {
                    REQUIRE_FALSE(p2.count(TestType{1, 3}));
                }
                SECTION("Other rank"){ REQUIRE_FALSE(p2.count(e)); }
            }
        }
    }

    SECTION("rank()") {
        SECTION("Empty"){
            const auto& p0 = ps.at("empty");
            REQUIRE(p0.rank() == 0);
        }

        for(std::size_t i = 0; i < 3; ++i) {
            const std::string key1 = "1 rank " + std::to_string(i) + " index";
            SECTION(key1) { REQUIRE(ps.at(key1).rank() == i); }
        }

        for(std::size_t i = 1; i < 3; ++i) {
            const std::string key1 = "2 rank " + std::to_string(i) + " indices";
            SECTION(key1) { REQUIRE(ps.at(key1).rank() == i); }
        }
    }

    SECTION("size()") {
        SECTION("empty"){
            const auto& p0 = ps.at("empty");
            REQUIRE(p0.size() == 0);
        }

        for(std::size_t i = 0; i < 3; ++i){
            const auto key = "1 rank " + std::to_string(i) + " index";
            SECTION(key){ REQUIRE(ps.at(key).size() == 1); }
        }

        for(std::size_t i = 1; i < 3; ++i) {
            const auto key = "2 rank " + std::to_string(i) + " indices";
            SECTION(key) { REQUIRE(ps.at(key).size() == 2); }
        }
    }

    SECTION("result_extents") {
        SECTION("empty") {
            std::vector<std::size_t> corr;
            REQUIRE(ps.at("empty").result_extents() == corr);
        }

        SECTION("1 rank 0 index"){
            std::vector<std::size_t> corr;
            REQUIRE(ps.at("1 rank 0 index").result_extents() == corr);
        }

        SECTION("1 rank 1 index"){
            std::vector<std::size_t> corr{1};
            REQUIRE(ps.at("1 rank 1 index").result_extents() == corr);
        }

        SECTION("2 rank 1 indices"){
            std::vector<std::size_t> corr{2};
            REQUIRE(ps.at("2 rank 1 indices").result_extents() == corr);
        }

        SECTION("1 rank 2 index"){
            std::vector<std::size_t> corr{1, 1};
            REQUIRE(ps.at("1 rank 2 index").result_extents() == corr);
        }

        SECTION("2 rank 2 indices"){
            std::vector<std::size_t> corr{2, 2};
            REQUIRE(ps.at("2 rank 2 indices").result_extents() == corr);
        }

        SECTION("rank 2 with repeated offset") {
            auto& p = ps.at("2 rank 2 indices");
            p.insert(TestType{1, 4});
            std::vector<std::size_t> corr{2, 3};
            REQUIRE(p.result_extents() == corr);
        }

    }

    SECTION("result_index") {
        SECTION("empty"){
            const auto& p0 = ps.at("empty");
            REQUIRE_THROWS_AS(p0.result_index(e), std::out_of_range);
        }

        SECTION("1 rank 0 index") {
            const auto& p = ps.at("1 rank 0 index");
            SECTION("Good input index") {
                REQUIRE(p.result_index(e) == e);
            }
            
            SECTION("Bad index"){
                REQUIRE_THROWS_AS(p.result_index(e1), std::out_of_range);
            }
        }
        
        SECTION("1 rank 1 index") {
            const auto& p1 = ps.at("1 rank 1 index");
            SECTION("Good input index") {
                REQUIRE(p1.result_index(e1) == e0);
            }
            SECTION("Invalid input index"){
                SECTION("Out of bounds") {
                    REQUIRE_THROWS_AS(p1.result_index(TestType(3)), std::out_of_range);
                }
                SECTION("Wrong rank") {
                    REQUIRE_THROWS_AS(p1.result_index(e), std::out_of_range);
                }
            }
        }

        SECTION("2 rank 1 indices") {
            const auto& p1 = ps.at("2 rank 1 indices");
            SECTION("Good input index") {
                REQUIRE(p1.result_index(e1) == e0);
                REQUIRE(p1.result_index(e2) == e1);
            }
            SECTION("Invalid input index"){
                SECTION("Out of bounds") {
                    REQUIRE_THROWS_AS(p1.result_index(TestType(3)), std::out_of_range);
                }
                SECTION("Wrong rank") {
                    REQUIRE_THROWS_AS(p1.result_index(e), std::out_of_range);
                }
            }
        }

        SECTION("1 rank 2 index") {
            const auto& p1 = ps.at("1 rank 2 index");
            SECTION("Good input index") {
                REQUIRE(p1.result_index(e12) == TestType(0, 0));
            }
            SECTION("Invalid input index"){
                SECTION("Out of bounds") {
                    REQUIRE_THROWS_AS(p1.result_index(e23), std::out_of_range);
                }
                SECTION("Wrong rank") {
                    REQUIRE_THROWS_AS(p1.result_index(e), std::out_of_range);
                }
            }
        }

        SECTION("2 rank 2 indices") {
            const auto& p1 = ps.at("2 rank 2 indices");
            SECTION("Good input index") {
                REQUIRE(p1.result_index(e12) == TestType(0, 0));
                REQUIRE(p1.result_index(e23) == TestType(1, 1));
            }
            SECTION("Invalid input index"){
                SECTION("Out of bounds") {
                    REQUIRE_THROWS_AS(p1.result_index(TestType(3,3)), std::out_of_range);
                }
                SECTION("Wrong rank") {
                    REQUIRE_THROWS_AS(p1.result_index(e), std::out_of_range);
                }
            }
        }

        SECTION("rank 2 repeat offsets") {
            auto& p1 = ps.at("2 rank 2 indices");
            p1.insert(TestType{1, 3});
            SECTION("Good input index") {
                REQUIRE(p1.result_index(e12) == TestType(0, 0));
                REQUIRE(p1.result_index(TestType(1, 3)) == TestType(0, 1));
                REQUIRE(p1.result_index(e23) == TestType(1, 1));
            }
            SECTION("Invalid input index"){
                SECTION("Out of bounds") {
                    REQUIRE_THROWS_AS(p1.result_index(TestType(3,3)), std::out_of_range);
                }
                SECTION("Wrong rank") {
                    REQUIRE_THROWS_AS(p1.result_index(e), std::out_of_range);
                }
            }
        }
    }

    SECTION("at()") {
        SECTION("empty"){
            REQUIRE_THROWS_AS(ps.at("empty").at(0), std::out_of_range);
        }

        SECTION("1 rank 0 index"){
            const auto& p = ps.at("1 rank 0 index");
            SECTION("Good index") { REQUIRE(p.at(0) == e); }
            SECTION("Bad index") {
                REQUIRE_THROWS_AS(p.at(1), std::out_of_range);
            }
        }

        SECTION("1 rank 1 index"){
            const auto& p = ps.at("1 rank 1 index");
            SECTION("Good index") { REQUIRE(p.at(0) == e1); }
            SECTION("Bad index") {
                REQUIRE_THROWS_AS(p.at(1), std::out_of_range);
            }
        }

        SECTION("2 rank 1 indices"){
            const auto& p = ps.at("2 rank 1 indices");
            SECTION("Good index") {
                REQUIRE(p.at(0) == e1);
                REQUIRE(p.at(1) == e2);
            }
            SECTION("Bad index") {
                REQUIRE_THROWS_AS(p.at(2), std::out_of_range);
            }
        }

        SECTION("1 rank 2 index"){
            const auto& p = ps.at("1 rank 2 index");
            SECTION("Good index") { REQUIRE(p.at(0) == e12); }
            SECTION("Bad index") {
                REQUIRE_THROWS_AS(p.at(1), std::out_of_range);
            }
        }

        SECTION("2 rank 2 indices"){
            const auto& p = ps.at("2 rank 2 indices");
            SECTION("Good index") {
                REQUIRE(p.at(0) == e12);
                REQUIRE(p.at(1) == e23);
            }
            SECTION("Bad index") {
                REQUIRE_THROWS_AS(p.at(3), std::out_of_range);
            }
        }
    }

    SECTION("insert"){
        pimpl_type p0;
        using extents_type = typename pimpl_type::extents_type;
        SECTION("Add scalar index"){
            p0.insert(e);
            SECTION("Correct state") {
                REQUIRE(p0.size() == 1);
                REQUIRE(p0.rank() == 0);
                REQUIRE(p0.at(0) == e);
                REQUIRE(p0.result_extents().empty());
                REQUIRE(p0.result_index(e) == e);
            }
            SECTION("Throws if rank is greater") {
                REQUIRE_THROWS_AS(p0.insert(e1), std::runtime_error);
            }
        }

        SECTION("Repeated addition of scalar index"){
            p0.insert(e);
            p0.insert(e);
            REQUIRE(p0.size() == 1);
            REQUIRE(p0.rank() == 0);
            REQUIRE(p0.at(0) == e);
            REQUIRE(p0.result_extents().empty());
            REQUIRE(p0.result_index(e) == e);
        }

        auto& p1 = ps.at("1 rank 1 index");
        SECTION("Add vector index"){
            REQUIRE(p1.size() == 1);
            REQUIRE(p1.rank() == 1);
            REQUIRE(p1.at(0) == e1);
            REQUIRE(p1.result_extents() == extents_type{1});
            REQUIRE(p1.result_index(e1) == e0);
            SECTION("Throws if rank is less") {
                REQUIRE_THROWS_AS(p1.insert(e), std::runtime_error);
            }
            SECTION("Throws if rank is greater"){
                REQUIRE_THROWS_AS(p1.insert(e12), std::runtime_error);
            }

        }

        SECTION("Repeated addition of vector index"){
            p1.insert(e1);
            REQUIRE(p1.size() == 1);
            REQUIRE(p1.rank() == 1);
            REQUIRE(p1.at(0) == e1);
            REQUIRE(p1.result_extents() == extents_type{1});
            REQUIRE(p1.result_index(e1) == e0);
        }

        SECTION("Add two vector indices, lexicographical order"){
            p1.insert(e2);

            REQUIRE(p1.size() == 2);
            REQUIRE(p1.rank() == 1);
            REQUIRE(p1.at(0) == e1);
            REQUIRE(p1.at(1) == e2);
            REQUIRE(p1.result_extents() == extents_type{2});
            REQUIRE(p1.result_index(e1) == e0);
            REQUIRE(p1.result_index(e2) == e1);
        }

        SECTION("Add two vector indices, reverse-lexicographical order"){
            p0.insert(e2);
            p0.insert(e1);

            REQUIRE(p0.size() == 2);
            REQUIRE(p0.rank() == 1);
            REQUIRE(p0.at(0) == e1);
            REQUIRE(p0.at(1) == e2);
            REQUIRE(p0.result_extents() == extents_type{2});
            REQUIRE(p0.result_index(e1) == e0);
            REQUIRE(p0.result_index(e2) == e1);
        }

        auto& p2 = ps.at("1 rank 2 index");
        SECTION("Add matrix index"){
            REQUIRE(p2.size() == 1);
            REQUIRE(p2.rank() == 2);
            REQUIRE(p2.at(0) == e12);
            REQUIRE(p2.result_extents() == extents_type{1, 1});
            REQUIRE(p2.result_index(e12) == e00);
        }

        SECTION("Repeated addition of matrix index"){
            p2.insert(e12);
            REQUIRE(p2.size() == 1);
            REQUIRE(p2.rank() == 2);
            REQUIRE(p2.at(0) == e12);
            REQUIRE(p2.result_extents() == extents_type{1, 1});
            REQUIRE(p2.result_index(e12) == e00);
        }

        SECTION("Add two matrix indices, lexicographical order"){
            p2.insert(e23);
            REQUIRE(p2.size() == 2);
            REQUIRE(p2.rank() == 2);
            REQUIRE(p2.at(0) == e12);
            REQUIRE(p2.at(1) == e23);
            REQUIRE(p2.result_extents() == extents_type{2, 2});
            REQUIRE(p2.result_index(e12) == e00);
            REQUIRE(p2.result_index(e23) == e11);
        }

        SECTION("Add two matrix indices, reverse-lexicographical order"){
            p0.insert(e23);
            p0.insert(e12);

            REQUIRE(p0.size() == 2);
            REQUIRE(p0.rank() == 2);
            REQUIRE(p0.at(0) == e12);
            REQUIRE(p0.at(1) == e23);
            REQUIRE(p0.result_extents() == extents_type{2, 2});
            REQUIRE(p0.result_index(e12) == e00);
            REQUIRE(p0.result_index(e23) == e11);
        }
    }

    SECTION("operator*=") {
        using extents_type = typename pimpl_type::extents_type;
        TestType e000{0, 0, 0}, e011{0, 1, 1}, e112{1, 1, 2}, e123{1, 2, 3};
        TestType e100{1, 0, 0}, e111{1, 1, 1}, e121{1, 2, 1}, e212{2, 1, 2};
        TestType e001{0, 0, 1}, e122{1, 2, 2}, e223{2, 2, 3}, e231{2, 3, 1};
        TestType e110{1, 1, 0}, e232{2, 3, 2}, e2323{2, 3, 2, 3};
        TestType e0000{0, 0, 0, 0}, e1212{1, 2, 1, 2}, e1223{1, 2, 2, 3};
        TestType e0011{0, 0, 1, 1}, e2312{2, 3, 1, 2}, e1100{1, 1, 0, 0};
        TestType e1111{1, 1, 1, 1};

        SECTION("lhs == empty") {
            auto& lhs = ps.at("empty");
            for(const auto& [k, rhs] : ps){
                SECTION(k){
                    auto plhs = &(lhs *= rhs);
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                    SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                    SECTION("result_extents"){
                        REQUIRE(lhs.result_extents().empty());
                    }
                }
            }
        }

        SECTION("lhs == 1 rank 0 index") {
            auto& lhs = ps.at("1 rank 0 index");
            for(const auto& [k, rhs] : ps){
                SECTION(k){
                    auto plhs = &(lhs *= rhs);
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                    SECTION("Value") { REQUIRE(lhs == rhs); }
                    SECTION("result_extents") {
                        REQUIRE(lhs.result_extents() == rhs.result_extents());
                    }
                    SECTION("result_index"){
                        for(std::size_t i = 0; i < rhs.size(); ++i) {
                            const auto idx  = rhs.at(i);
                            const auto lidx = lhs.result_index(idx);
                            const auto ridx = rhs.result_index(idx);
                            REQUIRE(lidx == ridx);
                        }
                    }
                }
            }
        }

        SECTION("lhs == 1 rank 1 index") {
            auto& lhs = ps.at("1 rank 1 index");
            SECTION("rhs == empty"){
                auto& rhs = ps.at("empty");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == rhs); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                auto& rhs = ps.at("1 rank 0 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1});
                }
                SECTION("result_index"){ REQUIRE(lhs.result_index(e1) == e0); }
            }

            SECTION("rhs == 1 rank 1 index") {
                auto& rhs = ps.at("1 rank 1 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e11);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e11) == e00);
                }
            }

            SECTION("rhs == 2 rank 1 indices") {
                auto& rhs = ps.at("2 rank 1 indices");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e11);
                    corr.insert(e12);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e11) == e00);
                    REQUIRE(lhs.result_index(e12) == e01);
                }
            }

            SECTION("rhs == 1 rank 2 index") {
                auto& rhs = ps.at("1 rank 2 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e112);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e112) == e000);
                }
            }

            SECTION("rhs == 2 rank 2 indices") {
                auto& rhs = ps.at("2 rank 2 indices");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e112);
                    corr.insert(e123);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e112) == e000);
                    REQUIRE(lhs.result_index(e123) == e011);
                }
            }
        }

        SECTION("lhs == 2 rank 1 indices") {
            auto& lhs = ps.at("2 rank 1 indices");
            SECTION("rhs == empty"){
                auto& rhs = ps.at("empty");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == rhs); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                auto& rhs = ps.at("1 rank 0 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1);
                    corr.insert(e2);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                    REQUIRE(lhs.result_index(e2) == e1);
                }
            }

            SECTION("rhs == 1 rank 1 index") {
                auto& rhs = ps.at("1 rank 1 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e11);
                    corr.insert(e21);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e11) == e00);
                    REQUIRE(lhs.result_index(e21) == e10);
                }
            }

            SECTION("rhs == 2 rank 1 indices") {
                auto& rhs = ps.at("2 rank 1 indices");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e11);
                    corr.insert(e12);
                    corr.insert(e21);
                    corr.insert(e22);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e11) == e00);
                    REQUIRE(lhs.result_index(e12) == e01);
                    REQUIRE(lhs.result_index(e21) == e10);
                    REQUIRE(lhs.result_index(e22) == e11);
                }
            }

            SECTION("rhs == 1 rank 2 index") {
                auto& rhs = ps.at("1 rank 2 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e112);
                    corr.insert(e212);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e112) == e000);
                    REQUIRE(lhs.result_index(e212) == e100);
                }
            }

            SECTION("rhs == 2 rank 2 indices") {
                auto& rhs = ps.at("2 rank 2 indices");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e112);
                    corr.insert(e123);
                    corr.insert(e212);
                    corr.insert(e223);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e112) == e000);
                    REQUIRE(lhs.result_index(e123) == e011);
                    REQUIRE(lhs.result_index(e212) == e100);
                    REQUIRE(lhs.result_index(e223) == e111);
                }
            }
        }

        SECTION("lhs == 1 rank 2 index") {
            auto& lhs = ps.at("1 rank 2 index");
            SECTION("rhs == empty"){
                auto& rhs = ps.at("empty");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == rhs); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                auto& rhs = ps.at("1 rank 0 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e12);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                }
            }

            SECTION("rhs == 1 rank 1 index") {
                auto& rhs = ps.at("1 rank 1 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e121);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e121) == e000);
                }
            }

            SECTION("rhs == 2 rank 1 indices") {
                auto& rhs = ps.at("2 rank 1 indices");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e121);
                    corr.insert(e122);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e121) == e000);
                    REQUIRE(lhs.result_index(e122) == e001);
                }
            }

            SECTION("rhs == 1 rank 2 index") {
                auto& rhs = ps.at("1 rank 2 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1212);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1, 1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1212) == e0000);
                }
            }

            SECTION("rhs == 2 rank 2 indices") {
                auto& rhs = ps.at("2 rank 2 indices");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1212);
                    corr.insert(e1223);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1, 2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1212) == e0000);
                    REQUIRE(lhs.result_index(e1223) == e0011);
                }
            }
        }

        SECTION("lhs == 2 rank 2 indices") {
            auto& lhs = ps.at("2 rank 2 indices");
            SECTION("rhs == empty"){
                auto& rhs = ps.at("empty");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == rhs); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                auto& rhs = ps.at("1 rank 0 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e12);
                    corr.insert(e23);
                    REQUIRE(lhs == corr);
                }
            }

            SECTION("rhs == 1 rank 1 index") {
                auto& rhs = ps.at("1 rank 1 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e121);
                    corr.insert(e231);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e121) == e000);
                    REQUIRE(lhs.result_index(e231) == e110);
                }
            }

            SECTION("rhs == 2 rank 1 indices") {
                auto& rhs = ps.at("2 rank 1 indices");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e121);
                    corr.insert(e122);
                    corr.insert(e231);
                    corr.insert(e232);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e121) == e000);
                    REQUIRE(lhs.result_index(e122) == e001);
                    REQUIRE(lhs.result_index(e231) == e110);
                    REQUIRE(lhs.result_index(e232) == e111);
                }
            }

            SECTION("rhs == 1 rank 2 index") {
                auto& rhs = ps.at("1 rank 2 index");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1212);
                    corr.insert(e2312);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2, 1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1212) == e0000);
                    REQUIRE(lhs.result_index(e2312) == e1100);
                }
            }

            SECTION("rhs == 2 rank 2 indices") {
                auto& rhs = ps.at("2 rank 2 indices");
                auto plhs = &(lhs *= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1212);
                    corr.insert(e1223);
                    corr.insert(e2312);
                    corr.insert(e2323);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2, 2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1212) == e0000);
                    REQUIRE(lhs.result_index(e1223) == e0011);
                    REQUIRE(lhs.result_index(e2312) == e1100);
                    REQUIRE(lhs.result_index(e2323) == e1111);
                }
            }
        }
    }

    SECTION("operator+=") {
        using extents_type = typename pimpl_type::extents_type;

        SECTION("lhs == empty") {
            auto& lhs = ps.at("empty");
            for(const auto& [k, rhs] : ps) {
                SECTION(k){
                    auto plhs = &(lhs += rhs);
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                    SECTION("Value") { REQUIRE(lhs == rhs); }
                    SECTION("result_extents"){
                        REQUIRE(lhs.result_extents() == rhs.result_extents());
                    }
                    SECTION("result_index"){
                        for(std::size_t i = 0; i < rhs.size(); ++i){
                            const auto idx  = rhs.at(i);
                            const auto lidx = lhs.result_index(idx);
                            const auto ridx = rhs.result_index(idx);
                            REQUIRE(lidx == ridx);
                        }
                    }
                }
            }
        }

        SECTION("lhs == rank 0") {
            auto& lhs = ps.at("1 rank 0 index");
            SECTION("rhs == empty"){
                auto plhs = &(lhs += ps.at("empty"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e) == e);
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                pimpl_type corr;
                corr.insert(e);
                auto plhs = &(lhs += corr);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == corr); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e) == e);
                }
            }

            SECTION("rhs == 1 rank 1 index"){
                REQUIRE_THROWS_AS(lhs += ps.at("1 rank 1 index"), std::runtime_error);
            }

            SECTION("rhs == 2 rank 1 indices"){
                REQUIRE_THROWS_AS(lhs += ps.at("2 rank 1 indices"), std::runtime_error);
            }

            SECTION("rhs == 1 rank 2 index"){
                REQUIRE_THROWS_AS(lhs += ps.at("1 rank 2 index"), std::runtime_error);
            }

            SECTION("rhs == 2 rank 2 indices"){
                REQUIRE_THROWS_AS(lhs += ps.at("2 rank 2 indices"), std::runtime_error);
            }
        }

        SECTION("lhs == 1 rank 1 index") {
            auto& lhs = ps.at("1 rank 1 index");
            SECTION("rhs == empty"){
                auto plhs = &(lhs += ps.at("empty"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                REQUIRE_THROWS_AS(lhs += ps.at("1 rank 0 index"), std::runtime_error);
            }

            SECTION("rhs == 1 rank 1 index"){
                auto plhs = &(lhs += ps.at("1 rank 1 index"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                }
            }

            SECTION("rhs == 2 rank 1 indices"){
                auto& rhs = ps.at("2 rank 1 indices");
                auto plhs = &(lhs += rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == rhs); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                    REQUIRE(lhs.result_index(e2) == e1);
                }
            }

            SECTION("rhs == 1 rank 2 index"){
                REQUIRE_THROWS_AS(lhs += ps.at("1 rank 2 index"), std::runtime_error);
            }

            SECTION("rhs == 2 rank 2 indices"){
                REQUIRE_THROWS_AS(lhs += ps.at("2 rank 2 indices"), std::runtime_error);
            }
        }

        SECTION("lhs == 2 rank 1 indices") {
            auto& lhs = ps.at("2 rank 1 indices");
            pimpl_type corr;
            corr.insert(e1);
            corr.insert(e2);
            SECTION("rhs == empty"){
                auto plhs = &(lhs += ps.at("empty"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == corr); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                    REQUIRE(lhs.result_index(e2) == e1);
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                REQUIRE_THROWS_AS(lhs += ps.at("1 rank 0 index"), std::runtime_error);
            }

            SECTION("rhs == 1 rank 1 index"){
                auto plhs = &(lhs += ps.at("1 rank 1 index"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == corr); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                    REQUIRE(lhs.result_index(e2) == e1);
                }
            }

            SECTION("rhs == 2 rank 1 indices"){
                auto& rhs = ps.at("2 rank 1 indices");
                auto plhs = &(lhs += rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == corr); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                    REQUIRE(lhs.result_index(e2) == e1);
                }
            }

            SECTION("rhs == 1 rank 2 index"){
                REQUIRE_THROWS_AS(lhs += ps.at("1 rank 2 index"), std::runtime_error);
            }

            SECTION("rhs == 2 rank 2 indices"){
                REQUIRE_THROWS_AS(lhs += ps.at("2 rank 2 indices"), std::runtime_error);
            }
        }

        SECTION("lhs == 1 rank 2 index") {
            auto& lhs = ps.at("1 rank 2 index");
            SECTION("rhs == empty"){
                auto plhs = &(lhs += ps.at("empty"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e12);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                REQUIRE_THROWS_AS(lhs += ps.at("1 rank 0 index"), std::runtime_error);
            }

            SECTION("rhs == 1 rank 1 index"){
                REQUIRE_THROWS_AS(lhs += ps.at("1 rank 1 index"), std::runtime_error);
            }

            SECTION("rhs == 2 rank 1 indices"){
                REQUIRE_THROWS_AS(lhs += ps.at("2 rank 1 indices"), std::runtime_error);
            }

            SECTION("rhs == 1 rank 2 index"){
                auto plhs = &(lhs += ps.at("1 rank 2 index"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e12);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                }
            }

            SECTION("rhs == 2 rank 2 indices"){
                auto& rhs = ps.at("2 rank 2 indices");
                auto plhs = &(lhs += rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == rhs); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                    REQUIRE(lhs.result_index(e23) == e11);
                }
            }
        }

        SECTION("lhs == 2 rank 1 indices") {
            auto& lhs = ps.at("2 rank 2 indices");
            pimpl_type corr;
            corr.insert(e12);
            corr.insert(e23);
            SECTION("rhs == empty"){
                auto plhs = &(lhs += ps.at("empty"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == corr); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                    REQUIRE(lhs.result_index(e23) == e11);
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                REQUIRE_THROWS_AS(lhs += ps.at("1 rank 0 index"), std::runtime_error);
            }

            SECTION("rhs == 1 rank 1 index"){
                REQUIRE_THROWS_AS(lhs += ps.at("1 rank 1 index"), std::runtime_error);
            }

            SECTION("rhs == 2 rank 1 indices"){
                REQUIRE_THROWS_AS(lhs += ps.at("2 rank 1 indices"), std::runtime_error);
            }

            SECTION("rhs == 1 rank 2 index"){
                auto plhs = &(lhs += ps.at("1 rank 2 index"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == corr); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                    REQUIRE(lhs.result_index(e23) == e11);
                }
            }

            SECTION("rhs == 2 rank 2 indices"){
                auto& rhs = ps.at("2 rank 2 indices");
                auto plhs = &(lhs += rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == corr); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                    REQUIRE(lhs.result_index(e23) == e11);
                }
            }
        }
    }

    SECTION("operator^=") {
        using extents_type = typename pimpl_type::extents_type;
        SECTION("lhs == empty") {
            auto& lhs = ps.at("empty");
            pimpl_type corr;
            for(const auto& [k, rhs] : ps) {
                SECTION(k){
                    auto plhs = &(lhs ^= rhs);
                    SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                    SECTION("Value") { REQUIRE(lhs == corr); }
                    SECTION("result_extents"){
                        REQUIRE(lhs.result_extents().empty());
                    }
                }
            }
        }

        SECTION("lhs == rank 0") {
            auto& lhs = ps.at("1 rank 0 index");
            SECTION("rhs == empty"){
                auto plhs = &(lhs ^= ps.at("empty"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                pimpl_type corr;
                corr.insert(e);
                auto plhs = &(lhs ^= corr);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == corr); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e) == e);
                }
            }

            SECTION("rhs == 1 rank 1 index"){
                auto plhs = &(lhs ^= ps.at("1 rank 1 index"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 2 rank 1 indices"){
                auto plhs = &(lhs ^= ps.at("2 rank 1 indices"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 2 index"){
                auto plhs = &(lhs ^= ps.at("1 rank 2 index"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 2 rank 2 indices"){
                auto plhs = &(lhs ^= ps.at("2 rank 2 indices"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }
        }

        SECTION("lhs == 1 rank 1 index") {
            auto& lhs = ps.at("1 rank 1 index");
            SECTION("rhs == empty"){
                auto plhs = &(lhs ^= ps.at("empty"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 0 index"){
              auto& rhs = ps.at("1 rank 0 index");
              auto plhs = &(lhs ^= rhs);
              SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
              SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 1 index"){
                auto plhs = &(lhs ^= ps.at("1 rank 1 index"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                }
            }

            SECTION("rhs == 2 rank 1 indices"){
                auto& rhs = ps.at("2 rank 1 indices");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                }
            }

            SECTION("rhs == 1 rank 2 index"){
                auto& rhs = ps.at("1 rank 2 index");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 2 rank 2 indices"){
                auto& rhs = ps.at("2 rank 2 indices");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }
        }

        SECTION("lhs == 2 rank 1 indices") {
            auto& lhs = ps.at("2 rank 1 indices");
            SECTION("rhs == empty"){
                auto plhs = &(lhs ^= ps.at("empty"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                auto& rhs = ps.at("1 rank 0 index");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 1 index"){
                auto plhs = &(lhs ^= ps.at("1 rank 1 index"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                }
            }

            SECTION("rhs == 2 rank 1 indices"){
                auto& rhs = ps.at("2 rank 1 indices");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e1);
                    corr.insert(e2);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e1) == e0);
                    REQUIRE(lhs.result_index(e2) == e1);
                }
            }

            SECTION("rhs == 1 rank 2 index"){
                auto& rhs = ps.at("1 rank 2 index");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 2 rank 2 indices"){
                auto& rhs = ps.at("2 rank 2 indices");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }
        }

        SECTION("lhs == 1 rank 2 index") {
            auto& lhs = ps.at("1 rank 2 index");
            SECTION("rhs == empty"){
                auto plhs = &(lhs ^= ps.at("empty"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                auto& rhs = ps.at("1 rank 0 index");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 1 index"){
                auto& rhs = ps.at("1 rank 1 index");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 2 rank 1 indices"){
                auto& rhs = ps.at("2 rank 1 indices");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 2 index"){
                auto plhs = &(lhs ^= ps.at("1 rank 2 index"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e12);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                }
            }

            SECTION("rhs == 2 rank 2 indices"){
                auto& rhs = ps.at("2 rank 2 indices");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e12);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                }
            }
        }

        SECTION("lhs == 2 rank 2 indices") {
            auto& lhs = ps.at("2 rank 2 indices");
            SECTION("rhs == empty"){
                auto plhs = &(lhs ^= ps.at("empty"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 0 index"){
                auto& rhs = ps.at("1 rank 0 index");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 1 index"){
                auto& rhs = ps.at("1 rank 1 index");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 2 rank 1 indices"){
                auto& rhs = ps.at("2 rank 1 indices");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == pimpl_type{}); }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents().empty());
                }
            }

            SECTION("rhs == 1 rank 2 index"){
                auto plhs = &(lhs ^= ps.at("1 rank 2 index"));
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e12);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{1, 1});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                }
            }

            SECTION("rhs == 2 rank 2 indices"){
                auto& rhs = ps.at("2 rank 2 indices");
                auto plhs = &(lhs ^= rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") {
                    pimpl_type corr;
                    corr.insert(e12);
                    corr.insert(e23);
                    REQUIRE(lhs == corr);
                }
                SECTION("result_extents"){
                    REQUIRE(lhs.result_extents() == extents_type{2, 2});
                }
                SECTION("result_index"){
                    REQUIRE(lhs.result_index(e12) == e00);
                    REQUIRE(lhs.result_index(e23) == e11);
                }
            }
        }
    }

    SECTION("hash"){
        auto h1 = sde::hash_objects(ps.at("empty"));

        SECTION("Empty"){ REQUIRE(h1 == sde::hash_objects(pimpl_type{})); }

        SECTION("Single element"){
            REQUIRE(h1 != sde::hash_objects(ps.at("1 rank 1 index")));
        }

        SECTION("Two elements"){
            REQUIRE(h1 != sde::hash_objects(ps.at("2 rank 2 indices")));
        }
    }
}

TEMPLATE_LIST_TEST_CASE("DomainPIMPL comparisons", "", index_types){
    using DomainPIMPL = libchemist::sparse_map::detail_::DomainPIMPL<TestType>;

    TestType e1{1}, e12{1, 2}, e2{2};

    SECTION("Same instance"){
        DomainPIMPL p1;
        REQUIRE(p1 == p1);
        REQUIRE_FALSE(p1 != p1);
    }

    SECTION("Default == Default"){
        DomainPIMPL p1, p2;
        REQUIRE(p1 == p2);
        REQUIRE_FALSE(p1 != p2);
    }

    SECTION("Default != Non-Default"){
        DomainPIMPL p1, p2;
        p2.insert(e1);
        REQUIRE_FALSE(p1 == p2);
        REQUIRE(p1 != p2);
    }

    SECTION("Two non-defaults"){
        DomainPIMPL p1, p2;
        p1.insert(e1);

        SECTION("Same contents"){
            p2.insert(e1);
            REQUIRE(p1 == p2);
            REQUIRE_FALSE(p1 != p2);
        }

        SECTION("Different contents"){
            SECTION("Different index"){
                p2.insert(e2);
                REQUIRE_FALSE(p1 == p2);
                REQUIRE(p1 != p2);
            }

            SECTION("Additional elements"){
                p2.insert(e1);
                p2.insert(e2);
                REQUIRE_FALSE(p1 == p2);
                REQUIRE(p1 != p2);
            }

            SECTION("Different rank"){
                p2.insert(e12);
                REQUIRE_FALSE(p1 == p2);
                REQUIRE(p1 != p2);
            }
        }
    }
}
