#include "libchemist/sparse_map/detail_/domain_base.hpp"
#include "libchemist/sparse_map/domain.hpp"
#include <catch2/catch.hpp>

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
using namespace libchemist::sparse_map::detail_;

using index_types = std::tuple<ElementIndex, TileIndex>;

TEMPLATE_LIST_TEST_CASE("DomainBase", "", index_types) {
    using derived_t = Domain<TestType>;
    using base_t    = DomainBase<derived_t, TestType>;

    /* Create some instances used throughout the test:
     * - d_empty is a default constructed instance
     * - d0 contains a rank 0 index
     * - d1 contains a rank 1 index
     * - d2 contains a rank 2 index
     * - mf is moved from (and thus has no PIMPL)
     * - temp is used only to make mf
     */
    base_t d_empty, d0{TestType{}}, d1{TestType{1}}, d2{TestType{1, 2}}, mf;
    base_t temp(std::move(mf));

    SECTION("Typedefs") {
        using traits_t = DomainTraits<TestType>;

        SECTION("value_type") {
            using corr_t     = typename traits_t::value_type;
            using value_type = typename base_t::value_type;
            STATIC_REQUIRE(std::is_same_v<value_type, corr_t>);
        }

        SECTION("size_type") {
            using corr_t     = typename traits_t::size_type;
            using size_type = typename base_t::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr_t>);
        }

        SECTION("const_reference") {
            using corr_t          = typename traits_t::const_reference;
            using const_reference = typename base_t::const_reference;
            STATIC_REQUIRE(std::is_same_v<const_reference, corr_t>);
        }

        SECTION("const_iterator") {
            using corr_t   = typename traits_t::template const_iterator<base_t>;
            using const_iterator = typename base_t::const_iterator;
            STATIC_REQUIRE(std::is_same_v<const_iterator, corr_t>);
        }
    }

    SECTION("Default ctor") {
        REQUIRE(d_empty.empty());
        REQUIRE(d_empty.size() == 0);
        REQUIRE(d_empty.rank() == 0);
    }

    SECTION("Initializer list ctor") {
        SECTION("Rank 0") {
            REQUIRE_FALSE(d0.empty());
            REQUIRE(d0.size() == 1);
            REQUIRE(d0.rank() == 0);
        }

        SECTION("Rank 1") {
            REQUIRE_FALSE(d1.empty());
            REQUIRE(d1.size() == 1);
            REQUIRE(d1.rank() == 1);
        }

        SECTION("Rank 2") {
            REQUIRE_FALSE(d2.empty());
            REQUIRE(d2.size() == 1);
            REQUIRE(d2.rank() == 2);
        }

        SECTION("Throws if indices have different ranks"){
            TestType i0{1}, i1{1, 2};
            REQUIRE_THROWS_AS(base_t({i0, i1}), std::runtime_error);
        }
    }

    SECTION("Copy ctor") {
        SECTION("Default Domain") {
            base_t copy(d0);
            REQUIRE(d0 == copy);
        }

        SECTION("Rank 1 index") {
            base_t copy(d1);
            SECTION("value") { REQUIRE(d1 == copy); }
        }

        SECTION("Rank 2 index") {
            base_t copy(d2);
            SECTION("value") { REQUIRE(d2 == copy); }
        }

        SECTION("Moved-from") {
            REQUIRE_THROWS_AS(base_t(mf), std::runtime_error);
        }
    }

    SECTION("Move ctor") {
        SECTION("Default") {
            base_t moved2(std::move(d_empty));
            REQUIRE(moved2 == base_t{});
        }

        SECTION("Rank 0") {
            base_t corr(d0);
            base_t moved2(std::move(d0));
            REQUIRE(corr == moved2);
        }

        SECTION("Rank 1") {
            base_t corr(d1);
            base_t moved2(std::move(d1));
            REQUIRE(corr == moved2);
        }

        SECTION("Rank 2") {
            base_t corr(d2);
            base_t moved2(std::move(d2));
            REQUIRE(corr == moved2);
        }

        SECTION("Moved-from") {
            base_t moved2(std::move(mf));
            REQUIRE(moved2 == mf);
        }
    }

    SECTION("Copy assignment") {
        SECTION("Default Domain") {
            base_t copy;
            auto pcopy = &(copy = d0);
            SECTION("Value"){ REQUIRE(d0 == copy); }
            SECTION("Returns *this"){ REQUIRE(pcopy == &copy); }
        }

        SECTION("Rank 1 index") {
            base_t copy;
            auto pcopy = &(copy = d1);
            SECTION("Value"){ REQUIRE(d1 == copy); }
            SECTION("Returns *this"){ REQUIRE(pcopy == &copy); }
        }

        SECTION("Rank 2 index") {
            base_t copy;
            auto pcopy = &(copy = d2);
            SECTION("Value"){ REQUIRE(d2 == copy); }
            SECTION("Returns *this"){ REQUIRE(pcopy == &copy); }
        }

        SECTION("Moved-from") {
            base_t copy;
            REQUIRE_THROWS_AS(copy = mf, std::runtime_error);
        }
    }

    SECTION("Move assignment") {
        SECTION("Default") {
            base_t moved2;
            auto pmoved2 = &(moved2 = std::move(d_empty));
            SECTION("Value"){ REQUIRE(moved2 == base_t{}); }
            SECTION("Returns *this"){ REQUIRE(pmoved2 == &moved2); }
        }

        SECTION("Rank 0") {
            base_t moved2, corr(d0);
            auto pmoved2 = &(moved2 = std::move(d0));
            SECTION("Value"){ REQUIRE(moved2 == corr); }
            SECTION("Returns *this"){ REQUIRE(pmoved2 == &moved2); }
        }

        SECTION("Rank 1") {
            base_t moved2, corr(d1);
            auto pmoved2 = &(moved2 = std::move(d1));
            SECTION("Value"){ REQUIRE(moved2 == corr); }
            SECTION("Returns *this"){ REQUIRE(pmoved2 == &moved2); }
        }

        SECTION("Rank 2") {
            base_t moved2, corr(d2);
            auto pmoved2 = &(moved2 = std::move(d2));
            SECTION("Value"){ REQUIRE(moved2 == corr); }
            SECTION("Returns *this"){ REQUIRE(pmoved2 == &moved2); }
        }

        SECTION("Moved-from") {
            base_t moved2;
            auto pmoved2 = &(moved2 = std::move(mf));
            SECTION("Value"){ REQUIRE(moved2 == mf); }
            SECTION("Returns *this"){ REQUIRE(pmoved2 == &moved2); }
        }
    }

    SECTION("swap") {
        SECTION("LHS == default") {
            SECTION("RHS == default") {
                base_t rhs;
                d_empty.swap(rhs);
                REQUIRE(d_empty == rhs);
            }

            SECTION("RHS == non-default") {
                d_empty.swap(d0);
                REQUIRE(d_empty == base_t{TestType{}});
                REQUIRE(d0 == base_t{});
            }

            SECTION("RHS == moved-from") {
                base_t corr;
                temp = std::move(corr);
                d_empty.swap(mf);
                REQUIRE(corr == d_empty);
                REQUIRE(mf == base_t{});

            }
        }
    }

    SECTION("begin") {
        using const_iterator = typename base_t::const_iterator;
        SECTION("Default constructed") {
            REQUIRE(d_empty.begin() == const_iterator(0, &d_empty));
        }
        SECTION("Rank 0") { REQUIRE(d0.begin() == const_iterator(0, &d0)); }
        SECTION("Rank 1") { REQUIRE(d1.begin() == const_iterator(0, &d1)); }
        SECTION("Rank 2") { REQUIRE(d2.begin() == const_iterator(0, &d2)); }
        SECTION("Moved from") { REQUIRE(mf.begin() == const_iterator(0, &mf)); }
    }

    SECTION("end") {
        using const_iterator = typename base_t::const_iterator;
        SECTION("Default constructed") {
            REQUIRE(d_empty.end() == const_iterator(0, &d_empty));
        }
        SECTION("Rank 0") { REQUIRE(d0.end() == const_iterator(1, &d0)); }
        SECTION("Rank 1") { REQUIRE(d1.end() == const_iterator(1, &d1)); }
        SECTION("Rank 2") { REQUIRE(d2.end() == const_iterator(1, &d2)); }
        SECTION("Moved from") { REQUIRE(mf.end() == const_iterator(0, &mf)); }
    }

    SECTION("empty") {
        SECTION("Default constructed") { REQUIRE(d_empty.empty()); }
        SECTION("Rank 0") { REQUIRE_FALSE(d0.empty()); }
        SECTION("Rank 1") { REQUIRE_FALSE(d1.empty()); }
        SECTION("Rank 2") { REQUIRE_FALSE(d2.empty()); }
        SECTION("Moved from") { REQUIRE(mf.empty()); }
    }

    SECTION("rank") {
       SECTION("Default") { REQUIRE(d_empty.rank() == 0); }
       SECTION("Rank 0") { REQUIRE(d0.rank() == 0); }
       SECTION("Rank 1") { REQUIRE(d1.rank() == 1); }
       SECTION("Rank 2") { REQUIRE(d2.rank() == 2); }
       SECTION("No PIMPL") { REQUIRE(mf.rank() == 0); }
    }

    SECTION("size") {
        SECTION("Default") { REQUIRE(d_empty.size() == 0); }
        SECTION("Rank 0") { REQUIRE(d0.size() == 1); }
        SECTION("Rank 1") { REQUIRE(d1.size() == 1); }
        SECTION("Rank 2") { REQUIRE(d2.size() == 1); }
        SECTION("No PIMPL") { REQUIRE(mf.size() == 0); }

        SECTION("Two element domain") {
            d2.insert(TestType{2, 3});
            REQUIRE(d2.size() == 2);
        }
    }
}

//
//TEST_CASE("Domain : count") {
//    SECTION("Default"){
//        Domain d;
//        REQUIRE_FALSE(d.count(value_type{0}));
//    }
//
//    SECTION("Non-default"){
//        Domain d{value_type{1}};
//        SECTION("Has value"){ REQUIRE(d.count(value_type{1})); }
//        SECTION("Does not have value"){ REQUIRE_FALSE(d.count(value_type{0})); }
//    }
//
//    SECTION("Moved-from"){
//        Domain d;
//        Domain d2(std::move(d));
//        REQUIRE_FALSE(d.count(value_type{0}));
//    }
//}
//
//

//
//TEST_CASE("Domain : operator[]") {
//    SECTION("Default"){
//        Domain d;
//        REQUIRE_THROWS_AS(d[0], std::out_of_range);
//    }
//
//    SECTION("Non-default"){
//        Domain d{value_type{1}};
//        SECTION("value 0"){ REQUIRE(d[0] == value_type{1}); }
//        SECTION("value 1"){ REQUIRE_THROWS_AS(d[1], std::out_of_range); }
//    }
//
//    SECTION("Moved-from"){
//        Domain d;
//        Domain d2(std::move(d));
//        REQUIRE_THROWS_AS(d[0], std::runtime_error);
//    }
//}
//
//TEST_CASE("Domain : operator[] const") {
//    SECTION("Default"){
//        const Domain d;
//        REQUIRE_THROWS_AS(d[0], std::out_of_range);
//    }
//
//    SECTION("Non-default"){
//        const Domain d{value_type{1}};
//        SECTION("value 0"){ REQUIRE(d[0] == value_type{1}); }
//        SECTION("value 1"){ REQUIRE_THROWS_AS(d[1], std::out_of_range); }
//    }
//
//    SECTION("Moved-from"){
//        Domain d;
//        Domain d2(std::move(d));
//        REQUIRE_THROWS_AS(std::as_const(d)[0], std::runtime_error);
//    }
//}
//
///* insert() is a thin-wrapper around DomainPIMPL::insert. As long as that
// * function works correctly and we properly create a new PIMPL when it is null
// * Domain::insert will work too. Thus we really only need to check that we can
// * insert a value into a normal and a moved-from instance. For good measure we
// * also make sure repeated insertion does not change the container, elements
// * must be the same rank, and that elements are stored in lexicographical order.
// */
//TEST_CASE("Domain : insert") {
//    SECTION("Can insert") {
//        Domain d;
//        d.insert(value_type{1});
//        REQUIRE(d == Domain{value_type{1}});
//    }
//
//    SECTION("Repeated insertion") {
//        Domain d;
//        d.insert(value_type{1});
//        d.insert(value_type{1});
//        REQUIRE(d == Domain{value_type{1}});
//    }
//
//    SECTION("Lexicographical order") {
//        Domain d;
//        d.insert(value_type{2});
//        d.insert(value_type{1});
//        REQUIRE(d[0] == value_type{1});
//        REQUIRE(d[1] == value_type{2});
//    }
//
//    SECTION("Different ranks are an error") {
//        Domain d{value_type{2}};
//        REQUIRE_THROWS_AS(d.insert(value_type{}), std::runtime_error);
//    }
//
//    SECTION("Moved from creates a new PIMPL and works") {
//        Domain d;
//        Domain d2(std::move(d));
//        d.insert(value_type{});
//        REQUIRE(d == Domain{value_type{}});
//    }
//}
//
///* For the tensor product the real work occurs in operator*. operator*= simply
// * calls operator* and swaps the current instance with the returned instance.
// * Thus operator*= will work as long as:
// *
// * - we properly feed the inputs into operator*, and swap
// * - operator* and swap work
// * - we return *this
// *
// * Here we simply test a vector vector product and assert that we get the
// * correct answer and that we return *this. It is assumed that operator*= will
// * work for other products of tensors because operator* works.
// */
//TEST_CASE("Domain : operator*="){
//    Domain d1{value_type{1}, value_type{2}};
//    Domain d2{value_type{3}, value_type{4}};
//
//    SECTION("d1 * d2") {
//        auto pd1 = &(d1 *= d2);
//        SECTION("Returns *this") { REQUIRE(pd1 == &d1); }
//        SECTION("value") {
//            Domain corr{value_type{1, 3}, value_type{1, 4}, value_type{2, 3},
//                        value_type{2, 4}};
//            REQUIRE(corr == d1);
//        }
//    }
//
//    SECTION("d2 * d1") {
//        auto pd2 = &(d2 *= d1);
//        SECTION("Returns *this") { REQUIRE(pd2 == &d2); }
//        SECTION("value") {
//            Domain corr{value_type{3, 1}, value_type{3, 2}, value_type{4, 1},
//                        value_type{4, 2}};
//            REQUIRE(corr == d2);
//        }
//    }
//}
//
///* operator* is the work horse of tensor products for the domain class. Thus
// * this unit test is responsible for making sure it works correctly for tensor
// * products between tensors of arbitrary rank. In particular we check:
// *
// * - scalar-scalar
// * - scalar-vector
// * - scalar-matrix
// * - vector-scalar
// * - vector-vector
// * - vector-matrix
// * - matrix-scalar
// * - matrix-vector
// * - matrix-matrix
// *
// * and note that the tensor product is non-commutative.
// */
//TEST_CASE("Domain : operator*"){
//
//    SECTION("d1 is scalar") {
//        Domain d1{value_type{}};
//
//        SECTION("d2 is scalar") {
//            Domain d2{value_type{}};
//            Domain corr{value_type{}};
//
//            SECTION("d1 * d2") {
//                Domain d12 = d1 * d2;
//                REQUIRE(d12 == corr);
//            }
//
//            SECTION("d2 * d1") {
//                Domain d21 = d2 * d1;
//                REQUIRE(d21 == corr);
//            }
//        }
//
//        SECTION("d2 is vector"){
//            Domain d2{value_type{1}, value_type{2}};
//            Domain corr{value_type{1}, value_type{2}};
//
//            SECTION("d1 * d2"){
//                auto d12 = d1 * d2;
//                REQUIRE(d12 == corr);
//            }
//
//            SECTION("d2 * d1"){
//                auto d21 = d2 * d1;
//                REQUIRE(d21 == corr);
//            }
//        }
//
//        SECTION("d2 is matrix"){
//            Domain d2{value_type{1, 2}, value_type{2, 3}};
//            Domain corr{value_type{1, 2}, value_type{2, 3}};
//
//            SECTION("d1 * d2"){
//                auto d12 = d1 * d2;
//                REQUIRE(d12 == corr);
//            }
//
//            SECTION("d2 * d1"){
//                auto d21 = d2 * d1;
//                REQUIRE(d21 == corr);
//            }
//        }
//    }
//
//    SECTION("d1 is vector") {
//        Domain d1{value_type{1}, value_type{2}};
//
//        SECTION("d2 is vector"){
//            Domain d2{value_type{3}, value_type{4}};
//
//            SECTION("d1 * d2"){
//                Domain corr{value_type{1, 3}, value_type{1, 4},
//                            value_type{2, 3}, value_type{2, 4}};
//
//                auto d12 = d1 * d2;
//                REQUIRE(d12 == corr);
//            }
//
//            SECTION("d2 * d1"){
//                Domain corr{value_type{3, 1}, value_type{3, 2},
//                            value_type{4, 1}, value_type{4, 2}};
//
//                auto d21 = d2 * d1;
//                REQUIRE(d21 == corr);
//            }
//        }
//
//        SECTION("d2 is matrix"){
//            Domain d2{value_type{3, 5}, value_type{4, 6}};
//
//            SECTION("d1 * d2"){
//                Domain corr{value_type{1, 3, 5}, value_type{1, 4, 6},
//                            value_type{2, 3, 5}, value_type{2, 4, 6}};
//
//                auto d12 = d1 * d2;
//                REQUIRE(d12 == corr);
//            }
//
//            SECTION("d2 * d1"){
//                Domain corr{value_type{3, 5, 1}, value_type{3, 5, 2},
//                            value_type{4, 6, 1}, value_type{4, 6, 2}};
//
//                auto d21 = d2 * d1;
//                REQUIRE(d21 == corr);
//            }
//        }
//    }
//
//    SECTION("d1 is matrix") {
//        Domain d1{value_type{1, 2}, value_type{2, 3}};
//
//        SECTION("d2 is matrix"){
//            Domain d2{value_type{3, 4}, value_type{4, 5}};
//
//            SECTION("d1 * d2"){
//                Domain corr{value_type{1, 2, 3, 4}, value_type{1, 2, 4, 5},
//                            value_type{2, 3, 3, 4}, value_type{2, 3, 4, 5}};
//
//                auto d12 = d1 * d2;
//                REQUIRE(d12 == corr);
//            }
//
//            SECTION("d2 * d1"){
//                Domain corr{value_type{3, 4, 1, 2}, value_type{3, 4, 2, 3},
//                            value_type{4, 5, 1, 2}, value_type{4, 5, 2, 3}};
//
//                auto d21 = d2 * d1;
//                REQUIRE(d21 == corr);
//            }
//        }
//    }
//}
//
///* For operator+ and operator+=, operator+= is the work horse. operator+ simply
// * wraps a copy and a call to operator+=. Thus if operator+= and the copy ctor
// * work, operator+ will work too.
// */
//TEST_CASE("Domain : operator+=") {
//    SECTION("LHS = default") {
//        Domain lhs;
//
//        SECTION("RHS = default") {
//            Domain rhs, corr;
//
//            SECTION(" lhs += rhs ") {
//                auto plhs = &(lhs += rhs);
//                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
//                SECTION("value") { REQUIRE(lhs == corr); }
//            }
//
//            SECTION(" rhs += lhs ") {
//                auto prhs = &(rhs += lhs);
//                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
//                SECTION("value") { REQUIRE(rhs == corr); }
//            }
//        }
//
//        SECTION("RHS = scalar") {
//            Domain rhs{value_type{}};
//
//            SECTION("lhs += rhs") {
//                auto plhs = &(lhs += rhs);
//                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
//                SECTION("Value") { REQUIRE(lhs == Domain{value_type{}}); }
//            }
//
//            SECTION("rhs += lhs") {
//                auto prhs = &(rhs += lhs);
//                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
//                SECTION("Value") { REQUIRE(rhs == Domain{value_type{}}); }
//            }
//        }
//
//        SECTION("RHS = vector") {
//            Domain rhs{value_type{1}};
//
//            SECTION("lhs += rhs") {
//                auto plhs = &(lhs += rhs);
//                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
//                SECTION("Value") { REQUIRE(lhs == Domain{value_type{1}}); }
//            }
//
//            SECTION("rhs += lhs") {
//                auto prhs = &(rhs += lhs);
//                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
//                SECTION("Value") { REQUIRE(rhs == Domain{value_type{1}}); }
//            }
//        }
//
//        SECTION("RHS = matrix") {
//            Domain rhs{value_type{1, 2}};
//
//            SECTION("lhs += rhs") {
//                auto plhs = &(lhs += rhs);
//                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
//                SECTION("Value") { REQUIRE(lhs == Domain{value_type{1, 2}}); }
//            }
//
//            SECTION("rhs += lhs") {
//                auto prhs = &(rhs += lhs);
//                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
//                SECTION("Value") { REQUIRE(rhs == Domain{value_type{1, 2}}); }
//            }
//        }
//    }
//
//    SECTION("LHS = scalar") {
//        Domain lhs{value_type{}};
//
//        SECTION("RHS = scalar") {
//            Domain rhs{value_type{}};
//
//            SECTION("lhs += rhs") {
//                auto plhs = &(lhs += rhs);
//                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
//                SECTION("Value") { REQUIRE(lhs == Domain{value_type{}}); }
//            }
//
//            SECTION("rhs += lhs") {
//                auto prhs = &(rhs += lhs);
//                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
//                SECTION("Value") { REQUIRE(rhs == Domain{value_type{}}); }
//            }
//        }
//
//        SECTION("RHS = vector") {
//            Domain rhs{value_type{1}};
//
//            SECTION("lhs += rhs") {
//                REQUIRE_THROWS_AS(lhs += rhs, std::runtime_error);
//            }
//
//            SECTION("rhs += lhs") {
//                REQUIRE_THROWS_AS(rhs += lhs, std::runtime_error);
//            }
//        }
//
//        SECTION("RHS = matrix") {
//            Domain rhs{value_type{1, 2}};
//
//            SECTION("lhs += rhs") {
//                REQUIRE_THROWS_AS(lhs += rhs, std::runtime_error);
//            }
//
//            SECTION("rhs += lhs") {
//                REQUIRE_THROWS_AS(rhs += lhs, std::runtime_error);
//            }
//        }
//    }
//
//    SECTION("LHS = vector") {
//        Domain lhs{value_type{1}};
//
//        SECTION("RHS = vector") {
//            Domain rhs{value_type{2}};
//
//            SECTION("lhs += rhs") {
//                auto plhs = &(lhs += rhs);
//                Domain corr{value_type{1}, value_type{2}};
//                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
//                SECTION("Value") { REQUIRE(lhs == corr); }
//            }
//
//            SECTION("rhs += lhs") {
//                auto prhs = &(rhs += lhs);
//                Domain corr{value_type{1}, value_type{2}};
//                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
//                SECTION("Value") { REQUIRE(rhs == corr); }
//            }
//        }
//
//        SECTION("RHS = matrix") {
//            Domain rhs{value_type{1, 2}};
//
//            SECTION("lhs += rhs") {
//                REQUIRE_THROWS_AS(lhs += rhs, std::runtime_error);
//            }
//
//            SECTION("rhs += lhs") {
//                REQUIRE_THROWS_AS(rhs += lhs, std::runtime_error);
//            }
//        }
//    }
//
//    SECTION("LHS = matrix") {
//        Domain lhs{value_type{1, 2}};
//
//        SECTION("RHS = vector") {
//            Domain rhs{value_type{2, 2}};
//
//            SECTION("lhs += rhs") {
//                auto plhs = &(lhs += rhs);
//                Domain corr{value_type{1, 2}, value_type{2, 2}};
//                SECTION("Returns *this") { REQUIRE(plhs == &lhs); }
//                SECTION("Value") { REQUIRE(lhs == corr); }
//            }
//
//            SECTION("rhs += lhs") {
//                auto prhs = &(rhs += lhs);
//                Domain corr{value_type{1, 2}, value_type{2, 2}};
//                SECTION("Returns *this") { REQUIRE(prhs == &rhs); }
//                SECTION("Value") { REQUIRE(rhs == corr); }
//            }
//        }
//    }
//}
//
//TEST_CASE("Domain : operator+"){
//    Domain d1{value_type{1}}, d2{value_type{2}};
//    Domain d3 = d1 + d2;
//    REQUIRE(d3 == Domain{value_type{1}, value_type{2}});
//}
//
//TEST_CASE("Domain : hash"){
//    SECTION("Default"){
//        Domain d;
//        REQUIRE(sde::hash_objects(d) == "50e20c9ba664054fb322f0499e108b5f");
//    }
//
//    SECTION("non-default"){
//        Domain d{value_type{1}};
//        REQUIRE(sde::hash_objects(d) == "34c293a68255c03004e2191c762c1971");
//    }
//}
//
//TEST_CASE("Domain : print"){
//    SECTION("Default"){
//        Domain d;
//        std::stringstream ss;
//        d.print(ss);
//        REQUIRE(ss.str() == "{}");
//    }
//
//    SECTION("Scalar"){
//        Domain d{value_type{}};
//        std::stringstream ss;
//        d.print(ss);
//        REQUIRE(ss.str() == "{[]}");
//    }
//
//    SECTION("Vector"){
//        Domain d{value_type{1}};
//        std::stringstream ss;
//        d.print(ss);
//        REQUIRE(ss.str() == "{[1]}");
//    }
//
//    SECTION("Matrix"){
//        Domain d{value_type{1, 2}};
//        std::stringstream ss;
//        d.print(ss);
//        REQUIRE(ss.str() == "{[1, 2]}");
//    }
//
//    SECTION("Multiple Elements"){
//        Domain d{value_type{1}, value_type{2}};
//        std::stringstream ss;
//        d.print(ss);
//        REQUIRE(ss.str() == "{[1], [2]}");
//    }
//}
//
//TEST_CASE("Domain : equality"){
//    SECTION("Same instance"){
//        Domain d1;
//        REQUIRE(d1 == d1);
//        REQUIRE_FALSE(d1 != d1);
//    }
//
//    SECTION("Default == Default"){
//        Domain d1, d2;
//        REQUIRE(d1 == d2);
//        REQUIRE_FALSE(d1 != d2);
//    }
//
//    SECTION("Default != Non-Default"){
//        Domain d1, d2{value_type{1}};
//        REQUIRE_FALSE(d1 == d2);
//        REQUIRE(d1 != d2);
//    }
//
//    SECTION("Two non-defaults"){
//        Domain d1{value_type{1}};
//
//        SECTION("Same contents"){
//            Domain d2{value_type{1}};
//            REQUIRE(d1 == d2);
//            REQUIRE_FALSE(d1 != d2);
//        }
//
//        SECTION("Different contents"){
//            SECTION("Different index"){
//                Domain d2{value_type{2}};
//                REQUIRE_FALSE(d1 == d2);
//                REQUIRE(d1 != d2);
//            }
//
//            SECTION("Additional elements"){
//                Domain d2{value_type{1}, value_type{2}};
//                REQUIRE_FALSE(d1 == d2);
//                REQUIRE(d1 != d2);
//            }
//
//            SECTION("Different rank"){
//                Domain d2{value_type{1, 2}};
//                REQUIRE_FALSE(d1 == d2);
//                REQUIRE(d1 != d2);
//            }
//        }
//    }
//
//    SECTION("Moved-from"){
//        Domain d1;
//        Domain d3(std::move(d1));
//
//        SECTION("default"){
//            Domain d2;
//            REQUIRE_FALSE(d1 == d2);
//            REQUIRE(d1 != d2);
//        }
//
//        SECTION("non-default"){
//            Domain d2{value_type{1}};
//            REQUIRE_FALSE(d1 == d2);
//            REQUIRE(d1 != d2);
//        }
//
//        SECTION("moved-from"){
//            Domain d2;
//            d3 = std::move(d2);
//            REQUIRE(d1 == d2);
//            REQUIRE_FALSE(d1 != d2);
//        }
//    }
//}
//
//TEST_CASE("ostream::operator<<(Domain)"){
//    Domain d;
//    std::stringstream ss;
//    ss << d;
//    REQUIRE(ss.str() == "{}");
//}
