#include "chemist/operators/coulomb.hpp"
#include "test_operator.hpp"

using namespace chemist::operators;

namespace {

template<typename T, typename U, typename V>
void check_state(T&& r12, U&& p0, V&& p1) {
    using particle_types           = typename std::decay_t<T>::particle_type;
    constexpr auto n_body          = std::tuple_size_v<particle_types>;
    constexpr bool is_two_particle = n_body == 2;

    REQUIRE(r12.template at<0>() == p0);
    if constexpr(is_two_particle) { REQUIRE(r12.template at<1>() == p1); }
}

} // namespace

TEMPLATE_LIST_TEST_CASE("CoulombInteraction", "", testing::all_coulomb) {
    using r12_type                 = TestType;
    using particle_types           = typename r12_type::particle_type;
    constexpr auto n_body          = std::tuple_size_v<particle_types>;
    constexpr bool is_two_particle = n_body == 2;
    using particle0_type           = std::tuple_element_t<0, particle_types>;
    using particle1_type = std::tuple_element_t<n_body - 1, particle_types>;

    r12_type r12;
    particle0_type p0_default;
    particle1_type p1_default;

    auto p0 = testing::non_default_parameter<particle0_type>();
    auto p1 = testing::non_default_parameter<particle1_type>();

    r12_type non_default;
    if constexpr(is_two_particle) {
        non_default = r12_type(p0, p1);
    } else {
        non_default = r12_type(p0);
    }

    SECTION("CTors") {
        SECTION("Default") { check_state(r12, p0_default, p1_default); }
        SECTION("Value") { check_state(non_default, p0, p1); }
        SECTION("Copy") {
            r12_type copy(non_default);
            check_state(copy, p0, p1);
        }
        SECTION("Move") {
            r12_type moved(std::move(non_default));
            check_state(moved, p0, p1);
        }
        SECTION("Copy assignment") {
            r12_type copy;
            auto pcopy = &(copy = non_default);
            REQUIRE(pcopy == &copy);
            check_state(copy, p0, p1);
        }
        SECTION("Move assignment") {
            r12_type moved;
            auto pmoved = &(moved = std::move(non_default));
            REQUIRE(pmoved == &moved);
            check_state(moved, p0, p1);
        }
    }

    SECTION("as_string") { REQUIRE(r12.as_string() == "(r̂₁₂)⁻¹"); }

    // r12 and non_default can be the same if the particles are stateless
    bool is_diff = std::tie(p0, p1) != std::tie(p0_default, p1_default);

    SECTION("hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS is default") {
            auto lhs = hash_objects(r12);
            SECTION("RHS == LHS") { REQUIRE(lhs == hash_objects(r12_type{})); }
            SECTION("RHS != LHS") {
                if(is_diff) REQUIRE(lhs != hash_objects(non_default));
            }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS is default") {
            SECTION("RHS == LHS") {
                r12_type rhs;
                REQUIRE(r12 == rhs);
                REQUIRE_FALSE(r12 != rhs);
            }

            SECTION("RHS != LHS") {
                if(is_diff) {
                    REQUIRE(r12 != non_default);
                    REQUIRE_FALSE(r12 == non_default);
                }
            }
        }
    }
}
