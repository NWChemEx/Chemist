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

/* The default ctor makes an empty domain. We check that all accessible state
 * is what we think it is.
 *
 */
TEST_CASE("Domain : default ctor"){
    Domain d;

    REQUIRE(d.empty());
    REQUIRE(d.size() == 0);
    REQUIRE(d.rank() == 0);
    REQUIRE(sde::hash_objects(d) == "50e20c9ba664054fb322f0499e108b5f");
}

TEST_CASE("Domain : copy ctor") {
    SECTION("Default Domain") {
        Domain d1;
        Domain d2(d1);
        REQUIRE(d1 == d2);
    }

    SECTION("Non-default") {
        Domain d1{value_type{1, 2}};
        Domain d2(d1);
        SECTION("value") { REQUIRE(d1 == d2); }
        SECTION("deep copy") { REQUIRE(&d1[0] != &d2[0]); }
    }

    SECTION("Moved-from") {
        Domain d1{value_type{1, 2}};
        Domain d2(std::move(d1));
        REQUIRE_THROWS_AS(Domain(d1), std::runtime_error);
    }

}

TEST_CASE("Domain : move ctor") {
    SECTION("Default Domain") {
        Domain d1;
        Domain d2(std::move(d1));
        REQUIRE(Domain{} == d2);
    }

    SECTION("Non-default") {
        Domain d1{value_type{1, 2}};
        Domain d2(std::move(d1));
        Domain corr{value_type{1, 2}};
        SECTION("value") { REQUIRE(corr == d2); }
    }

    SECTION("Moved-from") {
        Domain d1{value_type{1, 2}};
        Domain d2(std::move(d1));
        Domain corr{value_type{1, 2}};
        REQUIRE(d2 == corr);
    }

}

TEST_CASE("Domain : copy assignment") {
    SECTION("Default Domain") {
        Domain d1;
        Domain d2;
        auto pd2 = &(d2 = d1);
        SECTION("value") { REQUIRE(d1 == d2); }
        SECTION("returns *this") { REQUIRE(pd2 == &d2); }
    }

    SECTION("Non-default") {
        Domain d1{value_type{1, 2}};
        Domain d2;
        auto pd2 = &(d2 = d1);
        SECTION("value") { REQUIRE(d1 == d2); }
        SECTION("deep copy") { REQUIRE(&d1[0] != &d2[0]); }
        SECTION("returns *this"){ REQUIRE(pd2 == &d2); }
    }

    SECTION("Moved-from") {
        Domain d1;
        Domain d3(std::move(d1));
        Domain d2;
        REQUIRE_THROWS_AS(d2 = d1, std::runtime_error);
    }

}

TEST_CASE("Domain : move assignment") {
    SECTION("Default Domain") {
        Domain d1;
        Domain d2;
        auto pd2 = &(d2 = std::move(d1));
        SECTION("value"){ REQUIRE(Domain{} == d2); }
        SECTION("returns *this"){ REQUIRE(pd2 == &d2); }
    }

    SECTION("Non-default") {
        Domain d1{value_type{1, 2}};
        Domain d2;
        auto pd2 = &(d2 = std::move(d1));
        Domain corr{value_type{1, 2}};
        SECTION("value") { REQUIRE(corr == d2); }
        SECTION("returns *this") { REQUIRE(pd2 == &d2); }
    }

    SECTION("Moved-from") {
        Domain d1{value_type{1, 2}};
        Domain d2;
        auto pd2 = &(d2 = std::move(d1));
        Domain corr{value_type{1, 2}};
        SECTION("value"){ REQUIRE(d2 == corr); }
        SECTION("returns *this"){ REQUIRE(pd2 == &d2); }
    }

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

TEST_CASE("Domain : swap") {
    SECTION("Default and non-default") {
        Domain d1, d2{value_type{1}};
        SECTION("d1.swap(d2)") {
            d1.swap(d2);
            REQUIRE(d1 == Domain{value_type{1}});
            REQUIRE(d2 == Domain{});
        }

        SECTION("d2.swap(d1)") {
            d2.swap(d1);
            REQUIRE(d1 == Domain{value_type{1}});
            REQUIRE(d2 == Domain{});
        }
    }

    SECTION("Default and moved-from") {
        Domain d1, d2, corr;
        Domain d3(std::move(d2));
        d3 = std::move(corr);
        SECTION("d1.swap(d2)") {
            d1.swap(d2);
            REQUIRE(d1 == corr);
            REQUIRE(d2 == Domain{});
        }

        SECTION("d2.swap(d1)") {
            d2.swap(d1);
            REQUIRE(d1 == corr);
            REQUIRE(d2 == Domain{});
        }
    }

    SECTION("Two non-default") {
        Domain d1{value_type{1, 2}}, d2{value_type{1}};
        SECTION("d1.swap(d2)") {
            d1.swap(d2);
            REQUIRE(d1 == Domain{value_type{1}});
            REQUIRE(d2 == Domain{value_type{1, 2}});
        }

        SECTION("d2.swap(d1)") {
            d2.swap(d1);
            REQUIRE(d1 == Domain{value_type{1}});
            REQUIRE(d2 == Domain{value_type{1, 2}});
        }
    }

    SECTION("Non-Default and moved-from") {
        Domain d1{value_type{1}}, d2, corr;
        Domain d3(std::move(d2));
        d3 = std::move(corr);
        SECTION("d1.swap(d2)") {
            d1.swap(d2);
            REQUIRE(d1 == corr);
            REQUIRE(d2 == Domain{value_type{1}});
        }

        SECTION("d2.swap(d1)") {
            d2.swap(d1);
            REQUIRE(d1 == corr);
            REQUIRE(d2 == Domain{value_type{1}});
        }
    }

    SECTION("Two moved-from") {
        Domain d1, d2, corr;
        Domain d3(std::move(d1));
        d3 = std::move(d2);
        d3 = std::move(corr);
        SECTION("d1.swap(d2)") {
            d1.swap(d2);
            REQUIRE(d1 == corr);
            REQUIRE(d2 == corr);
        }

        SECTION("d2.swap(d1)") {
            d2.swap(d1);
            REQUIRE(d1 == corr);
            REQUIRE(d2 == corr);
        }
    }
}

TEST_CASE("Domain : begin") {
    using iterator = typename Domain::iterator;
    SECTION("Default"){
        Domain d;
        REQUIRE(d.begin() == iterator(0, &d));
    }

    SECTION("Non-default"){
        Domain d{value_type{1}};
        REQUIRE(d.begin() == iterator(0, &d));
    }

    SECTION("Moved-from"){
        Domain d;
        Domain d2(std::move(d));
        REQUIRE(d.begin() == iterator(0, &d));
    }
}

TEST_CASE("Domain : begin() const") {
    using iterator = typename Domain::const_iterator;
    SECTION("Default"){
        const Domain d;
        REQUIRE(d.begin() == iterator(0, &d));
    }

    SECTION("Non-default"){
        const Domain d{value_type{1}};
        REQUIRE(d.begin() == iterator(0, &d));
    }

    SECTION("Moved-from"){
        Domain d;
        Domain d2(std::move(d));
        REQUIRE(std::as_const(d).begin() == iterator(0, &std::as_const(d)));
    }
}

TEST_CASE("Domain : end") {
    using iterator = typename Domain::iterator;
    SECTION("Default"){
        Domain d;
        REQUIRE(d.end() == iterator(0, &d));
    }

    SECTION("Non-default"){
        Domain d{value_type{1}};
        REQUIRE(d.end() == iterator(1, &d));
    }

    SECTION("Moved-from"){
        Domain d;
        Domain d2(std::move(d));
        REQUIRE(d.end() == iterator(0, &d));
    }
}

TEST_CASE("Domain : end() const") {
    using iterator = typename Domain::const_iterator;
    SECTION("Default"){
        const Domain d;
        REQUIRE(d.end() == iterator(0, &d));
    }

    SECTION("Non-default"){
        const Domain d{value_type{1}};
        REQUIRE(d.end() == iterator(1, &d));
    }

    SECTION("Moved-from"){
        Domain d;
        Domain d2(std::move(d));
        REQUIRE(std::as_const(d).end() == iterator(0, &std::as_const(d)));
    }
}

TEST_CASE("Domain : count") {
    SECTION("Default"){
        Domain d;
        REQUIRE_FALSE(d.count(value_type{0}));
    }

    SECTION("Non-default"){
        Domain d{value_type{1}};
        SECTION("Has value"){ REQUIRE(d.count(value_type{1})); }
        SECTION("Does not have value"){ REQUIRE_FALSE(d.count(value_type{0})); }
    }

    SECTION("Moved-from"){
        Domain d;
        Domain d2(std::move(d));
        REQUIRE_FALSE(d.count(value_type{0}));
    }
}

TEST_CASE("Domain : empty") {
    SECTION("Default"){
        Domain d;
        REQUIRE(d.empty());
    }

    SECTION("Non-default"){
        Domain d{value_type{1}};
        REQUIRE_FALSE(d.empty());
    }

    SECTION("Moved-from"){
        Domain d;
        Domain d2(std::move(d));
        REQUIRE(d.empty());
    }
}

TEST_CASE("Domain : rank") {
    SECTION("Default"){
        Domain d;
        REQUIRE(d.rank() == 0);
    }

    SECTION("scalar"){
        Domain d{value_type{}};
        REQUIRE(d.rank() == 0);
    }

    SECTION("vector"){
        Domain d{value_type{1}};
        REQUIRE(d.rank() == 1);
    }

    SECTION("matrix"){
        Domain d{value_type{1, 2}};
        REQUIRE(d.rank() == 2);
    }

    SECTION("Moved-from"){
        Domain d;
        Domain d2(std::move(d));
        REQUIRE_THROWS_AS(d.rank(), std::runtime_error);
    }
}

TEST_CASE("Domain : size") {
    SECTION("Default"){
        Domain d;
        REQUIRE(d.size() == 0);
    }

    SECTION("Non-default"){
        Domain d{value_type{1}};
        REQUIRE(d.size() == 1);
    }

    SECTION("Moved-from"){
        Domain d;
        Domain d2(std::move(d));
        REQUIRE(d.size() == 0);
    }
}

TEST_CASE("Domain : operator[]") {
    SECTION("Default"){
        Domain d;
        REQUIRE_THROWS_AS(d[0], std::out_of_range);
    }

    SECTION("Non-default"){
        Domain d{value_type{1}};
        SECTION("value 0"){ REQUIRE(d[0] == value_type{1}); }
        SECTION("value 1"){ REQUIRE_THROWS_AS(d[1], std::out_of_range); }
    }

    SECTION("Moved-from"){
        Domain d;
        Domain d2(std::move(d));
        REQUIRE_THROWS_AS(d[0], std::runtime_error);
    }
}

TEST_CASE("Domain : operator[] const") {
    SECTION("Default"){
        const Domain d;
        REQUIRE_THROWS_AS(d[0], std::out_of_range);
    }

    SECTION("Non-default"){
        const Domain d{value_type{1}};
        SECTION("value 0"){ REQUIRE(d[0] == value_type{1}); }
        SECTION("value 1"){ REQUIRE_THROWS_AS(d[1], std::out_of_range); }
    }

    SECTION("Moved-from"){
        Domain d;
        Domain d2(std::move(d));
        REQUIRE_THROWS_AS(std::as_const(d)[0], std::runtime_error);
    }
}

/* insert() is a thin-wrapper around DomainPIMPL::insert. As long as that
 * function works correctly and we properly create a new PIMPL when it is null
 * Domain::insert will work too. Thus we really only need to check that we can
 * insert a value into a normal and a moved-from instance. For good measure we
 * also make sure repeated insertion does not change the container, elements
 * must be the same rank, and that elements are stored in lexicographical order.
 */
TEST_CASE("Domain : insert") {
    SECTION("Can insert") {
        Domain d;
        d.insert(value_type{1});
        REQUIRE(d == Domain{value_type{1}});
    }

    SECTION("Repeated insertion") {
        Domain d;
        d.insert(value_type{1});
        d.insert(value_type{1});
        REQUIRE(d == Domain{value_type{1}});
    }

    SECTION("Lexicographical order") {
        Domain d;
        d.insert(value_type{2});
        d.insert(value_type{1});
        REQUIRE(d[0] == value_type{1});
        REQUIRE(d[1] == value_type{2});
    }

    SECTION("Different ranks are an error") {
        Domain d{value_type{2}};
        REQUIRE_THROWS_AS(d.insert(value_type{}), std::runtime_error);
    }

    SECTION("Moved from creates a new PIMPL and works") {
        Domain d;
        Domain d2(std::move(d));
        d.insert(value_type{});
        REQUIRE(d == Domain{value_type{}});
    }
}

/* For the tensor product the real work occurs in operator*. operator*= simply
 * calls operator* and swaps the current instance with the returned instance.
 * Thus operator*= will work as long as:
 *
 * - we properly feed the inputs into operator*, and swap
 * - operator* and swap work
 * - we return *this
 *
 * Here we simply test a vector vector product and assert that we get the
 * correct answer and that we return *this. It is assumed that operator*= will
 * work for other products of tensors because operator* works.
 */
TEST_CASE("Domain : operator*="){
    Domain d1{value_type{1}, value_type{2}};
    Domain d2{value_type{3}, value_type{4}};

    SECTION("d1 * d2") {
        auto pd1 = &(d1 *= d2);
        SECTION("Returns *this") { REQUIRE(pd1 == &d1); }
        SECTION("value") {
            Domain corr{value_type{1, 3}, value_type{1, 4}, value_type{2, 3},
                        value_type{2, 4}};
            REQUIRE(corr == d1);
        }
    }

    SECTION("d2 * d1") {
        auto pd2 = &(d2 *= d1);
        SECTION("Returns *this") { REQUIRE(pd2 == &d2); }
        SECTION("value") {
            Domain corr{value_type{3, 1}, value_type{3, 2}, value_type{4, 1},
                        value_type{4, 2}};
            REQUIRE(corr == d2);
        }
    }
}

/* operator* is the work horse of tensor products for the domain class. Thus
 * this unit test is responsible for making sure it works correctly for tensor
 * products between tensors of arbitrary rank. In particular we check:
 *
 * - scalar-scalar
 * - scalar-vector
 * - scalar-matrix
 * - vector-scalar
 * - vector-vector
 * - vector-matrix
 * - matrix-scalar
 * - matrix-vector
 * - matrix-matrix
 *
 * and note that the tensor product is non-commutative.
 */
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

/* For operator+ and operator+=, operator+= is the work horse. operator+ simply
 * wraps a copy and a call to operator+=. Thus if operator+= and the copy ctor
 * work, operator+ will work too.
 */
TEST_CASE("Domain : operator+=") {
    SECTION("LHS = default") {
        Domain lhs;

        SECTION("RHS = default") {
            Domain rhs, corr;

            SECTION(" lhs += rhs ") {
                auto plhs = &(lhs += rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("value") { REQUIRE(lhs == corr); }
            }

            SECTION(" rhs += lhs ") {
                auto prhs = &(rhs += lhs);
                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                SECTION("value") { REQUIRE(rhs == corr); }
            }
        }

        SECTION("RHS = scalar") {
            Domain rhs{value_type{}};

            SECTION("lhs += rhs") {
                auto plhs = &(lhs += rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == Domain{value_type{}}); }
            }

            SECTION("rhs += lhs") {
                auto prhs = &(rhs += lhs);
                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                SECTION("Value") { REQUIRE(rhs == Domain{value_type{}}); }
            }
        }

        SECTION("RHS = vector") {
            Domain rhs{value_type{1}};

            SECTION("lhs += rhs") {
                auto plhs = &(lhs += rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == Domain{value_type{1}}); }
            }

            SECTION("rhs += lhs") {
                auto prhs = &(rhs += lhs);
                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                SECTION("Value") { REQUIRE(rhs == Domain{value_type{1}}); }
            }
        }

        SECTION("RHS = matrix") {
            Domain rhs{value_type{1, 2}};

            SECTION("lhs += rhs") {
                auto plhs = &(lhs += rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == Domain{value_type{1, 2}}); }
            }

            SECTION("rhs += lhs") {
                auto prhs = &(rhs += lhs);
                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                SECTION("Value") { REQUIRE(rhs == Domain{value_type{1, 2}}); }
            }
        }
    }

    SECTION("LHS = scalar") {
        Domain lhs{value_type{}};

        SECTION("RHS = scalar") {
            Domain rhs{value_type{}};

            SECTION("lhs += rhs") {
                auto plhs = &(lhs += rhs);
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == Domain{value_type{}}); }
            }

            SECTION("rhs += lhs") {
                auto prhs = &(rhs += lhs);
                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                SECTION("Value") { REQUIRE(rhs == Domain{value_type{}}); }
            }
        }

        SECTION("RHS = vector") {
            Domain rhs{value_type{1}};

            SECTION("lhs += rhs") {
                REQUIRE_THROWS_AS(lhs += rhs, std::runtime_error);
            }

            SECTION("rhs += lhs") {
                REQUIRE_THROWS_AS(rhs += lhs, std::runtime_error);
            }
        }

        SECTION("RHS = matrix") {
            Domain rhs{value_type{1, 2}};

            SECTION("lhs += rhs") {
                REQUIRE_THROWS_AS(lhs += rhs, std::runtime_error);
            }

            SECTION("rhs += lhs") {
                REQUIRE_THROWS_AS(rhs += lhs, std::runtime_error);
            }
        }
    }

    SECTION("LHS = vector") {
        Domain lhs{value_type{1}};

        SECTION("RHS = vector") {
            Domain rhs{value_type{2}};

            SECTION("lhs += rhs") {
                auto plhs = &(lhs += rhs);
                Domain corr{value_type{1}, value_type{2}};
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == corr); }
            }

            SECTION("rhs += lhs") {
                auto prhs = &(rhs += lhs);
                Domain corr{value_type{1}, value_type{2}};
                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                SECTION("Value") { REQUIRE(rhs == corr); }
            }
        }

        SECTION("RHS = matrix") {
            Domain rhs{value_type{1, 2}};

            SECTION("lhs += rhs") {
                REQUIRE_THROWS_AS(lhs += rhs, std::runtime_error);
            }

            SECTION("rhs += lhs") {
                REQUIRE_THROWS_AS(rhs += lhs, std::runtime_error);
            }
        }
    }

    SECTION("LHS = matrix") {
        Domain lhs{value_type{1, 2}};

        SECTION("RHS = vector") {
            Domain rhs{value_type{2, 2}};

            SECTION("lhs += rhs") {
                auto plhs = &(lhs += rhs);
                Domain corr{value_type{1, 2}, value_type{2, 2}};
                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
                SECTION("Value") { REQUIRE(lhs == corr); }
            }

            SECTION("rhs += lhs") {
                auto prhs = &(rhs += lhs);
                Domain corr{value_type{1, 2}, value_type{2, 2}};
                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
                SECTION("Value") { REQUIRE(rhs == corr); }
            }
        }
    }
}

TEST_CASE("Domain : operator+"){
    Domain d1{value_type{1}}, d2{value_type{2}};
    Domain d3 = d1 + d2;
    REQUIRE(d3 == Domain{value_type{1}, value_type{2}});
}

TEST_CASE("Domain : hash"){
    SECTION("Default"){
        Domain d;
        REQUIRE(sde::hash_objects(d) == "50e20c9ba664054fb322f0499e108b5f");
    }

    SECTION("non-default"){
        Domain d{value_type{1}};
        REQUIRE(sde::hash_objects(d) == "34c293a68255c03004e2191c762c1971");
    }
}

TEST_CASE("Domain : print"){
    SECTION("Default"){
        Domain d;
        std::stringstream ss;
        d.print(ss);
        REQUIRE(ss.str() == "{}");
    }

    SECTION("Scalar"){
        Domain d{value_type{}};
        std::stringstream ss;
        d.print(ss);
        REQUIRE(ss.str() == "{[]}");
    }

    SECTION("Vector"){
        Domain d{value_type{1}};
        std::stringstream ss;
        d.print(ss);
        REQUIRE(ss.str() == "{[1]}");
    }

    SECTION("Matrix"){
        Domain d{value_type{1, 2}};
        std::stringstream ss;
        d.print(ss);
        REQUIRE(ss.str() == "{[1, 2]}");
    }

    SECTION("Multiple Elements"){
        Domain d{value_type{1}, value_type{2}};
        std::stringstream ss;
        d.print(ss);
        REQUIRE(ss.str() == "{[1], [2]}");
    }
}

TEST_CASE("Domain : equality"){
    SECTION("Same instance"){
        Domain d1;
        REQUIRE(d1 == d1);
        REQUIRE_FALSE(d1 != d1);
    }

    SECTION("Default == Default"){
        Domain d1, d2;
        REQUIRE(d1 == d2);
        REQUIRE_FALSE(d1 != d2);
    }

    SECTION("Default != Non-Default"){
        Domain d1, d2{value_type{1}};
        REQUIRE_FALSE(d1 == d2);
        REQUIRE(d1 != d2);
    }

    SECTION("Two non-defaults"){
        Domain d1{value_type{1}};

        SECTION("Same contents"){
            Domain d2{value_type{1}};
            REQUIRE(d1 == d2);
            REQUIRE_FALSE(d1 != d2);
        }

        SECTION("Different contents"){
            SECTION("Different index"){
                Domain d2{value_type{2}};
                REQUIRE_FALSE(d1 == d2);
                REQUIRE(d1 != d2);
            }

            SECTION("Additional elements"){
                Domain d2{value_type{1}, value_type{2}};
                REQUIRE_FALSE(d1 == d2);
                REQUIRE(d1 != d2);
            }

            SECTION("Different rank"){
                Domain d2{value_type{1, 2}};
                REQUIRE_FALSE(d1 == d2);
                REQUIRE(d1 != d2);
            }
        }
    }

    SECTION("Moved-from"){
        Domain d1;
        Domain d3(std::move(d1));

        SECTION("default"){
            Domain d2;
            REQUIRE_FALSE(d1 == d2);
            REQUIRE(d1 != d2);
        }

        SECTION("non-default"){
            Domain d2{value_type{1}};
            REQUIRE_FALSE(d1 == d2);
            REQUIRE(d1 != d2);
        }

        SECTION("moved-from"){
            Domain d2;
            d3 = std::move(d2);
            REQUIRE(d1 == d2);
            REQUIRE_FALSE(d1 != d2);
        }
    }
}

TEST_CASE("ostream::operator<<(Domain)"){
    Domain d;
    std::stringstream ss;
    ss << d;
    REQUIRE(ss.str() == "{}");
}