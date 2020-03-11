#include <catch2/catch.hpp>
#include <libchemist/sparse_map/domain.hpp>

/* Testing strategy.
 *
 * We know that the PIMPL works from unit testing, thus for many cases we simply
 * need to test that we forward the arguments correctly (and check for a
 * non-null PIMPL). The Domain class adds some additional features onto the
 * PIMPL, which go through the PIMPL's public API. These features need to be
 * checked more thoroughly and include:
 *
 * - tensor product (i.e. operator*)
 */


using namespace libchemist::sparse_map;
using value_type = typename Domain::value_type;

TEST_CASE("Domain : default ctor"){
    Domain d;

    REQUIRE(d.empty());
    REQUIRE(d.size() == 0);
    REQUIRE(d.rank() == 0);
    REQUIRE(sde::hash_objects(d) == "50e20c9ba664054fb322f0499e108b5f");
}

TEST_CASE("Domain : initializer_list ctor"){
    SECTION("Scalar type") {
        Domain d{value_type{}};
        Domain corr;
        corr.insert(value_type{});
        REQUIRE(d == corr);
    }

    SECTION("vector type") {
        Domain d{value_type{1}, value_type{2}};
        Domain corr;
        corr.insert(value_type{1});
        corr.insert(value_type{2});
        REQUIRE(d == corr);
    }

    SECTION("matrix type") {
        Domain d{value_type{1, 2}, value_type{2, 3}};
        Domain corr;
        corr.insert(value_type{1, 2});
        corr.insert(value_type{2, 3});
        REQUIRE(d == corr);
    }
}

TEST_CASE("Domain : operator*"){

    SECTION("d1 is scalar") {
        Domain d1{value_type{}};

        SECTION("d2 is scalar") {
            Domain d2{value_type{}};
            Domain corr{value_type{}};

            SECTION("d1 * d2") {
                Domain d12 = d1 * d2;
                REQUIRE(d12 == corr);
            }

            SECTION("d2 * d1") {
                Domain d21 = d2 * d1;
                REQUIRE(d21 == corr);
            }
        }

        SECTION("d2 is vector"){
            Domain d2{value_type{1}, value_type{2}};
            Domain corr{value_type{1}, value_type{2}};

            SECTION("d1 * d2"){
                auto d12 = d1 * d2;
                REQUIRE(d12 == corr);
            }

            SECTION("d2 * d1"){
                auto d21 = d2 * d1;
                REQUIRE(d21 == corr);
            }
        }

        SECTION("d2 is matrix"){
            Domain d2{value_type{1, 2}, value_type{2, 3}};
            Domain corr{value_type{1, 2}, value_type{2, 3}};

            SECTION("d1 * d2"){
                auto d12 = d1 * d2;
                REQUIRE(d12 == corr);
            }

            SECTION("d2 * d1"){
                auto d21 = d2 * d1;
                REQUIRE(d21 == corr);
            }
        }
    }

    SECTION("d1 is vector") {
        Domain d1{value_type{1}, value_type{2}};

        SECTION("d2 is vector"){
            Domain d2{value_type{3}, value_type{4}};

            SECTION("d1 * d2"){
                Domain corr{value_type{1, 3}, value_type{1, 4},
                            value_type{2, 3}, value_type{2, 4}};

                auto d12 = d1 * d2;
                REQUIRE(d12 == corr);
            }

            SECTION("d2 * d1"){
                Domain corr{value_type{3, 1}, value_type{3, 2},
                            value_type{4, 1}, value_type{4, 2}};

                auto d21 = d2 * d1;
                REQUIRE(d21 == corr);
            }
        }

        SECTION("d2 is matrix"){
            Domain d2{value_type{3, 5}, value_type{4, 6}};

            SECTION("d1 * d2"){
                Domain corr{value_type{1, 3, 5}, value_type{1, 4, 6},
                            value_type{2, 3, 5}, value_type{2, 4, 6}};

                auto d12 = d1 * d2;
                REQUIRE(d12 == corr);
            }

            SECTION("d2 * d1"){
                Domain corr{value_type{3, 5, 1}, value_type{3, 5, 2},
                            value_type{4, 6, 1}, value_type{4, 6, 2}};

                auto d21 = d2 * d1;
                REQUIRE(d21 == corr);
            }
        }
    }

    SECTION("d1 is matrix") {
        Domain d1{value_type{1, 2}, value_type{2, 3}};

        SECTION("d2 is matrix"){
            Domain d2{value_type{3, 4}, value_type{4, 5}};

            SECTION("d1 * d2"){
                Domain corr{value_type{1, 2, 3, 4}, value_type{1, 2, 4, 5},
                            value_type{2, 3, 3, 4}, value_type{2, 3, 4, 5}};

                auto d12 = d1 * d2;
                REQUIRE(d12 == corr);
            }

            SECTION("d2 * d1"){
                Domain corr{value_type{3, 4, 1, 2}, value_type{3, 4, 2, 3},
                            value_type{4, 5, 1, 2}, value_type{4, 5, 2, 3}};

                auto d21 = d2 * d1;
                REQUIRE(d21 == corr);
            }
        }
    }
}