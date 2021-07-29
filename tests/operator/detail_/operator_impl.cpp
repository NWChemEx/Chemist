#include "../test_operator.hpp"
#include <utilities/type_traits/tuple/count_type.hpp>

/* Testing Strategy:
 *
 * OperatorImpl actually implements the majority of the operators via CRTP. All
 * constructors of OperatorImpl are protected so instances of OperatorImpl can
 * only be created by making derived class instances. Here we test the
 * non-polymorphic operations.
 */

using namespace utilities::type_traits;

namespace {

struct FacadeOperator : OperatorImpl

}

TEMPLATE_LIST_TEST_CASE("OperatorImpl", "", testing::all_operators) {
    using derived_type   = TestType;
    using particle_types = typename derived_type::particle_type;

    SECTION("Typedefs / static values") {
        SECTION("n_body") {
            constexpr auto corr = std::tuple_size<particle_types>();
            STATIC_REQUIRE(derived_type::n_body == corr);
        }

        SECTION("n_electrons") {
            constexpr auto corr = tuple::count_type_v<Electron, particle_types>;
            STATIC_REQUIRE(derived_type::n_electrons == corr);
        }

        SECTION("n_nuclei") {
            constexpr auto corr = tuple::count_type_v<Nuclei, particle_types>;
            STATIC_REQUIRE(derived_type::n_nuclei == corr);
        }
    }

    SECTION("Non-polymorphic comparisons") {
        SECTION("Types are different") {
            // Abuse the fact that DerivedClass can be pretty much any type
            using other_impl = OperatorImpl<int>;
            REQUIRE(other_impl)
        }
    }
}
