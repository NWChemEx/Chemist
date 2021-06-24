#include "libchemist/basis_set/basis_set.hpp"
#include "libchemist/orbital_space/derived_space.hpp"
#include "libchemist/ta_helpers/ta_helpers.hpp"
#include <catch2/catch.hpp>

/* For testing purposes we assume:
 *
 * - The actual identity/state of the from space does not impact the
 *   DerivedSpace class aside from needing to have a slot in the ctor and be
 *   accessible through the DerivedSpace members. i.e. as long as that type is
 *   tested it'll work here too.
 *
 * - The base class more-or-less passes through the DerivedSpace class. i.e.,
 *   if the base class works, it'll work when used with DerivedSpace too.
 *
 *   - The caveats are that we need to make sure it's accounted for in the ctor,
 *     hash, and comparison operations.
 */

using namespace libchemist;
using namespace libchemist::orbital_space;

TEST_CASE("DerivedSpace") {
    using scalar_type = double;
    using space_type  = DerivedSpaceD;
    using tensor_type = libchemist::type::tensor<scalar_type>;
    using from_space  = AOSpaceD;
    using base_space  = BaseSpace;

    SECTION("Typedefs") {
        SECTION("transform_type") {
            using transform_type = typename space_type::transform_type;
            STATIC_REQUIRE(std::is_same_v<transform_type, tensor_type>);
        }
        SECTION("from_space_type") {
            using from_space_type = typename space_type::from_space_type;
            using corr            = from_space;
            STATIC_REQUIRE(std::is_same_v<from_space_type, corr>);
        }
        SECTION("from_space_ptr") {
            using from_space_ptr = typename space_type::from_space_ptr;
            using corr           = std::shared_ptr<const from_space>;
            STATIC_REQUIRE(std::is_same_v<from_space_ptr, corr>);
        }
        SECTION("size_type") {
            using size_type = typename space_type::size_type;
            using corr      = typename base_space::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr>);
        }
    }

    auto& world = TA::get_default_world();
    tensor_type C(world, {{1.0, 2.0}, {3.0, 4.0}});
    tensor_type t(
      world, {{{{10.0, 11.0}, {12.0, 13.0}}, {{14.0, 15.0}, {16.0, 17.0}}},
              {{{18.0, 19.0}, {20.0, 21.0}}, {{22.0, 23.0}, {24.0, 25.0}}}});

    SECTION("Default Ctor") {
        space_type s;
        REQUIRE(s.C() == tensor_type{});
    }

    space_type default_ao(C, from_space{});
    AOBasisSetD bs;
    bs.add_center(libchemist::Center<scalar_type>(1.0, 2.0, 3.0));
    from_space aos(std::move(bs));
    space_type non_default(C, aos);

    SECTION("Value Ctor") {
        REQUIRE(default_ao.C() == C);
        REQUIRE(default_ao.from_space() == from_space{});
        REQUIRE(non_default.C() == C);
        REQUIRE(non_default.from_space() == aos);
    }

    SECTION("Aliasing Ctor") {
        space_type alias(C, default_ao.from_space_data());
        REQUIRE(alias.C() == C);
        REQUIRE(alias.from_space_data() == default_ao.from_space_data());
    }

    SECTION("Copy Ctor") {
        space_type copy(non_default);
        REQUIRE(copy.C() == C);
        REQUIRE(copy.from_space_data() == non_default.from_space_data());
    }

    SECTION("Move Ctor") {
        space_type moved(std::move(non_default));
        REQUIRE(moved.C() == C);
        REQUIRE(moved.from_space() == aos);
    }

    SECTION("Copy assignment") {
        space_type copy;
        auto pcopy = &(copy = non_default);
        REQUIRE(pcopy == &copy);
        REQUIRE(copy.C() == C);
        REQUIRE(copy.from_space_data() == non_default.from_space_data());
    }

    SECTION("Move assignment") {
        space_type moved;
        auto pmoved = &(moved = std::move(non_default));
        REQUIRE(pmoved == &moved);
        REQUIRE(moved.C() == C);
        REQUIRE(moved.from_space() == aos);
    }
}

TEST_CASE("IndDerivedSpace") {
    using scalar_type = double;
    using space_type  = IndDerivedSpaceD;
    using tensor_type = libchemist::type::tensor<scalar_type>;
    using tot_type    = libchemist::type::tensor_of_tensors<scalar_type>;
    using tile_type   = typename tot_type::value_type;
    using inner_type  = typename tile_type::value_type;
    using from_space  = DepAOSpaceD;
    using base_space  = BaseSpace;

    SECTION("Typedefs") {
        SECTION("transform_type") {
            using transform_type = typename space_type::transform_type;
            STATIC_REQUIRE(std::is_same_v<transform_type, tensor_type>);
        }
        SECTION("from_space_type") {
            using from_space_type = typename space_type::from_space_type;
            using corr            = from_space;
            STATIC_REQUIRE(std::is_same_v<from_space_type, corr>);
        }
        SECTION("from_space_ptr") {
            using from_space_ptr = typename space_type::from_space_ptr;
            using corr           = std::shared_ptr<const from_space>;
            STATIC_REQUIRE(std::is_same_v<from_space_ptr, corr>);
        }
        SECTION("size_type") {
            using size_type = typename space_type::size_type;
            using corr      = typename base_space::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr>);
        }
    }

    auto& world = TA::get_default_world();
    tensor_type C(world, {{1.0, 2.0}, {3.0, 4.0}});

    inner_type t00(TA::Range({2, 2}), {10.0, 11.0, 12.0, 13.0});
    inner_type t01(TA::Range({2, 2}), {14.0, 15.0, 16.0, 17.0});
    inner_type t10(TA::Range({2, 2}), {18.0, 19.0, 20.0, 21.0});
    inner_type t11(TA::Range({2, 2}), {22.0, 23.0, 24.0, 25.0});
    tot_type t(world, {{t00, t01}, {t10, t11}});

    SECTION("Default Ctor") {
        space_type s;
        REQUIRE(s.C() == tensor_type{});
    }

    space_type default_ao(C, from_space{});
    AOBasisSetD bs;
    bs.add_center(libchemist::Center<scalar_type>(1.0, 2.0, 3.0));
    from_space aos(std::move(bs));
    space_type non_default(C, aos);

    SECTION("Value Ctor") {
        REQUIRE(default_ao.C() == C);
        REQUIRE(default_ao.from_space() == from_space{});
        REQUIRE(non_default.C() == C);
        REQUIRE(non_default.from_space() == aos);
    }

    SECTION("Aliasing Ctor") {
        space_type alias(C, default_ao.from_space_data());
        REQUIRE(alias.C() == C);
        REQUIRE(alias.from_space_data() == default_ao.from_space_data());
    }

    SECTION("Copy Ctor") {
        space_type copy(non_default);
        REQUIRE(copy.C() == C);
        REQUIRE(copy.from_space_data() == non_default.from_space_data());
    }

    SECTION("Move Ctor") {
        space_type moved(std::move(non_default));
        REQUIRE(moved.C() == C);
        REQUIRE(moved.from_space() == aos);
    }

    SECTION("Copy assignment") {
        space_type copy;
        auto pcopy = &(copy = non_default);
        REQUIRE(pcopy == &copy);
        REQUIRE(copy.C() == C);
        REQUIRE(copy.from_space_data() == non_default.from_space_data());
    }

    SECTION("Move assignment") {
        space_type moved;
        auto pmoved = &(moved = std::move(non_default));
        REQUIRE(pmoved == &moved);
        REQUIRE(moved.C() == C);
        REQUIRE(moved.from_space() == aos);
    }
}

TEST_CASE("DepDerivedSpace") {
    using scalar_type = double;
    using space_type  = DepDerivedSpaceD;
    using tensor_type = libchemist::type::tensor_of_tensors<scalar_type>;
    using tile_type   = typename tensor_type::value_type;
    using inner_type  = typename tile_type::value_type;
    using from_space  = DepAOSpaceD;
    using base_space  = DependentSpace;
    using sparse_map  = typename base_space::sparse_map_type;
    using index_type  = typename sparse_map::key_type;

    SECTION("Typedefs") {
        SECTION("transform_type") {
            using transform_type = typename space_type::transform_type;
            STATIC_REQUIRE(std::is_same_v<transform_type, tensor_type>);
        }
        SECTION("from_space_type") {
            using from_space_type = typename space_type::from_space_type;
            using corr            = from_space;
            STATIC_REQUIRE(std::is_same_v<from_space_type, corr>);
        }
        SECTION("from_space_ptr") {
            using from_space_ptr = typename space_type::from_space_ptr;
            using corr           = std::shared_ptr<const from_space>;
            STATIC_REQUIRE(std::is_same_v<from_space_ptr, corr>);
        }
        SECTION("size_type") {
            using size_type = typename space_type::size_type;
            using corr      = typename base_space::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr>);
        }
    }

    auto& world = TA::get_default_world();
    inner_type c00(TA::Range({2, 2}), {1.0, 2.0, 3.0, 4.0});
    inner_type c01(TA::Range({2, 2}), {5.0, 6.0, 7.0, 8.0});
    inner_type c10(TA::Range({2, 2}), {9.0, 1.0, 2.0, 3.0});
    inner_type c11(TA::Range({2, 2}), {4.0, 5.0, 6.0, 7.0});
    tensor_type C(world, {{c00, c01}, {c10, c11}});

    inner_type t00(TA::Range({2, 2}), {10.0, 11.0, 12.0, 13.0});
    inner_type t01(TA::Range({2, 2}), {14.0, 15.0, 16.0, 17.0});
    inner_type t10(TA::Range({2, 2}), {18.0, 19.0, 20.0, 21.0});
    inner_type t11(TA::Range({2, 2}), {22.0, 23.0, 24.0, 25.0});
    tensor_type t(world, {{t00, t01}, {t10, t11}});

    SECTION("Default Ctor") {
        space_type s;
        REQUIRE(s.C() == tensor_type{});
        REQUIRE(s.sparse_map() == sparse_map{});
    }

    space_type non_default_c(C, from_space{});

    AOBasisSetD bs;
    bs.add_center(libchemist::Center<scalar_type>(1.0, 2.0, 3.0));
    from_space aos(std::move(bs));
    space_type non_default_bs(C, aos);

    index_type i00{0, 0}, i01{0, 1}, i10{1, 0}, i11{1, 1};
    std::initializer_list domain{i00, i01, i10, i11};
    sparse_map sm{{i00, domain}, {i01, domain}, {i10, domain}, {i11, domain}};
    space_type non_default(C, aos, sm);

    SECTION("Value Ctor") {
        REQUIRE(non_default_c.C() == C);
        REQUIRE(non_default_c.from_space() == from_space{});
        REQUIRE(non_default_c.sparse_map() == sparse_map{});
        REQUIRE(non_default_bs.C() == C);
        REQUIRE(non_default_bs.from_space() == aos);
        REQUIRE(non_default_bs.sparse_map() == sparse_map{});
        REQUIRE(non_default.C() == C);
        REQUIRE(non_default.from_space() == aos);
        REQUIRE(non_default.sparse_map() == sm);
    }

    SECTION("Aliasing Ctor") {
        space_type alias(C, non_default.from_space_data(), sm);
        REQUIRE(alias.C() == C);
        REQUIRE(alias.from_space_data() == non_default.from_space_data());
        REQUIRE(alias.sparse_map() == sm);
    }

    SECTION("Copy Ctor") {
        space_type copy(non_default);
        REQUIRE(copy.C() == C);
        REQUIRE(copy.from_space_data() == non_default.from_space_data());
        REQUIRE(copy.sparse_map() == sm);
    }

    SECTION("Move Ctor") {
        space_type moved(std::move(non_default));
        REQUIRE(moved.C() == C);
        REQUIRE(moved.from_space() == aos);
        REQUIRE(moved.sparse_map() == sm);
    }

    SECTION("Copy assignment") {
        space_type copy;
        auto pcopy = &(copy = non_default);
        REQUIRE(pcopy == &copy);
        REQUIRE(copy.C() == C);
        REQUIRE(copy.from_space_data() == non_default.from_space_data());
        REQUIRE(copy.sparse_map() == sm);
    }

    SECTION("Move assignment") {
        space_type moved;
        auto pmoved = &(moved = std::move(non_default));
        REQUIRE(pmoved == &moved);
        REQUIRE(moved.C() == C);
        REQUIRE(moved.from_space() == aos);
        REQUIRE(moved.sparse_map() == sm);
    }
}
