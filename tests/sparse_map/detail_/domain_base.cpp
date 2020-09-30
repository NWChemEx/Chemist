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

    TestType i0, i1{1}, i2{1, 2};

    /* Create some instances used throughout the test:
     * - d_empty is a default constructed instance
     * - d0 contains a rank 0 index
     * - d1 contains a rank 1 index
     * - d2 contains a rank 2 index
     * - mf is moved from (and thus has no PIMPL)
     * - temp is used only to make mf
     */
    base_t d_empty, d0{i0}, d1{i1}, d2{i2}, mf;
    base_t temp(std::move(mf));

    SECTION("Typedefs") {
        using traits_t = DomainTraits<TestType>;

        SECTION("value_type") {
            using corr_t     = typename traits_t::value_type;
            using value_type = typename base_t::value_type;
            STATIC_REQUIRE(std::is_same_v<value_type, corr_t>);
        }

        SECTION("size_type") {
            using corr_t    = typename traits_t::size_type;
            using size_type = typename base_t::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr_t>);
        }

        SECTION("const_reference") {
            using corr_t          = typename traits_t::const_reference;
            using const_reference = typename base_t::const_reference;
            STATIC_REQUIRE(std::is_same_v<const_reference, corr_t>);
        }

        SECTION("const_iterator") {
            using corr_t = typename traits_t::template const_iterator<base_t>;
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

        SECTION("Throws if indices have different ranks") {
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
            SECTION("Value") { REQUIRE(d0 == copy); }
            SECTION("Returns *this") { REQUIRE(pcopy == &copy); }
        }

        SECTION("Rank 1 index") {
            base_t copy;
            auto pcopy = &(copy = d1);
            SECTION("Value") { REQUIRE(d1 == copy); }
            SECTION("Returns *this") { REQUIRE(pcopy == &copy); }
        }

        SECTION("Rank 2 index") {
            base_t copy;
            auto pcopy = &(copy = d2);
            SECTION("Value") { REQUIRE(d2 == copy); }
            SECTION("Returns *this") { REQUIRE(pcopy == &copy); }
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
            SECTION("Value") { REQUIRE(moved2 == base_t{}); }
            SECTION("Returns *this") { REQUIRE(pmoved2 == &moved2); }
        }

        SECTION("Rank 0") {
            base_t moved2, corr(d0);
            auto pmoved2 = &(moved2 = std::move(d0));
            SECTION("Value") { REQUIRE(moved2 == corr); }
            SECTION("Returns *this") { REQUIRE(pmoved2 == &moved2); }
        }

        SECTION("Rank 1") {
            base_t moved2, corr(d1);
            auto pmoved2 = &(moved2 = std::move(d1));
            SECTION("Value") { REQUIRE(moved2 == corr); }
            SECTION("Returns *this") { REQUIRE(pmoved2 == &moved2); }
        }

        SECTION("Rank 2") {
            base_t moved2, corr(d2);
            auto pmoved2 = &(moved2 = std::move(d2));
            SECTION("Value") { REQUIRE(moved2 == corr); }
            SECTION("Returns *this") { REQUIRE(pmoved2 == &moved2); }
        }

        SECTION("Moved-from") {
            base_t moved2;
            auto pmoved2 = &(moved2 = std::move(mf));
            SECTION("Value") { REQUIRE(moved2 == mf); }
            SECTION("Returns *this") { REQUIRE(pmoved2 == &moved2); }
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
                REQUIRE(d_empty == base_t{i0});
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

    SECTION("count") {
        SECTION("Default") { REQUIRE_FALSE(d_empty.count(i1)); }

        SECTION("Rank 0") {
            SECTION("Has value") { REQUIRE(d0.count(i0)); }
            SECTION("Does not have value") { REQUIRE_FALSE(d0.count(i1)); }
        }

        SECTION("Rank 1") {
            SECTION("Has value") { REQUIRE(d1.count(i1)); }
            SECTION("Does not have value") { REQUIRE_FALSE(d1.count(i0)); }
        }

        SECTION("Rank 2") {
            SECTION("Has value") { REQUIRE(d2.count(i2)); }
            SECTION("Does not have value") { REQUIRE_FALSE(d2.count(i1)); }
        }

        SECTION("Moved-from") { REQUIRE_FALSE(mf.count(i0)); }
    }

    SECTION("operator[]") {
        SECTION("Default") { REQUIRE_THROWS_AS(d_empty[0], std::out_of_range); }

        SECTION("Rank 0") {
            SECTION("value 0") { REQUIRE(d0[0] == i0); }
            SECTION("value 1") { REQUIRE_THROWS_AS(d0[1], std::out_of_range); }
        }

        SECTION("Rank 1") {
            SECTION("value 0") { REQUIRE(d1[0] == i1); }
            SECTION("value 1") { REQUIRE_THROWS_AS(d1[1], std::out_of_range); }
        }

        SECTION("Rank 2") {
            SECTION("value 0") { REQUIRE(d2[0] == i2); }
            SECTION("value 1") { REQUIRE_THROWS_AS(d2[1], std::out_of_range); }
        }

        SECTION("Has more than one value") {
            TestType i34{3, 4};
            d2.insert(i34);
            SECTION("value 0") { REQUIRE(d2[0] == i2); }
            SECTION("value 1") { REQUIRE(d2[1] == i34); }
            SECTION("Value 2") { REQUIRE_THROWS_AS(d2[2], std::out_of_range); }
        }

        SECTION("Moved-from") { REQUIRE_THROWS_AS(mf[0], std::runtime_error); }
    }

    /* insert() is a thin-wrapper around DomainPIMPL::insert. As long as that
     * function works correctly and we properly create a new PIMPL when it is
     * null
     * Domain::insert will work too. Thus we really only need to check that we
     * can
     * insert a value into a normal and a moved-from instance. For good measure
     * we
     * also make sure repeated insertion does not change the container, elements
     * must be the same rank, and that elements are stored in lexicographical
     * order.
     */
    SECTION("insert") {
        SECTION("Can insert") {
            d_empty.insert(i1);
            REQUIRE(d_empty == d1);
        }

        SECTION("Repeated insertion") {
            d1.insert(i1);
            REQUIRE(d1 == base_t{i1});
        }

        SECTION("Stored in Lexicographical order") {
            d1.insert(TestType{0});
            REQUIRE(d1[0] == TestType{0});
            REQUIRE(d1[1] == i1);
        }

        SECTION("Different ranks are an error") {
            REQUIRE_THROWS_AS(d1.insert(i0), std::runtime_error);
        }

        SECTION("Moved from creates a new PIMPL and works") {
            mf.insert(i0);
            REQUIRE(mf == d0);
        }
    }

    /* operator*= is the work horse of tensor products for the domain class.
     * Thus this unit test is responsible for making sure it works correctly for
     * tensor products between tensors of arbitrary rank and we note that the
     * tensor product is non-commutative.
     */
    SECTION("operator*=") {
        SECTION("LHS is default") {
            SECTION("RHS is default") {
                auto plhs = &(d_empty *= d_empty);
                SECTION("Value") { REQUIRE(d_empty == base_t{}); }
                SECTION("Returns *this") { REQUIRE(&d_empty == plhs); }
            }

            SECTION("RHS is rank 0") {
                auto plhs = &(d_empty *= d0);
                SECTION("Value") { REQUIRE(d_empty == base_t{}); }
                SECTION("Returns *this") { REQUIRE(&d_empty == plhs); }
            }

            SECTION("RHS is rank 1") {
                auto plhs = &(d_empty *= d1);
                SECTION("Value") { REQUIRE(d_empty == base_t{}); }
                SECTION("Returns *this") { REQUIRE(&d_empty == plhs); }
            }

            SECTION("RHS is rank 2") {
                auto plhs = &(d_empty *= d2);
                SECTION("Value") { REQUIRE(d_empty == base_t{}); }
                SECTION("Returns *this") { REQUIRE(&d_empty == plhs); }
            }

            SECTION("RHS is moved-from") {
                auto plhs = &(d_empty *= mf);
                SECTION("Value") { REQUIRE(d_empty == base_t{}); }
                SECTION("Returns *this") { REQUIRE(&d_empty == plhs); }
            }
        }

        SECTION("LHS is scalar") {
            SECTION("RHS is default") {
                auto plhs = &(d0 *= d_empty);
                SECTION("Value") { REQUIRE(d0 == d_empty); }
                SECTION("Returns *this") { REQUIRE(&d0 == plhs); }
            }

            SECTION("RHS is rank 0") {
                auto plhs = &(d0 *= d0);
                SECTION("Value") { REQUIRE(d0 == base_t{i0}); }
                SECTION("Returns *this") { REQUIRE(&d0 == plhs); }
            }

            SECTION("RHS is rank 1") {
                auto plhs = &(d0 *= d1);
                SECTION("Value") { REQUIRE(d0 == d1); }
                SECTION("Returns *this") { REQUIRE(&d0 == plhs); }
            }

            SECTION("RHS is rank 2") {
                auto plhs = &(d0 *= d2);
                SECTION("Value") { REQUIRE(d0 == d2); }
                SECTION("Returns *this") { REQUIRE(&d0 == plhs); }
            }

            SECTION("RHS is moved-from") {
                auto plhs = &(d0 *= mf);
                SECTION("Value") { REQUIRE(d0 == d_empty); }
                SECTION("Returns *this") { REQUIRE(&d0 == plhs); }
            }
        }

        SECTION("LHS is rank 1") {
            SECTION("RHS is default") {
                auto plhs = &(d1 *= d_empty);
                SECTION("Value") { REQUIRE(d1 == d_empty); }
                SECTION("Returns *this") { REQUIRE(&d1 == plhs); }
            }

            SECTION("RHS is rank 0") {
                auto plhs = &(d1 *= d0);
                SECTION("Value") { REQUIRE(d1 == base_t{i1}); }
                SECTION("Returns *this") { REQUIRE(&d1 == plhs); }
            }

            SECTION("RHS is rank 1") {
                auto plhs = &(d1 *= d1);
                SECTION("Value") { REQUIRE(d1 == base_t{TestType{1, 1}}); }
                SECTION("Returns *this") { REQUIRE(&d1 == plhs); }
            }

            SECTION("RHS is rank 2") {
                auto plhs = &(d1 *= d2);
                SECTION("Value") { REQUIRE(d1 == base_t{TestType{1, 1, 2}}); }
                SECTION("Returns *this") { REQUIRE(&d1 == plhs); }
            }

            SECTION("RHS is moved-from") {
                auto plhs = &(d1 *= mf);
                SECTION("Value") { REQUIRE(d1 == d_empty); }
                SECTION("Returns *this") { REQUIRE(&d1 == plhs); }
            }
        }

        SECTION("LHS is rank 2") {
            SECTION("RHS is default") {
                auto plhs = &(d2 *= d_empty);
                SECTION("Value") { REQUIRE(d2 == d_empty); }
                SECTION("Returns *this") { REQUIRE(&d2 == plhs); }
            }

            SECTION("RHS is rank 0") {
                auto plhs = &(d2 *= d0);
                SECTION("Value") { REQUIRE(d2 == base_t{i2}); }
                SECTION("Returns *this") { REQUIRE(&d2 == plhs); }
            }

            SECTION("RHS is rank 1") {
                auto plhs = &(d2 *= d1);
                SECTION("Value") { REQUIRE(d2 == base_t{TestType{1, 2, 1}}); }
                SECTION("Returns *this") { REQUIRE(&d2 == plhs); }
            }

            SECTION("RHS is rank 2") {
                auto plhs = &(d2 *= d2);
                SECTION("Value") { REQUIRE(d2 == base_t{TestType{1, 2, 1, 2}}); }
                SECTION("Returns *this") { REQUIRE(&d2 == plhs); }
            }

            SECTION("RHS is moved-from") {
                auto plhs = &(d2 *= mf);
                SECTION("Value") { REQUIRE(d2 == d_empty); }
                SECTION("Returns *this") { REQUIRE(&d2 == plhs); }
            }
        }

        SECTION("LHS is moved from") {
            SECTION("RHS is default") {
                auto plhs = &(mf *= d_empty);
                SECTION("Value") { REQUIRE(mf == d_empty); }
                SECTION("Returns *this") { REQUIRE(&mf == plhs); }
            }

            SECTION("RHS is rank 0") {
                auto plhs = &(mf *= d0);
                SECTION("Value") { REQUIRE(mf == d_empty); }
                SECTION("Returns *this") { REQUIRE(&mf == plhs); }
            }

            SECTION("RHS is rank 1") {
                auto plhs = &(mf *= d1);
                SECTION("Value") { REQUIRE(mf == d_empty); }
                SECTION("Returns *this") { REQUIRE(&mf == plhs); }
            }

            SECTION("RHS is rank 2") {
                auto plhs = &(mf *= d2);
                SECTION("Value") { REQUIRE(mf == d_empty); }
                SECTION("Returns *this") { REQUIRE(&mf == plhs); }
            }

            SECTION("RHS is moved-from") {
                auto plhs = &(mf *= mf);
                SECTION("Value") { REQUIRE(mf == d_empty); }
                SECTION("Returns *this") { REQUIRE(&mf == plhs); }
            }
        }
    } // SECTION("operator*=")

    /* For the Cartesian product the real work occurs in operator*=. operator*
     * simply calls operator*= on a copy. Thus operator* will work as long as:
     *
     * - we properly copy and feed the inputs into operator*=
     * - operator*= and copying work
     *
     * Here we simply test a single Cartesian product and assert that we get the
     * correct answer. It is assumed that operator* will work for other products
     * of tensors because operator*= works.
     */
    SECTION("operator*") {
       auto r = d1 * d2;
       REQUIRE(r == base_t{TestType{1, 1, 2}});
    }

    /* For operator+ and operator+=, operator+= is the work horse. operator+ simply
     * wraps a copy and a call to operator+=. Thus if operator+= and the copy ctor
     * work, operator+ will work too.
     */
    SECTION("operator+="){
        SECTION("LHS == empty"){
            SECTION("RHS == empty") {
                auto plhs = &(d_empty += d_empty);
                SECTION("Value") { REQUIRE(d_empty == base_t{}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d_empty); }
            }

            SECTION("RHS == rank 0") {
                auto plhs = &(d_empty += d0);
                SECTION("Value") { REQUIRE(d_empty == d0); }
                SECTION("Returns *this") { REQUIRE(plhs == &d_empty); }
            }

            SECTION("RHS == rank 1") {
                auto plhs = &(d_empty += d1);
                SECTION("Value") { REQUIRE(d_empty == d1); }
                SECTION("Returns *this") { REQUIRE(plhs == &d_empty); }
            }

            SECTION("RHS == rank 2") {
                auto plhs = &(d_empty += d2);
                SECTION("Value") { REQUIRE(d_empty == d2); }
                SECTION("Returns *this") { REQUIRE(plhs == &d_empty); }
            }

            SECTION("RHS == no PIMPL") {
                auto plhs = &(d_empty += mf);
                SECTION("Value") { REQUIRE(d_empty == base_t{}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d_empty); }
            }
        }

        SECTION("LHS == rank 0"){
            SECTION("RHS == empty") {
                auto plhs = &(d0 += d_empty);
                SECTION("Value") { REQUIRE(d0 == base_t{i0}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d0); }
            }

            SECTION("RHS == rank 0") {
                auto plhs = &(d0 += d0);
                SECTION("Value") { REQUIRE(d0 == base_t{i0}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d0); }
            }

            SECTION("RHS == rank 1") {
                REQUIRE_THROWS_AS(d0 += d1, std::runtime_error);
            }

            SECTION("RHS == rank 2") {
                REQUIRE_THROWS_AS(d0 += d2, std::runtime_error);
            }

            SECTION("RHS == no PIMPL") {
                auto plhs = &(d0 += mf);
                SECTION("Value") { REQUIRE(d0 == base_t{i0}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d0); }
            }
        }

        SECTION("LHS == rank 1"){
            SECTION("RHS == empty") {
                auto plhs = &(d1 += d_empty);
                SECTION("Value") { REQUIRE(d1 == base_t{i1}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d1); }
            }

            SECTION("RHS == rank 0") {
                REQUIRE_THROWS_AS(d1 += d0, std::runtime_error);
            }

            SECTION("RHS == rank 1") {
                auto plhs = &(d1 += d1);
                SECTION("Value") { REQUIRE(d1 == base_t{i1}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d1); }
            }

            SECTION("RHS == rank 2") {
                REQUIRE_THROWS_AS(d1 += d2, std::runtime_error);
            }

            SECTION("RHS == no PIMPL") {
                auto plhs = &(d1 += mf);
                SECTION("Value") { REQUIRE(d1 == base_t{i1}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d1); }
            }
        }

        SECTION("LHS == rank 2"){
            SECTION("RHS == empty") {
                auto plhs = &(d2 += d_empty);
                SECTION("Value") { REQUIRE(d2 == base_t{i2}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d2); }
            }

            SECTION("RHS == rank 0") {
                REQUIRE_THROWS_AS(d2 += d0, std::runtime_error);
            }

            SECTION("RHS == rank 1") {
                REQUIRE_THROWS_AS(d2 += d1, std::runtime_error);
            }

            SECTION("RHS == rank 2") {
                auto plhs = &(d2 += d2);
                SECTION("Value") { REQUIRE(d2 == base_t{i2}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d2); }
            }

            SECTION("RHS == no PIMPL") {
                auto plhs = &(d2 += mf);
                SECTION("Value") { REQUIRE(d2 == base_t{i2}); }
                SECTION("Returns *this") { REQUIRE(plhs == &d2); }
            }
        }

        SECTION("LHS == No PIMPL"){
            SECTION("RHS == empty") {
                auto plhs = &(mf += d_empty);
                SECTION("Value") { REQUIRE(mf == d_empty); }
                SECTION("Returns *this") { REQUIRE(plhs == &mf); }
            }

            SECTION("RHS == rank 0") {
                auto plhs = &(mf += d0);
                SECTION("Value") { REQUIRE(mf == d0); }
                SECTION("Returns *this") { REQUIRE(plhs == &mf); }
            }

            SECTION("RHS == rank 1") {
                auto plhs = &(mf += d1);
                SECTION("Value") { REQUIRE(mf == d1); }
                SECTION("Returns *this") { REQUIRE(plhs == &mf); }
            }

            SECTION("RHS == rank 2") {
                auto plhs = &(mf += d2);
                SECTION("Value") { REQUIRE(mf == d2); }
                SECTION("Returns *this") { REQUIRE(plhs == &mf); }
            }

            SECTION("RHS == No PIMPL") {
                auto plhs = &(mf += mf);
                SECTION("Value") { REQUIRE(mf == d_empty); }
                SECTION("Returns *this") { REQUIRE(plhs == &mf); }
            }
        }
    } // SECTION("operator+=")

    SECTION("operator+"){
        base_t d{TestType{2}};
        auto r = d1 + d;
        REQUIRE(r == base_t{i1, TestType{2}});
    }

    SECTION("Comparisons") {
        SECTION("LHS == empty") {
            SECTION("RHS == empty") {
                REQUIRE(d_empty == base_t{});
                REQUIRE_FALSE(d_empty != base_t{});
            }

            SECTION("RHS == rank 0") {
                REQUIRE_FALSE(d_empty == d0);
                REQUIRE(d_empty != d0);
            }

            SECTION("RHS == rank 1") {
                REQUIRE_FALSE(d_empty == d1);
                REQUIRE(d_empty != d1);
            }

            SECTION("RHS == rank 2") {
                REQUIRE_FALSE(d_empty == d2);
                REQUIRE(d_empty != d2);
            }

            SECTION("RHS == No PIMPL") {
                REQUIRE_FALSE(d_empty == mf);
                REQUIRE(d_empty != mf);
            }
        }

        SECTION("LHS == rank 0") {
            SECTION("RHS == empty") {
                REQUIRE_FALSE(d0 == d_empty);
                REQUIRE(d0 != d_empty);
            }

            SECTION("RHS == rank 0") {
                REQUIRE(d0 == base_t{i0});
                REQUIRE_FALSE(d0 != base_t{i0});
            }

            SECTION("RHS == rank 1") {
                REQUIRE_FALSE(d0 == d1);
                REQUIRE(d0 != d1);
            }

            SECTION("RHS == rank 2") {
                REQUIRE_FALSE(d0 == d2);
                REQUIRE(d0 != d2);
            }

            SECTION("RHS == No PIMPL") {
                REQUIRE_FALSE(d0 == mf);
                REQUIRE(d0 != mf);
            }
        }

        SECTION("LHS == rank 1") {
            SECTION("RHS == empty") {
                REQUIRE_FALSE(d1 == d_empty);
                REQUIRE(d1 != d_empty);
            }

            SECTION("RHS == rank 0") {
                REQUIRE_FALSE(d1 == d0);
                REQUIRE(d1 != d0);
            }

            SECTION("RHS == rank 1") {
                SECTION("Same values") {
                    REQUIRE(d1 == base_t{i1});
                    REQUIRE_FALSE(d1 != base_t{i1});
                }

                SECTION("Different values"){
                    base_t d{TestType{2}};
                    REQUIRE_FALSE(d1 == d);
                    REQUIRE(d1 != d);
                }
            }

            SECTION("RHS == rank 2") {
                REQUIRE_FALSE(d1 == d2);
                REQUIRE(d1 != d2);
            }

            SECTION("RHS == No PIMPL") {
                REQUIRE_FALSE(d1 == mf);
                REQUIRE(d1 != mf);
            }
        }

        SECTION("LHS == rank 1") {
            SECTION("RHS == empty") {
                REQUIRE_FALSE(d2 == d_empty);
                REQUIRE(d2 != d_empty);
            }

            SECTION("RHS == rank 0") {
                REQUIRE_FALSE(d2 == d0);
                REQUIRE(d2 != d0);
            }

            SECTION("RHS == rank 1") {
                REQUIRE_FALSE(d2 == d1);
                REQUIRE(d2 != d1);
            }

            SECTION("RHS == rank 2") {
                SECTION("Same values") {
                    REQUIRE(d2 == base_t{i2});
                    REQUIRE_FALSE(d2 != base_t{i2});
                }

                SECTION("Different values"){
                    base_t d{TestType{2, 1}};
                    REQUIRE_FALSE(d2 == d);
                    REQUIRE(d2 != d);
                }
            }

            SECTION("RHS == No PIMPL") {
                REQUIRE_FALSE(d2 == mf);
                REQUIRE(d2 != mf);
            }
        }

        SECTION("LHS == No PIMPL") {
            SECTION("RHS == empty") {
                REQUIRE_FALSE(mf == d_empty);
                REQUIRE(mf != d_empty);
            }

            SECTION("RHS == rank 0") {
                REQUIRE_FALSE(mf == d0);
                REQUIRE(mf != d0);
            }

            SECTION("RHS == rank 1") {
                REQUIRE_FALSE(mf == d1);
                REQUIRE(mf != d1);
            }

            SECTION("RHS == rank 2") {
                REQUIRE_FALSE(mf == d2);
                REQUIRE(mf != d2);
            }

            SECTION("RHS == No PIMPL") {
                base_t d;
                temp = std::move(d);
                REQUIRE(mf == d);
                REQUIRE_FALSE(mf != d);
            }
        }
    }

    SECTION("hash") {
        SECTION("LHS == empty") {
            auto h = sde::hash_objects(d_empty);
            SECTION("RHS == empty") {
                REQUIRE(h == sde::hash_objects(base_t{}));
            }

            SECTION("RHS == rank 0") { REQUIRE(h != sde::hash_objects(d0)); }
            SECTION("RHS == rank 1") { REQUIRE(h != sde::hash_objects(d1)); }
            SECTION("RHS == rank 2") { REQUIRE(h != sde::hash_objects(d2)); }
        }

        SECTION("LHS == rank 0") {
            auto h = sde::hash_objects(d0);
            SECTION("RHS == empty") {
                REQUIRE(h != sde::hash_objects(d_empty));
            }
            SECTION("RHS == rank 0") {
                REQUIRE(h == sde::hash_objects(base_t{i0}));
            }
            SECTION("RHS == rank 1") { REQUIRE(h != sde::hash_objects(d1)); }
            SECTION("RHS == rank 2") { REQUIRE(h != sde::hash_objects(d2)); }
        }

        SECTION("LHS == rank 1") {
            auto h = sde::hash_objects(d1);
            SECTION("RHS == empty") {
                REQUIRE(h != sde::hash_objects(d_empty));
            }
            SECTION("RHS == rank 0") { REQUIRE(h != sde::hash_objects(d0)); }
            SECTION("RHS == rank 1") {
                SECTION("Same") { REQUIRE(h == sde::hash_objects(base_t{i1})); }
                SECTION("Different") {
                    REQUIRE(h != sde::hash_objects(base_t{TestType{2}}));
                }
            }
            SECTION("RHS == rank 2") { REQUIRE(h != sde::hash_objects(d2)); }
        }

        SECTION("LHS == rank 2") {
            auto h = sde::hash_objects(d2);
            SECTION("RHS == empty") {
                REQUIRE(h != sde::hash_objects(d_empty));
            }
            SECTION("RHS == rank 0") { REQUIRE(h != sde::hash_objects(d0)); }
            SECTION("RHS == rank 1") { REQUIRE(h != sde::hash_objects(d1)); }
            SECTION("RHS == rank 2") {
                SECTION("Same") { REQUIRE(h == sde::hash_objects(base_t{i2})); }
                SECTION("Different") {
                    REQUIRE(h != sde::hash_objects(base_t{TestType{2, 1}}));
                }
            }
        }

        SECTION("LHS == No PIMPL") {
            REQUIRE_THROWS_AS(sde::hash_objects(mf), std::runtime_error);
        }
    } // SECTION("hash")

    SECTION("print") {
        std::stringstream ss;
        SECTION("Default") {
            auto pss = &(d_empty.print(ss));
            SECTION("Value") { REQUIRE(ss.str() == "{}"); }
            SECTION("Returns os") { REQUIRE(pss == &ss); }
        }

        SECTION("Rank 0") {
            auto pss = &(d0.print(ss));
            SECTION("Value") { REQUIRE(ss.str() == "{{}}"); }
            SECTION("Returns os") { REQUIRE(pss == &ss); }
        }

        SECTION("Rank 1") {
            auto pss = &(d1.print(ss));
            SECTION("Value") { REQUIRE(ss.str() == "{{1}}"); }
            SECTION("Returns os") { REQUIRE(pss == &ss); }
        }

        SECTION("Rank 2") {
            auto pss = &(d2.print(ss));
            SECTION("Value") { REQUIRE(ss.str() == "{{1, 2}}"); }
            SECTION("Returns os") { REQUIRE(pss == &ss); }
        }

        SECTION("Multiple Elements") {
            d1.insert(TestType{2});
            auto pss = &(d1.print(ss));
            SECTION("Value") { REQUIRE(ss.str() == "{{1}, {2}}"); }
            SECTION("Returns os") { REQUIRE(pss == &ss); }
        }
    }

    //operator<< just calls print, so if print works operator<< will work too
    SECTION("ostream::operator<<(Domain)"){
        std::stringstream ss;
        auto pss = &(ss << d_empty);
        SECTION("Value") { REQUIRE(ss.str() == "{}"); }
        SECTION("Returns os") { REQUIRE(pss == &ss); }
    }
} // TEMPLATE_LIST_TEST_CASE("DomainBase")



