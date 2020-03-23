#include <catch2/catch.hpp>
#include <libchemist/sparse_map/sparse_map.hpp>

using namespace libchemist::sparse_map;
using index_type = typename SparseMap::key_type;

TEST_CASE("SparseMap : default ctor") {
    SparseMap sm;
    REQUIRE(sm.size() == 0);
    REQUIRE(sm.ind_rank() == 0);
    REQUIRE(sm.dep_rank() == 0);
    REQUIRE(sde::hash_objects(sm) == "50e20c9ba664054fb322f0499e108b5f");
}

TEST_CASE("SparseMap : copy ctor"){
    SECTION("Empty") {
        SparseMap sm1;
        SparseMap sm2(sm1);
        REQUIRE(sm2 == SparseMap{});
    }

    SECTION("Non-empty"){
        SparseMap sm1{{index_type{}, {index_type{}}}};
        SparseMap sm2(sm1);
        SECTION("Value") { REQUIRE(sm2 == sm1); }
    }
}

TEST_CASE("SparseMap : move ctor"){
    SECTION("Empty") {
        SparseMap sm1;
        SparseMap sm2(std::move(sm1));
        REQUIRE(sm2 == SparseMap{});
    }

    SECTION("Non-empty"){
        SparseMap sm1{{index_type{}, {index_type{}}}};
        SparseMap sm2(std::move(sm1));
        SparseMap corr{{index_type{}, {index_type{}}}};
        SECTION("Value") { REQUIRE(sm2 == corr); }
    }
}

TEST_CASE("SparseMap : copy assignment"){
    SECTION("Empty") {
        SparseMap sm1;
        SparseMap sm2;
        auto psm2 = &(sm2 = sm1);
        SECTION("value") { REQUIRE(sm2 == SparseMap{}); }
        SECTION("returns *this") { REQUIRE(psm2 == &sm2); }
    }

    SECTION("Non-empty"){
        SparseMap sm1{{index_type{}, {index_type{}}}};
        SparseMap sm2;
        auto psm2 = &(sm2 = sm1);
        SECTION("Value") { REQUIRE(sm2 == sm1); }
        SECTION("Returns *this"){ REQUIRE(psm2 == &sm2); }
    }
}

TEST_CASE("SparseMap : move assignment"){
    SECTION("Empty") {
        SparseMap sm1;
        SparseMap sm2;
        auto psm2 = &(sm2 = std::move(sm1));
        SECTION("Value"){ REQUIRE(sm2 == SparseMap{}); }
        SECTION("Returns *this") { REQUIRE(psm2 == &sm2); }
    }

    SECTION("Non-empty"){
        SparseMap sm1{{index_type{}, {index_type{}}}};
        SparseMap sm2;
        auto psm2 = &(sm2 = std::move(sm1));
        SparseMap corr{{index_type{}, {index_type{}}}};
        SECTION("Value") { REQUIRE(sm2 == corr); }
        SECTION("Returns *this"){ REQUIRE(psm2 == &sm2); }
    }
}

/* The initializer_list ctor unit tests assume that default construction
 * followed by repeated calls to add_to_domain work in order to produce the
 * correct state. The tests also assume that equality comparisons work. Assuming
 * that all aforementioned assumptions are met, we only need to test that we
 * can use an initializer list to create a SparseMap between indices of
 * different ranks.
 */
TEST_CASE("SparseMap: initializer_list ctor") {

    SECTION("scalar - scalar") {
        SparseMap sm{{index_type{}, {index_type{}}}};
        SparseMap corr;
        corr.add_to_domain(index_type{}, index_type{});
        REQUIRE(sm == corr);
    }

    SECTION("scalar - vector") {
        SECTION("Single element domain") {
            SparseMap sm{{index_type{}, {index_type{1}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{}, index_type{1});
            REQUIRE(sm == corr);
        }

        SECTION("Multi-element domain") {
            SparseMap sm{{index_type{}, {index_type{1}, index_type{2}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{}, index_type{1});
            corr.add_to_domain(index_type{}, index_type{2});
            REQUIRE(sm == corr);
        }
    }

    SECTION("scalar - matrix") {
        SECTION("Single element domain") {
            SparseMap sm{{index_type{}, {index_type{1, 2}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{}, index_type{1, 2});
            REQUIRE(sm == corr);
        }

        SECTION("Multi-element domain") {
            SparseMap sm{{index_type{}, {index_type{1, 2}, index_type{1, 3}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{}, index_type{1, 2});
            corr.add_to_domain(index_type{}, index_type{1, 3});
            REQUIRE(sm == corr);
        }
    }

    SECTION("vector - scalar") {
        SparseMap sm{{index_type{1}, {index_type{}}}};
        SparseMap corr;
        corr.add_to_domain(index_type{1}, index_type{});
        REQUIRE(sm == corr);
    }

    SECTION("vector - vector") {
        SECTION("Single element domain") {
            SparseMap sm{{index_type{1}, {index_type{2}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1}, index_type{2});
            REQUIRE(sm == corr);
        }

        SECTION("Multi element domain") {
            SparseMap sm{{index_type{1}, {index_type{2}, index_type{3}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1}, index_type{2});
            corr.add_to_domain(index_type{1}, index_type{3});
            REQUIRE(sm == corr);
        }

        SECTION("Multiple independent indices") {
            SparseMap sm{{index_type{1}, {index_type{2}}},
                         {index_type{2}, {index_type{1}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1}, index_type{2});
            corr.add_to_domain(index_type{2}, index_type{1});
            REQUIRE(sm == corr);
        }

        SECTION("Multi everything") {
            SparseMap sm{{index_type{1}, {index_type{2}, index_type{3}}},
                         {index_type{2}, {index_type{2}, index_type{4}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1}, index_type{2});
            corr.add_to_domain(index_type{1}, index_type{3});
            corr.add_to_domain(index_type{2}, index_type{2});
            corr.add_to_domain(index_type{2}, index_type{4});
            REQUIRE(sm == corr);
        }
    }

    SECTION("vector - matrix") {
        SECTION("Single element domain") {
            SparseMap sm{{index_type{1}, {index_type{1, 2}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1}, index_type{1, 2});
            REQUIRE(sm == corr);
        }

        SECTION("Multi-element domain") {
            SparseMap sm{{index_type{1}, {index_type{1, 2}, index_type{1, 3}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1}, index_type{1, 2});
            corr.add_to_domain(index_type{1}, index_type{1, 3});
            REQUIRE(sm == corr);
        }

        SECTION("Multi independent indices") {
            SparseMap sm{{index_type{1}, {index_type{1, 2}}},
                         {index_type{2}, {index_type{2, 3}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1}, index_type{1, 2});
            corr.add_to_domain(index_type{2}, index_type{2, 3});
            REQUIRE(sm == corr);
        }

        SECTION("Multi everything") {
            SparseMap sm{{index_type{1}, {index_type{1, 2}, index_type{1, 3}}},
                         {index_type{2}, {index_type{1, 3}, index_type{2, 4}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1}, index_type{1, 2});
            corr.add_to_domain(index_type{1}, index_type{1, 3});
            corr.add_to_domain(index_type{2}, index_type{1, 3});
            corr.add_to_domain(index_type{2}, index_type{2, 4});
            REQUIRE(sm == corr);
        }
    }

    SECTION("matrix - scalar") {
        SparseMap sm{{index_type{1, 2}, {index_type{}}}};
        SparseMap corr;
        corr.add_to_domain(index_type{1, 2}, index_type{});
        REQUIRE(sm == corr);
    }

    SECTION("matrix - vector") {
        SECTION("Single element domain") {
            SparseMap sm{{index_type{1, 2}, {index_type{1}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1, 2}, index_type{1});
            REQUIRE(sm == corr);
        }

        SECTION("Multi element domain") {
            SparseMap sm{{index_type{1, 2}, {index_type{1}, index_type{2}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1, 2}, index_type{1});
            corr.add_to_domain(index_type{1, 2}, index_type{2});
            REQUIRE(sm == corr);
        }

        SECTION("Multi independent index") {
            SparseMap sm{{index_type{1, 2}, {index_type{1}}},
                         {index_type{1, 3}, {index_type{2}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1, 2}, index_type{1});
            corr.add_to_domain(index_type{1, 3}, index_type{2});
            REQUIRE(sm == corr);
        }

        SECTION("Multi element domain") {
            SparseMap sm{{index_type{1, 2}, {index_type{1}, index_type{2}}},
                         {index_type{1, 3}, {index_type{2}, index_type{3}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1, 2}, index_type{1});
            corr.add_to_domain(index_type{1, 2}, index_type{2});
            corr.add_to_domain(index_type{1, 3}, index_type{2});
            corr.add_to_domain(index_type{1, 3}, index_type{3});
            REQUIRE(sm == corr);
        }
    }

    SECTION("matrix - matrix") {
        SECTION("Single element domain") {
            SparseMap sm{{index_type{1, 2}, {index_type{1, 3}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1, 2}, index_type{1, 3});
            REQUIRE(sm == corr);
        }

        SECTION("Multi element domain") {
            SparseMap sm{
              {index_type{1, 2}, {index_type{1, 3}, index_type{1, 4}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1, 2}, index_type{1, 3});
            corr.add_to_domain(index_type{1, 2}, index_type{1, 4});
            REQUIRE(sm == corr);
        }

        SECTION("Multi independent index") {
            SparseMap sm{{index_type{1, 2}, {index_type{1, 3}}},
                         {index_type{1, 3}, {index_type{1, 4}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1, 2}, index_type{1, 3});
            corr.add_to_domain(index_type{1, 3}, index_type{1, 4});
            REQUIRE(sm == corr);
        }

        SECTION("Multi element domain") {
            SparseMap sm{
              {index_type{1, 2}, {index_type{1, 3}, index_type{1, 4}}},
              {index_type{1, 3}, {index_type{1, 4}, index_type{1, 5}}}};
            SparseMap corr;
            corr.add_to_domain(index_type{1, 2}, index_type{1, 3});
            corr.add_to_domain(index_type{1, 2}, index_type{1, 4});
            corr.add_to_domain(index_type{1, 3}, index_type{1, 4});
            corr.add_to_domain(index_type{1, 3}, index_type{1, 5});
            REQUIRE(sm == corr);
        }
    }
}

TEST_CASE("SparseMap : swap"){
    SECTION("Empty"){
        SparseMap sm1, sm2;
        sm1.swap(sm2);
        REQUIRE(sm1 == SparseMap{});
        REQUIRE(sm2 == SparseMap{});
    }

    SECTION("Empty and non-empty"){
        SparseMap sm1, sm2{{index_type{}, {index_type{}}}};
        SparseMap corr(sm2);
        sm1.swap(sm2);
        REQUIRE(sm2 == SparseMap{});
        REQUIRE(sm1 == corr);
    }

    SECTION("Two non-empty"){
        SparseMap sm1{{index_type{1}, {index_type{}}}};
        SparseMap sm2{{index_type{}, {index_type{}}}};
        SparseMap corr1(sm1), corr2(sm2);
        sm1.swap(sm2);
        REQUIRE(sm2 == corr1);
        REQUIRE(sm1 == corr2);
    }
}

TEST_CASE("SparseMap : size"){
    SECTION("Empty"){
        SparseMap sm;
        REQUIRE(sm.size() == 0);
    }

    SECTION("Single element"){
        SparseMap sm{{index_type{}, {index_type{}}}};
        REQUIRE(sm.size() == 1);
    }

    SECTION("Multiple elements"){
        SparseMap sm{{index_type{1}, {index_type{1}}},
                     {index_type{2}, {index_type{1}}}};
        REQUIRE(sm.size() == 2);
    }
}

TEST_CASE("SparseMap : empty") {
    SECTION("Empty"){
        SparseMap sm;
        REQUIRE(sm.empty());
    }

    SECTION("Non-empty") {
        SparseMap sm{{index_type{1}, {index_type{2}}}};
        REQUIRE_FALSE(sm.empty());
    }
}

TEST_CASE("SparseMap : count") {
    SparseMap sm{{index_type{1}, {index_type{}}}};

    SECTION("Has element") { REQUIRE(sm.count(index_type{1})); }
    SECTION("Does not have element"){
        REQUIRE_FALSE(sm.count(index_type{2}));
    }
}

TEST_CASE("SparseMap : ind_rank"){
    SECTION("Empty"){
        SparseMap sm;
        REQUIRE(sm.ind_rank() == 0);
    }

    SECTION("Scalar"){
        SparseMap sm{{index_type{}, {index_type{}}}};
        REQUIRE(sm.ind_rank() == 0);
    }

    SECTION("Vector"){
        SparseMap sm{{index_type{1}, {index_type{}}}};
        REQUIRE(sm.ind_rank() == 1);
    }

    SECTION("Matrix"){
        SparseMap sm{{index_type{1, 2}, {index_type{}}}};
        REQUIRE(sm.ind_rank() == 2);
    }
}

TEST_CASE("SparseMap : dep_rank"){
    SECTION("Empty"){
        SparseMap sm;
        REQUIRE(sm.dep_rank() == 0);
    }

    SECTION("Scalar"){
        SparseMap sm{{index_type{}, {index_type{}}}};
        REQUIRE(sm.dep_rank() == 0);
    }

    SECTION("Vector"){
        SparseMap sm{{index_type{}, {index_type{1}}}};
        REQUIRE(sm.dep_rank() == 1);
    }

    SECTION("Matrix"){
        SparseMap sm{{index_type{}, {index_type{1, 2}}}};
        REQUIRE(sm.dep_rank() == 2);
    }
}

TEST_CASE("SparseMap : operator[]"){
    SparseMap sm{{index_type{1}, {index_type{1}}}};
    Domain corr{index_type{1}};
    REQUIRE(sm[index_type{1}] == corr);
}

TEST_CASE("SparseMap : operator[] const"){
    const SparseMap sm{{index_type{1}, {index_type{1}}}};
    Domain corr{index_type{1}};
    REQUIRE(sm[index_type{1}] == corr);
}

TEST_CASE("SparseMap : at"){
    SparseMap sm{{index_type{1}, {index_type{1}}}};
    Domain corr{index_type{1}};
    REQUIRE(sm.at(index_type{1}) == corr);
}

TEST_CASE("SparseMap : at const"){
    const SparseMap sm{{index_type{1}, {index_type{1}}}};
    Domain corr{index_type{1}};
    REQUIRE(sm.at(index_type{1}) == corr);
}

/* add_to_domain(index_type, index_type) is the work horse of the overloads and
 * we thus test it the best. The other overloads are simply convenience wrappers
 * to add_to_domain(index_type, index_type) and thus for testing them it
 * suffices to ensure arguments are forwarded correctly.
 */
TEST_CASE("SparseMap : add_to_domain(int, int)"){
    SparseMap sm;
    sm.add_to_domain(0, 0);
    SparseMap corr{{index_type{0}, {index_type{0}}}};
    REQUIRE(sm == corr);
}

TEST_CASE("SparseMap : add_to_domain(int, index_type)"){
    SparseMap sm;

    SECTION("Scalar") {
        sm.add_to_domain(0, index_type{});
        SparseMap corr{{index_type{0}, {index_type{}}}};
        REQUIRE(sm == corr);
    }

    SECTION("Vector") {
        sm.add_to_domain(0, index_type{1});
        SparseMap corr{{index_type{0}, {index_type{1}}}};
        REQUIRE(sm == corr);
    }

    SECTION("Matrix") {
        sm.add_to_domain(0, index_type{1, 2});
        SparseMap corr{{index_type{0}, {index_type{1, 2}}}};
        REQUIRE(sm == corr);
    }
}

TEST_CASE("SparseMap : add_to_domain(index_type, index_type)"){
    SparseMap sm;

    SECTION("Scalar-Scalar") {
        sm.add_to_domain(index_type{}, index_type{});
        SparseMap corr{{index_type{}, {index_type{}}}};
        REQUIRE(sm == corr);
    }

    SECTION("Scalar-Vector") {
        sm.add_to_domain(index_type{}, index_type{1});
        SparseMap corr{{index_type{}, {index_type{1}}}};
        REQUIRE(sm == corr);

        SECTION("Add second element to same domain"){
            sm.add_to_domain(index_type{}, index_type{2});
            SparseMap corr2{{index_type{}, {index_type{1}}},
                            {index_type{}, {index_type{2}}}};
            REQUIRE(sm == corr2);
        }

        SECTION("Add same element to same domain"){
            sm.add_to_domain(index_type{}, index_type{1});
            REQUIRE(sm == corr);
        }
    }

    SECTION("Scalar-Matrix") {
        sm.add_to_domain(index_type{}, index_type{1, 2});
        SparseMap corr{{index_type{}, {index_type{1, 2}}}};
        REQUIRE(sm == corr);

        SECTION("Add second element to same domain"){
            sm.add_to_domain(index_type{}, index_type{1, 3});
            SparseMap corr2{{index_type{}, {index_type{1, 2}}},
                            {index_type{}, {index_type{1, 3}}}};
            REQUIRE(sm == corr2);
        }

        SECTION("Add same element to same domain"){
            sm.add_to_domain(index_type{}, index_type{1, 2});
            REQUIRE(sm == corr);
        }
    }

    SECTION("Vector-Scalar") {
        sm.add_to_domain(index_type{1}, index_type{});
        SparseMap corr{{index_type{1}, {index_type{}}}};
        REQUIRE(sm == corr);
    }

    SECTION("Vector-Vector") {
        sm.add_to_domain(index_type{1}, index_type{2});
        SparseMap corr{{index_type{1}, {index_type{2}}}};
        REQUIRE(sm == corr);

        SECTION("Add second element to same domain"){
            sm.add_to_domain(index_type{1}, index_type{3});
            SparseMap corr2{{index_type{1}, {index_type{2}}},
                            {index_type{1}, {index_type{3}}}};
            REQUIRE(sm == corr2);
        }

        SECTION("Add same element to same domain"){
            sm.add_to_domain(index_type{1}, index_type{2});
            REQUIRE(sm == corr);
        }

        SECTION("Add element to different domain"){
            sm.add_to_domain(index_type{2}, index_type{3});
            SparseMap corr2{{index_type{1}, {index_type{2}}},
                            {index_type{2}, {index_type{3}}}};
            REQUIRE(sm == corr2);
        }
    }

    SECTION("Vector-Matrix") {
        sm.add_to_domain(index_type{1}, index_type{1, 2});
        SparseMap corr{{index_type{1}, {index_type{1, 2}}}};
        REQUIRE(sm == corr);

        SECTION("Add second element to same domain"){
            sm.add_to_domain(index_type{1}, index_type{1, 4});
            SparseMap corr2{{index_type{1}, {index_type{1, 2}}},
                            {index_type{1}, {index_type{1, 4}}}};
            REQUIRE(sm == corr2);
        }

        SECTION("Add same element to same domain"){
            sm.add_to_domain(index_type{1}, index_type{1, 2});
            REQUIRE(sm == corr);
        }

        SECTION("Add element to different domain"){
            sm.add_to_domain(index_type{2}, index_type{1, 4});
            SparseMap corr2{{index_type{1}, {index_type{1, 2}}},
                            {index_type{2}, {index_type{1, 4}}}};
            REQUIRE(sm == corr2);
        }
    }

    SECTION("Matrix-Scalar") {
        sm.add_to_domain(index_type{1, 2}, index_type{});
        SparseMap corr{{index_type{1, 2}, {index_type{}}}};
        REQUIRE(sm == corr);
    }

    SECTION("Matrix-Vector") {
        sm.add_to_domain(index_type{1, 2}, index_type{1});
        SparseMap corr{{index_type{1, 2}, {index_type{1}}}};
        REQUIRE(sm == corr);

        SECTION("Add second element to same domain"){
            sm.add_to_domain(index_type{1, 2}, index_type{2});
            SparseMap corr2{{index_type{1, 2}, {index_type{1}}},
                            {index_type{1, 2}, {index_type{2}}}};
            REQUIRE(sm == corr2);
        }

        SECTION("Add same element to same domain"){
            sm.add_to_domain(index_type{1, 2}, index_type{1});
            REQUIRE(sm == corr);
        }

        SECTION("Add element to different domain"){
            sm.add_to_domain(index_type{1, 3}, index_type{1});
            SparseMap corr2{{index_type{1, 2}, {index_type{1}}},
                            {index_type{1, 3}, {index_type{1}}}};
            REQUIRE(sm == corr2);
        }
    }

    SECTION("Matrix-Matrix") {
        sm.add_to_domain(index_type{1, 2}, index_type{1, 3});
        SparseMap corr{{index_type{1, 2}, {index_type{1, 3}}}};
        REQUIRE(sm == corr);

        SECTION("Add second element to same domain"){
            sm.add_to_domain(index_type{1, 2}, index_type{1, 4});
            SparseMap corr2{{index_type{1, 2}, {index_type{1, 3}}},
                            {index_type{1, 2}, {index_type{1, 4}}}};
            REQUIRE(sm == corr2);
        }

        SECTION("Add same element to same domain"){
            sm.add_to_domain(index_type{1, 2}, index_type{1, 3});
            REQUIRE(sm == corr);
        }

        SECTION("Add element to different domain"){
            sm.add_to_domain(index_type{1, 3}, index_type{1, 4});
            SparseMap corr2{{index_type{1, 2}, {index_type{1, 3}}},
                            {index_type{1, 3}, {index_type{1, 4}}}};
            REQUIRE(sm == corr2);
        }

    }
}

TEST_CASE("SparseMap : add_to_domain(index_type, range)"){
    SparseMap sm;
    Domain d{index_type{1}, index_type{2}};
    sm.add_to_domain(index_type{1}, d.begin(), d.end());
    SparseMap corr;
    corr.add_to_domain(index_type{1}, index_type{1});
    corr.add_to_domain(index_type{1}, index_type{2});
    REQUIRE(sm == corr);
}

/* We know that the Domain::operator* works so we really only need to test that
 * we are properly calling Domain::operator* and that we are saving them
 * under the appropriate keys. This means we need to ensure that the output
 * only contains independent indices in the intersection of the two SparseMaps
 * and that the corresponding domains are the tensor product of the domains. We
 * also note that operator* is the workhorse with operator*= simply calling it
 * and swapping the result.
 *
 */
TEST_CASE("SparseMap : operator*"){
    SECTION("LHS == empty"){
        SparseMap lhs;

        SECTION("RHS == empty"){
            SparseMap rhs;

            SECTION("lhs * rhs") {
                auto r = lhs * rhs;
                REQUIRE(r == lhs);
            }

            SECTION("rhs * lhs") {
                auto r = rhs * lhs;
                REQUIRE(r == lhs);
            }
        }

        SECTION("RHS == non-empty") {
            SparseMap rhs{{index_type{1}, {index_type{2}}}};

            SECTION("lhs * rhs") {
                auto r = lhs * rhs;
                REQUIRE(r == lhs);
            }

            SECTION("rhs * lhs") {
                auto r = rhs * lhs;
                REQUIRE(r == lhs);
            }
        }
    }

    SECTION("LHS == non-empty"){
        SparseMap lhs{{index_type{1}, {index_type{1}}}};

        SECTION("RHS same independent, single element domain"){
            SparseMap rhs{{index_type{1}, {index_type{2}}}};

            SECTION("lhs * rhs") {
                SparseMap corr{{index_type{1}, {index_type{1, 2}}}};
                auto r = lhs * rhs;
                REQUIRE(r == corr);
            }

            SECTION("rhs * lhs") {
                SparseMap corr{{index_type{1}, {index_type{2, 1}}}};
                auto r = rhs * lhs;
                REQUIRE(r == corr);
            }
        }

        SECTION("RHS same independent, two element domain"){
            SparseMap rhs{{index_type{1}, {index_type{2}, index_type{3}}}};

            SECTION("lhs * rhs") {
                SparseMap corr{{index_type{1}, {index_type{1, 2}, index_type{1, 3}}}};
                auto r = lhs * rhs;
                REQUIRE(r == corr);
            }

            SECTION("rhs * lhs") {
                SparseMap corr{{index_type{1}, {index_type{2, 1}, index_type{3, 1}}}};
                auto r = rhs * lhs;
                REQUIRE(r == corr);
            }
        }

        SECTION("RHS different independent, single element domain"){
            SparseMap rhs{{index_type{2}, {index_type{2}}}};

            SECTION("lhs * rhs") {
                auto r = lhs * rhs;
                REQUIRE(r == SparseMap{});
            }

            SECTION("rhs * lhs") {
                auto r = lhs * rhs;
                REQUIRE(r == SparseMap{});
            }
        }

        SECTION("RHS multiple independent"){
            SparseMap rhs{{index_type{1}, {index_type{2}}},
                          {index_type{2}, {index_type{2}}}};

            SECTION("lhs * rhs") {
                auto r = lhs * rhs;
                SparseMap corr{{index_type{1}, {index_type{1, 2}}}};
                REQUIRE(r == corr);
            }

            SECTION("rhs * lhs") {
                auto r = rhs * lhs;
                SparseMap corr{{index_type{1}, {index_type{2, 1}}}};
                REQUIRE(r == corr);
            }
        }
    }
}

TEST_CASE("SparseMap : operator*="){
    SparseMap lhs{{index_type{1}, {index_type{1}}}};
    SparseMap rhs{{index_type{1}, {index_type{2}}},
                  {index_type{2}, {index_type{2}}}};

    auto plhs = &(lhs *= rhs);
    SECTION("return *this"){ REQUIRE(plhs == &lhs); }
    SECTION("value") {
        SparseMap corr{{index_type{1}, {index_type{1, 2}}}};
        REQUIRE(lhs == corr);
    }
}

TEST_CASE("SparseMap : equality"){
    SECTION("LHS == Default"){
        SparseMap lhs;

        SECTION("RHS == Default"){
            SparseMap rhs;
            REQUIRE(lhs == rhs);
            REQUIRE_FALSE(lhs != rhs);
        }

        SECTION("RHS == non-default"){
            SparseMap rhs{{index_type{}, {index_type{}}}};
            REQUIRE_FALSE(lhs == rhs);
            REQUIRE(lhs != rhs);
        }
    }

    SECTION("LHS == Non-Default"){
        SparseMap lhs{{index_type{1}, {index_type{1}}}};

        SECTION("RHS == Default"){
            SparseMap rhs;
            REQUIRE_FALSE(lhs == rhs);
            REQUIRE(lhs != rhs);
        }

        SECTION("RHS == non-default") {
            SECTION("Same") {
                SparseMap rhs{{index_type{1}, {index_type{1}}}};
                REQUIRE(lhs == rhs);
                REQUIRE_FALSE(lhs != rhs);
            }

            SECTION("Different independent index"){
                SparseMap rhs{{index_type{2}, {index_type{1}}}};
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }

            SECTION("Different independent rank"){
                SparseMap rhs{{index_type{1, 2}, {index_type{1}}}};
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }

            SECTION("Different dependent index"){
                SparseMap rhs{{index_type{1}, {index_type{2}}}};
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }

            SECTION("Different dependent rank"){
                SparseMap rhs{{index_type{1}, {index_type{1, 2}}}};
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }

            SECTION("Additional element in domain"){
                SparseMap rhs{{index_type{1}, {index_type{1}, index_type{2}}}};
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }

            SECTION("Additional independent index"){
                SparseMap rhs{{index_type{1}, {index_type{1}}},
                              {index_type{2}, {index_type{2}}}};
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }
        }
    }
}

TEST_CASE("SparseMap : hash"){
    SECTION("Empty"){
        SparseMap sm;
        REQUIRE(sde::hash_objects(sm) == "50e20c9ba664054fb322f0499e108b5f");
    }

    SECTION("Non-empty"){
        SparseMap sm{{index_type{}, {index_type{}}}};
        REQUIRE(sde::hash_objects(sm) == "64d6c4b037c182902ce14d4fabe15ce2");
    }

}

TEST_CASE("SparseMap : print"){
    std::stringstream ss;

    SECTION("empty") {
        SparseMap sm;
        sm.print(ss);
        std::string corr = "{}";
        REQUIRE(ss.str() == corr);
    }

//    SECTION("Non-empty") {
//        SparseMap sm{{index_type{1}, {index_type{1}}}};
//        sm.print(ss);
//        std::string corr = "{([1] : {[1]})}";
//        REQUIRE(ss.str() == corr);
//    }
}