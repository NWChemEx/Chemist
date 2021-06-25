#include "libchemist/molecule/molecule.hpp"
#include "libchemist/set_theory/family_of_sets.hpp"
#include <catch2/catch.hpp>
#include <tuple>
#include <vector>

using namespace libchemist;
using namespace libchemist::set_theory;
using container_types = std::tuple<std::vector<int>, Molecule>;

namespace {

// Makes a non-default object of the specified type that we can partition
template<typename T>
auto make_object() {
    if constexpr(std::is_same_v<T, std::vector<int>>) {
        return T{1, 2, 3};
    } else if constexpr(std::is_same_v<T, libchemist::Molecule>) {
        using value_type = typename Molecule::value_type;
        using cart_type  = typename value_type::coord_type;
        value_type O{8ul, cart_type{0.0, 0.0, 0.0}};
        value_type H0{1ul, cart_type{0.0, 0.0, 0.5}};
        value_type H1{1ul, cart_type{0.0, 0.5, 0.0}};
        return T{O, H0, H1};
    }
}

} // namespace

TEMPLATE_LIST_TEST_CASE("FamilyOfSets", "", container_types) {
    using parent_type = TestType;
    using family_type = FamilyOfSets<TestType>;
    using value_type  = typename family_type::value_type;

    SECTION("typedefs") {
        SECTION("value_type") {
            using corr = Subset<parent_type>;
            STATIC_REQUIRE(std::is_same_v<value_type, corr>);
        }
    }
}