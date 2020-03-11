#include <catch2/catch.hpp>
#include <libchemist/sparse_map/detail_/domain_pimpl.hpp>

using namespace libchemist::sparse_map::detail_;
using value_type = typename DomainPIMPL::value_type;

// Makes sure the typedefs stay synched with the Domain class
TEST_CASE("DomainPIMPL : Typedefs"){
    using libchemist::sparse_map::Domain;

    SECTION("size_type"){
        using pimpl_t = typename DomainPIMPL::size_type;
        using corr_t = typename Domain::size_type;
        STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
    }

    SECTION("value_type"){
        using pimpl_t = typename DomainPIMPL::value_type;
        using corr_t = typename Domain::value_type;
        STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
    }

    SECTION("reference"){
        using pimpl_t = typename DomainPIMPL::reference;
        using corr_t = typename Domain::reference;
        STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
    }

    SECTION("const_reference"){
        using pimpl_t = typename DomainPIMPL::const_reference;
        using corr_t = typename Domain::const_reference;
        STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
    }
}

TEST_CASE("DomainPIMPL : Default Ctor"){
    DomainPIMPL p;
    SECTION("rank") { REQUIRE(p.rank() == 0); }
    SECTION("size") { REQUIRE(p.size() == 0); }
    SECTION("hash") {
        auto hash = sde::hash_objects(p);
        REQUIRE(hash == "50e20c9ba664054fb322f0499e108b5f");
    }
}

TEST_CASE("DomainPIMPL : Copy Ctor"){
    SECTION("Default instance"){
        DomainPIMPL p1;
        DomainPIMPL p2(p1);
        REQUIRE(p1 == p2);
    }

    SECTION("Non-default"){
        DomainPIMPL p1;
        p1.insert(value_type{1});
        DomainPIMPL p2(p1);

        SECTION("value"){ REQUIRE(p1 == p2); }
        SECTION("is deep copy") { REQUIRE(&p1.at(0) != &p2.at(0)); }
    }
}

TEST_CASE("DomainPIMPL : Move Ctor"){
    SECTION("Default instance"){
        DomainPIMPL p1;
        DomainPIMPL p2(std::move(p1));
        REQUIRE(DomainPIMPL{} == p2);
    }

    SECTION("Non-default"){
        DomainPIMPL p1;
        p1.insert(value_type{1});
        DomainPIMPL corr(p1);
        DomainPIMPL p2(std::move(p1));
        REQUIRE(corr == p2);
    }
}

TEST_CASE("DomainPIMPL : Copy Assignment"){
    SECTION("Default instance"){
        DomainPIMPL p1;
        DomainPIMPL p2;
        auto pp2 = &(p2 = p1);
        SECTION("value") { REQUIRE(p1 == p2); }
        SECTION("returns this") { REQUIRE(pp2 == &p2); }
    }

    SECTION("Non-default"){
        DomainPIMPL p1;
        p1.insert(value_type{1});
        DomainPIMPL p2;
        auto pp2 = &(p2 = p1);

        SECTION("value"){ REQUIRE(p1 == p2); }
        SECTION("is deep copy") { REQUIRE(&p1.at(0) != &p2.at(0)); }
        SECTION("returns this") { REQUIRE(pp2 == &p2); }
    }
}

TEST_CASE("DomainPIMPL : Move Assignment"){
    SECTION("Default instance"){
        DomainPIMPL p1;
        DomainPIMPL p2;
        DomainPIMPL p3(p1);
        auto pp2 = &(p2 = std::move(p1));
        SECTION("value") { REQUIRE(p3 == p2); }
        SECTION("returns this") { REQUIRE(pp2 == &p2); }
    }

    SECTION("Non-default"){
        DomainPIMPL p1;
        p1.insert(value_type{1});
        DomainPIMPL p3(p1);
        DomainPIMPL p2;
        auto pp2 = &(p2 = std::move(p1));

        SECTION("value"){ REQUIRE(p3 == p2); }
        SECTION("returns this") { REQUIRE(pp2 == &p2); }
    }
}

TEST_CASE("DomainPIMPL : count") {
    DomainPIMPL p;

    SECTION("Empty"){ REQUIRE_FALSE(p.count(value_type{})); }
    SECTION("Non-empty"){
        p.insert(value_type{1});

        SECTION("Has value"){ REQUIRE(p.count(value_type{1})); }
        SECTION("Does not have value"){ REQUIRE_FALSE(p.count(value_type{})); }
    }
}

TEST_CASE("DomainPIMPL : rank") {
    DomainPIMPL p;

    SECTION("Empty"){ REQUIRE(p.rank() == 0); }

    SECTION("Rank 0"){
        p.insert(value_type{});
        REQUIRE(p.rank() == 0);
    }

    SECTION("Rank 1"){
        p.insert(value_type{1});
        REQUIRE(p.rank() == 1);
    }

    SECTION("Rank 2"){
        p.insert(value_type{1, 2});
        REQUIRE(p.rank() == 2);
    }

    SECTION("Rank 3"){
        p.insert(value_type{1, 2, 3});
        REQUIRE(p.rank() == 3);
    }
}

TEST_CASE("DomainPIMPL : size") {
    DomainPIMPL p;

    SECTION("Empty"){ REQUIRE(p.size() == 0); }

    SECTION("Single element"){
        p.insert(value_type{1});
        REQUIRE(p.size() == 1);
    }

    SECTION("Two elements"){
        p.insert(value_type{1});
        p.insert(value_type{2});
        REQUIRE(p.size() == 2);
    }

    SECTION("Three elements"){
        p.insert(value_type{1});
        p.insert(value_type{2});
        p.insert(value_type{3});
        REQUIRE(p.size() == 3);
    }
}

TEST_CASE("DomainPIMPL : at") {
    DomainPIMPL p;

    SECTION("Single element"){
        p.insert(value_type{1});
        SECTION("Element 0"){ REQUIRE(p.at(0) == value_type{1}); }
        SECTION("Not in range [0, 1)"){
            REQUIRE_THROWS_AS(p.at(1), std::out_of_range);
        }
    }

    SECTION("Two elements"){
        p.insert(value_type{1});
        p.insert(value_type{2});
        SECTION("Element 0") { REQUIRE(p.at(0) == value_type{1}); }
        SECTION("Element 1") { REQUIRE(p.at(1) == value_type{2}); }
        SECTION("Not in range [0, 2)") {
            REQUIRE_THROWS_AS(p.at(2), std::out_of_range);
        }
    }

    SECTION("Three elements"){
        p.insert(value_type{1});
        p.insert(value_type{2});
        p.insert(value_type{3});
        SECTION("Element 0") { REQUIRE(p.at(0) == value_type{1}); }
        SECTION("Element 1") { REQUIRE(p.at(1) == value_type{2}); }
        SECTION("Element 2") { REQUIRE(p.at(2) == value_type{3}); }
        SECTION("Not in range [0, 3)") {
            REQUIRE_THROWS_AS(p.at(3), std::out_of_range);
        }
    }
}

TEST_CASE("DomainPIMPL : at() const") {
    DomainPIMPL p;

    SECTION("Single element"){
        p.insert(value_type{1});
        SECTION("Element 0") {
            REQUIRE(std::as_const(p).at(0) == value_type{1});
        }
        SECTION("Not in range [0, 1)") {
            REQUIRE_THROWS_AS(std::as_const(p).at(1), std::out_of_range);
        }
    }

    SECTION("Two elements"){
        p.insert(value_type{1});
        p.insert(value_type{2});
        SECTION("Element 0") {
            REQUIRE(std::as_const(p).at(0) == value_type{1});
        }
        SECTION("Element 1") {
            REQUIRE(std::as_const(p).at(1) == value_type{2});
        }
        SECTION("Not in range [0, 2)") {
            REQUIRE_THROWS_AS(std::as_const(p).at(2), std::out_of_range);
        }
    }

    SECTION("Three elements"){
        p.insert(value_type{1});
        p.insert(value_type{2});
        p.insert(value_type{3});
        SECTION("Element 0") {
            REQUIRE(std::as_const(p).at(0) == value_type{1});
        }
        SECTION("Element 1") {
            REQUIRE(std::as_const(p).at(1) == value_type{2});
        }
        SECTION("Element 2") {
            REQUIRE(std::as_const(p).at(2) == value_type{3});
        }
        SECTION("Not in range [0, 3)") {
            REQUIRE_THROWS_AS(std::as_const(p).at(3), std::out_of_range);
        }
    }
}

TEST_CASE("DomainPIMPL : insert"){
    DomainPIMPL p;

    SECTION("Add scalar index"){
        p.insert(value_type{});
        REQUIRE(p.size() == 1);
        REQUIRE(p.rank() == 0);
        REQUIRE(p.at(0) == value_type{});
        SECTION("Throws if rank is greater") {
            REQUIRE_THROWS_AS(p.insert(value_type{1}), std::runtime_error);
        }
    }

    SECTION("Repeated addition of scalar index"){
        p.insert(value_type{});
        p.insert(value_type{});
        REQUIRE(p.size() == 1);
        REQUIRE(p.rank() == 0);
        REQUIRE(p.at(0) == value_type{});
    }

    SECTION("Add vector index"){
        p.insert(value_type{1});

        REQUIRE(p.size() == 1);
        REQUIRE(p.rank() == 1);
        REQUIRE(p.at(0) == value_type{1});
        SECTION("Throws if rank is less") {
            REQUIRE_THROWS_AS(p.insert(value_type{}), std::runtime_error);
        }
        SECTION("Throws if rank is greater"){
            REQUIRE_THROWS_AS(p.insert(value_type{1, 2}), std::runtime_error);
        }

    }

    SECTION("Repeated addition of vector index"){
        p.insert(value_type{1});
        p.insert(value_type{1});

        REQUIRE(p.size() == 1);
        REQUIRE(p.rank() == 1);
        REQUIRE(p.at(0) == value_type{1});
    }

    SECTION("Add two vector indices, lexicographical order"){
        p.insert(value_type{1});
        p.insert(value_type{2});

        REQUIRE(p.size() == 2);
        REQUIRE(p.rank() == 1);
        REQUIRE(p.at(0) == value_type{1});
        REQUIRE(p.at(1) == value_type{2});
    }

    SECTION("Add two vector indices, reverse-lexicographical order"){
        p.insert(value_type{2});
        p.insert(value_type{1});

        REQUIRE(p.size() == 2);
        REQUIRE(p.rank() == 1);
        REQUIRE(p.at(0) == value_type{1});
        REQUIRE(p.at(1) == value_type{2});
    }

    SECTION("Add matrix index"){
        p.insert(value_type{1, 2});

        REQUIRE(p.size() == 1);
        REQUIRE(p.rank() == 2);
        REQUIRE(p.at(0) == value_type{1, 2});
    }

    SECTION("Repeated addition of matrix index"){
        p.insert(value_type{1, 2});
        p.insert(value_type{1, 2});

        REQUIRE(p.size() == 1);
        REQUIRE(p.rank() == 2);
        REQUIRE(p.at(0) == value_type{1, 2});
    }

    SECTION("Add two vector indices, lexicographical order"){
        p.insert(value_type{1, 2});
        p.insert(value_type{2, 3});

        REQUIRE(p.size() == 2);
        REQUIRE(p.rank() == 2);
        REQUIRE(p.at(0) == value_type{1, 2});
        REQUIRE(p.at(1) == value_type{2, 3});
    }

    SECTION("Add two vector indices, reverse-lexicographical order"){
        p.insert(value_type{2, 3});
        p.insert(value_type{1, 2});

        REQUIRE(p.size() == 2);
        REQUIRE(p.rank() == 2);
        REQUIRE(p.at(0) == value_type{1, 2});
        REQUIRE(p.at(1) == value_type{2, 3});
    }
}

TEST_CASE("DomainPIMPL : hash()"){
    DomainPIMPL p;

    SECTION("Empty"){
        auto h = sde::hash_objects(p);
        REQUIRE(h == "50e20c9ba664054fb322f0499e108b5f");
    }

    SECTION("Single element"){
        p.insert(value_type{1});
        auto h = sde::hash_objects(p);
        REQUIRE(h == "34c293a68255c03004e2191c762c1971");
    }

    SECTION("Two elements"){
        p.insert(value_type{1});
        p.insert(value_type{2});
        auto h = sde::hash_objects(p);
        REQUIRE(h == "cc2f0464930f46235156bf80d538061f");
    }
}

TEST_CASE("DomainPIMPL : equality"){
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
        p2.insert(value_type{1});
        REQUIRE_FALSE(p1 == p2);
        REQUIRE(p1 != p2);
    }

    SECTION("Two non-defaults"){
        DomainPIMPL p1, p2;
        p1.insert(value_type{1});

        SECTION("Same contents"){
            p2.insert(value_type{1});
            REQUIRE(p1 == p2);
            REQUIRE_FALSE(p1 != p2);
        }

        SECTION("Different contents"){
            SECTION("Different index"){
                p2.insert(value_type{2});
                REQUIRE_FALSE(p1 == p2);
                REQUIRE(p1 != p2);
            }

            SECTION("Additional elements"){
                p2.insert(value_type{1});
                p2.insert(value_type{2});
                REQUIRE_FALSE(p1 == p2);
                REQUIRE(p1 != p2);
            }

            SECTION("Different rank"){
                p2.insert(value_type{1, 2});
                REQUIRE_FALSE(p1 == p2);
                REQUIRE(p1 != p2);
            }
        }
    }
}