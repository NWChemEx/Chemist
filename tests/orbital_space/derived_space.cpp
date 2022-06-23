#include "chemist/basis_set/basis_set.hpp"
#include "chemist/orbital_space/derived_space.hpp"
#include <catch2/catch.hpp>

/* For testing purposes we assume:
 *
 * - The actual identity/state of the from space does not impact the
 *   DerivedSpace class aside from needing to have a slot in the ctor and be
 *   accessible through the DerivedSpace members. i.e. as long as that type is
 *   tested it'll work here too.
 */

using namespace chemist;
using namespace chemist::orbital_space;

TEST_CASE("DerivedSpace") {
    // Work out types we will need
    using scalar_type = double;
    using space_type  = DerivedSpaceD;
    using tensor_type = chemist::type::tensor;
    using from_space  = AOSpaceD;
    using base_space  = BaseSpace;
    using vector_il   = TA::detail::vector_il<double>;
    using matrix_il   = TA::detail::matrix_il<double>;

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

    /// Remove with removal of TW pin
    using ta_tensor = TA::DistArray<TA::Tensor<scalar_type>, TA::SparsePolicy>;
    auto& world     = TA::get_default_world();
    tensor_type C(
      ta_tensor(world, matrix_il{vector_il{1.0, 2.0}, vector_il{3.0, 4.0}}));

    /// Uncomment with removal of TW pin
    // tensor_type C(matrix_il{vector_il{1.0, 2.0}, vector_il{3.0, 4.0}});

    space_type default_ao(C, from_space{});
    AOBasisSetD bs;
    bs.add_center(chemist::AtomicBasisSet<scalar_type>("", 0, 1.0, 2.0, 3.0));
    from_space aos(std::move(bs));
    space_type non_default_aos(tensor_type{}, aos);
    space_type non_default(C, aos);

    SECTION("CTors/Assignment") {
        SECTION("Default Ctor") {
            space_type s;
            REQUIRE(s.size() == 0);
        }

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

    SECTION("Accessors") {
        SECTION("C()") {
            REQUIRE(non_default.C() == C);

            SECTION("Throws if no transformation") {
                space_type s;
                REQUIRE_THROWS_AS(s.C(), std::runtime_error);
            }
        }

        SECTION("from_space()") {
            REQUIRE(non_default.from_space() == aos);

            SECTION("Throws if no space") {
                space_type s;
                REQUIRE_THROWS_AS(s.from_space(), std::runtime_error);
            }
        }

        SECTION("C_data") {
            auto pC = &(non_default.C());
            REQUIRE(non_default.C_data().get() == pC);
        }

        SECTION("from_space_data()") {
            auto paos = &(non_default.from_space());
            REQUIRE(non_default.from_space_data().get() == paos);
        }

        SECTION("size") {
            REQUIRE(space_type{}.size() == 0);
            REQUIRE(non_default.size() == 2);
        }
    }

    SECTION("operator+") {
        SECTION("Throws if different from space") {
            REQUIRE_THROWS_AS(non_default + default_ao, std::runtime_error);
        }

        auto new_C = tensorwrapper::tensor::concatenate(non_default.C(),
                                                        non_default.C(), 1);
        space_type corr(new_C, non_default.from_space_data());
        auto rv = non_default + non_default;
        REQUIRE(corr == rv);
    }

    SECTION("hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS == default") {
            const auto lhs = hash_objects(space_type{});

            SECTION("RHS == default") {
                const space_type rhs;
                REQUIRE(lhs == hash_objects(rhs));
            }

            SECTION("RHS == non-default C") {
                REQUIRE(lhs != hash_objects(default_ao));
            }

            SECTION("RHS == non-default from-space") {
                REQUIRE(lhs != hash_objects(non_default_aos));
            }
        }

        SECTION("LHS == non-default C") {
            const auto lhs = hash_objects(default_ao);
            SECTION("RHS == non-default C") {
                REQUIRE(lhs == hash_objects(space_type(C, from_space{})));
            }
            SECTION("RHS == non-default from-space") {
                REQUIRE(lhs != hash_objects(non_default_aos));
            }
        }

        SECTION("Non-default from-space") {
            const auto lhs = hash_objects(non_default_aos);
            const auto rhs = hash_objects(space_type{tensor_type{}, aos});
            REQUIRE(lhs == rhs);
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            const space_type lhs;
            SECTION("RHS == default") {
                const space_type rhs;
                REQUIRE(lhs == rhs);
                REQUIRE_FALSE(lhs != rhs);
            }

            SECTION("RHS == non-default C") {
                REQUIRE_FALSE(lhs == default_ao);
                REQUIRE(lhs != default_ao);
            }

            SECTION("RHS == non-default from-space") {
                REQUIRE_FALSE(lhs == non_default_aos);
                REQUIRE(lhs != non_default_aos);
            }
        }

        SECTION("LHS == non-default C") {
            SECTION("RHS == non-default C") {
                const space_type rhs(C, from_space{});
                REQUIRE(default_ao == rhs);
                REQUIRE_FALSE(default_ao != rhs);
            }
            SECTION("RHS == non-default from-space") {
                REQUIRE_FALSE(default_ao == non_default_aos);
                REQUIRE(default_ao != non_default_aos);
                ;
            }
        }

        SECTION("Non-default from-space") {
            const auto rhs = space_type{tensor_type{}, aos};
            REQUIRE(non_default_aos == rhs);
            REQUIRE_FALSE(non_default_aos != rhs);
        }
    }
}
