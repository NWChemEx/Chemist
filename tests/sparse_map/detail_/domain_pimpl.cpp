#include "libchemist/sparse_map/detail_/domain_pimpl.hpp"
#include "libchemist/sparse_map/index.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::sparse_map;
using index_types = std::tuple<ElementIndex, TileIndex>;

TEMPLATE_LIST_TEST_CASE("DomainPIMPL", "", index_types) {
    using namespace libchemist::sparse_map::detail_;
    using DomainPIMPL = DomainPIMPL<TestType>;

    // Here we make PIMPLs: p0 which is empty, p1 contains a rank 1 index, and
    // p2 contains a rank 2 index.
    DomainPIMPL p0, p1, p2;
    p1.insert(TestType(1));
    p2.insert(TestType(1, 2));
    
    SECTION("Typedefs"){
        using traits = libchemist::sparse_map::detail_::DomainTraits<TestType>;

        SECTION("size_type"){
            using pimpl_t = typename DomainPIMPL::size_type;
            using corr_t  = typename traits::size_type;
            STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
        }

        SECTION("value_type"){
            using pimpl_t = typename DomainPIMPL::value_type;
            using corr_t  = typename traits::value_type;
            STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
        }

        SECTION("const_reference"){
            using pimpl_t = typename DomainPIMPL::const_reference;
            using corr_t  = typename traits::const_reference;
            STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
        }
    } // typedefs

    SECTION("Ctors") {
        SECTION("Default Ctor") {
            SECTION("rank") { REQUIRE(p0.rank() == 0); }
            SECTION("size") { REQUIRE(p0.size() == 0); }
        }

        SECTION("Copy Ctor") {
            SECTION("Default instance") {
                DomainPIMPL p0_copy(p0);
                REQUIRE(p0_copy == p0);
            }

            SECTION("Rank 1 index") {
                DomainPIMPL p1_copy(p1);

                SECTION("value") { REQUIRE(p1_copy == p1); }
                SECTION("is deep copy") {
                    REQUIRE(&p1_copy.at(0) != &p1.at(0));
                }
            }
        }

        SECTION("Move Ctor") {
            SECTION("Default instance") {
                DomainPIMPL corr;
                DomainPIMPL p0_move(std::move(p0));
                REQUIRE(p0_move == corr);
            }

            SECTION("Non-default") {
                DomainPIMPL corr(p1);
                DomainPIMPL p1_move(std::move(p1));
                REQUIRE(corr == p1);
            }
        }

        SECTION("Copy Assignment") {
            SECTION("Default instance") {
                DomainPIMPL p0_assign;
                auto addr = &(p0_assign = p0);
                SECTION("value") { REQUIRE(p0_assign == p0); }
                SECTION("returns this") { REQUIRE(addr == &p0_assign); }
            }

            SECTION("Non-default") {
                DomainPIMPL p1_assign;
                auto addr = &(p1_assign = p1);

                SECTION("value") { REQUIRE(p1_assign == p1); }
                SECTION("is deep copy") {
                    REQUIRE(&p1_assign.at(0) != &p1.at(0));
                }
                SECTION("returns this") { REQUIRE(addr == &p1_assign); }
            }
        }

        SECTION("Move Assignment") {
            SECTION("Default instance") {
                DomainPIMPL corr, p0_assign;
                auto addr = &(p0_assign = std::move(p0));
                SECTION("value") { REQUIRE(p0_assign == corr); }
                SECTION("returns this") { REQUIRE(addr == &p0_assign); }
            }

            SECTION("Non-default") {
                DomainPIMPL corr(p1), p1_assign;
                auto addr = &(p1_assign = std::move(p1));

                SECTION("value") { REQUIRE(p1_assign == corr); }
                SECTION("returns this") { REQUIRE(addr == &p1_assign); }
            }
        }
    } // SECTION("Ctors")

    SECTION("clone()"){
        SECTION("Default instance"){
            auto p0_copy = p0.clone();
            REQUIRE(*p0_copy == p0);
        }

        SECTION("Rank 1 index"){
            auto p1_copy = p1.clone();
            SECTION("value"){ REQUIRE(*p1_copy == p1); }
            SECTION("is deep copy") { REQUIRE(&(p1_copy->at(0)) != &p1.at(0)); }
        }
    }

    SECTION("count()") {
        SECTION("Empty"){ REQUIRE_FALSE(p0.count(TestType{})); }
        SECTION("Non-empty"){
            SECTION("Has value"){ REQUIRE(p1.count(TestType{1})); }
            SECTION("Does not have value"){ 
                REQUIRE_FALSE(p1.count(TestType{})); 
            }
        }
    }

    SECTION("rank()") {
        SECTION("Empty"){ REQUIRE(p0.rank() == 0); }
        
        SECTION("Rank 0"){
            DomainPIMPL p;
            p.insert(TestType{});
            REQUIRE(p.rank() == 0);
        }

        SECTION("Rank 1"){ REQUIRE(p1.rank() == 1); }

        SECTION("Rank 2"){ REQUIRE(p2.rank() == 2); }

        SECTION("Rank 3"){
            DomainPIMPL p;
            p.insert(TestType{1, 2, 3});
            REQUIRE(p.rank() == 3);
        }
    }

    SECTION("size") {
        SECTION("Empty"){ REQUIRE(p0.size() == 0); }

        SECTION("Single element"){ REQUIRE(p1.size() == 1); }

        SECTION("Two elements"){
            p1.insert(TestType{2});
            REQUIRE(p1.size() == 2);
        }

        SECTION("Three elements"){
            p1.insert(TestType{2});
            p1.insert(TestType{3});
            REQUIRE(p1.size() == 3);
        }
    }

    SECTION("result_extents") {
        SECTION("Empty") {
            std::vector<std::size_t> corr;
            REQUIRE(p0.result_extents() == corr);
        }

        SECTION("Rank 1") {
            std::vector<std::size_t> corr{1};
            REQUIRE(p1.result_extents() == corr);
        }

        SECTION("Rank 2") {
            p2.insert(TestType(1, 3));
            std::vector<std::size_t> corr{1, 2};
            REQUIRE(p2.result_extents() == corr);
        }

    }

    SECTION("result_index") {
        SECTION("Empty domain"){
            REQUIRE_THROWS_AS(p0.result_index(TestType{}), std::out_of_range);
        }

        SECTION("rank 1 domain") {
            SECTION("Good input index") {
                auto idx = p1.result_index(TestType(1));
                TestType corr(0);
                REQUIRE(idx == corr);
            }
            SECTION("Invalid input index"){
                REQUIRE_THROWS_AS(p1.result_index(TestType()), std::out_of_range);
            }
        }

        SECTION("rank 2 domain") {
            p2.insert(TestType(1, 3));
            SECTION("Good index 0") {
                auto idx = p2.result_index(TestType(1, 2));
                TestType corr(0, 0);
                REQUIRE(idx == corr);
            }
            SECTION("Good index 1") {
                auto idx = p2.result_index(TestType(1, 3));
                TestType corr(0, 1);
                REQUIRE(idx == corr);
            }
            SECTION("Invalid input index"){
                REQUIRE_THROWS_AS(p2.result_index(TestType()), std::out_of_range);
            }
        }
    }

    SECTION("at()") {
        SECTION("Single element"){
            SECTION("Element 0") { REQUIRE(p1.at(0) == TestType{1}); }
            SECTION("Not in range [0, 1)") {
                REQUIRE_THROWS_AS(p1.at(1), std::out_of_range);
            }
        }

        SECTION("Two elements"){
            p1.insert(TestType{2});
            SECTION("Element 0") { REQUIRE(p1.at(0) == TestType{1}); }
            SECTION("Element 1") { REQUIRE(p1.at(1) == TestType{2}); }
            SECTION("Not in range [0, 2)") {
                REQUIRE_THROWS_AS(p1.at(2), std::out_of_range);
            }
        }

        SECTION("Three elements"){
            p1.insert(TestType{2});
            p1.insert(TestType{3});
            SECTION("Element 0") { REQUIRE(p1.at(0) == TestType{1}); }
            SECTION("Element 1") { REQUIRE(p1.at(1) == TestType{2}); }
            SECTION("Element 2") { REQUIRE(p1.at(2) == TestType{3}); }
            SECTION("Not in range [0, 3)") {
                REQUIRE_THROWS_AS(p1.at(3), std::out_of_range);
            }
        }
    }

    SECTION("insert"){
        SECTION("Add scalar index"){
            p0.insert(TestType{});
            REQUIRE(p0.size() == 1);
            REQUIRE(p0.rank() == 0);
            REQUIRE(p0.at(0) == TestType{});
            SECTION("Throws if rank is greater") {
                REQUIRE_THROWS_AS(p0.insert(TestType{1}), std::runtime_error);
            }
        }

        SECTION("Repeated addition of scalar index"){
            p0.insert(TestType{});
            p0.insert(TestType{});
            REQUIRE(p0.size() == 1);
            REQUIRE(p0.rank() == 0);
            REQUIRE(p0.at(0) == TestType{});
        }

        SECTION("Add vector index"){
            REQUIRE(p1.size() == 1);
            REQUIRE(p1.rank() == 1);
            REQUIRE(p1.at(0) == TestType{1});
            SECTION("Throws if rank is less") {
                REQUIRE_THROWS_AS(p1.insert(TestType{}), std::runtime_error);
            }
            SECTION("Throws if rank is greater"){
                REQUIRE_THROWS_AS(p1.insert(TestType{1, 2}), std::runtime_error);
            }

        }

        SECTION("Repeated addition of vector index"){
            p1.insert(TestType{1});
            REQUIRE(p1.size() == 1);
            REQUIRE(p1.rank() == 1);
            REQUIRE(p1.at(0) == TestType{1});
        }

        SECTION("Add two vector indices, lexicographical order"){
            p1.insert(TestType{2});

            REQUIRE(p1.size() == 2);
            REQUIRE(p1.rank() == 1);
            REQUIRE(p1.at(0) == TestType{1});
            REQUIRE(p1.at(1) == TestType{2});
        }

        SECTION("Add two vector indices, reverse-lexicographical order"){
            p0.insert(TestType{2});
            p0.insert(TestType{1});

            REQUIRE(p0.size() == 2);
            REQUIRE(p0.rank() == 1);
            REQUIRE(p0.at(0) == TestType{1});
            REQUIRE(p0.at(1) == TestType{2});
        }

        SECTION("Add matrix index"){
            REQUIRE(p2.size() == 1);
            REQUIRE(p2.rank() == 2);
            REQUIRE(p2.at(0) == TestType{1, 2});
        }

        SECTION("Repeated addition of matrix index"){
            p2.insert(TestType{1, 2});
            REQUIRE(p2.size() == 1);
            REQUIRE(p2.rank() == 2);
            REQUIRE(p2.at(0) == TestType{1, 2});
        }

        SECTION("Add two vector indices, lexicographical order"){
            p2.insert(TestType{2, 3});
            REQUIRE(p2.size() == 2);
            REQUIRE(p2.rank() == 2);
            REQUIRE(p2.at(0) == TestType{1, 2});
            REQUIRE(p2.at(1) == TestType{2, 3});
        }

        SECTION("Add two vector indices, reverse-lexicographical order"){
            p0.insert(TestType{2, 3});
            p0.insert(TestType{1, 2});

            REQUIRE(p0.size() == 2);
            REQUIRE(p0.rank() == 2);
            REQUIRE(p0.at(0) == TestType{1, 2});
            REQUIRE(p0.at(1) == TestType{2, 3});
        }
    }

    SECTION("hash"){
        auto h1 = sde::hash_objects(p0);

        SECTION("Empty"){ REQUIRE(h1 == sde::hash_objects(DomainPIMPL{})); }

        SECTION("Single element"){ REQUIRE(h1 != sde::hash_objects(p1)); }

        SECTION("Two elements"){
            p1.insert(TestType{2});
            REQUIRE(h1 != sde::hash_objects(p1));
        }
    }
}

TEMPLATE_LIST_TEST_CASE("DomainPIMPL comparisons", "", index_types){
    using DomainPIMPL = libchemist::sparse_map::detail_::DomainPIMPL<TestType>;

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
        p2.insert(TestType{1});
        REQUIRE_FALSE(p1 == p2);
        REQUIRE(p1 != p2);
    }

    SECTION("Two non-defaults"){
        DomainPIMPL p1, p2;
        p1.insert(TestType{1});

        SECTION("Same contents"){
            p2.insert(TestType{1});
            REQUIRE(p1 == p2);
            REQUIRE_FALSE(p1 != p2);
        }

        SECTION("Different contents"){
            SECTION("Different index"){
                p2.insert(TestType{2});
                REQUIRE_FALSE(p1 == p2);
                REQUIRE(p1 != p2);
            }

            SECTION("Additional elements"){
                p2.insert(TestType{1});
                p2.insert(TestType{2});
                REQUIRE_FALSE(p1 == p2);
                REQUIRE(p1 != p2);
            }

            SECTION("Different rank"){
                p2.insert(TestType{1, 2});
                REQUIRE_FALSE(p1 == p2);
                REQUIRE(p1 != p2);
            }
        }
    }
}
