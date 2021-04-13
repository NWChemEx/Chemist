#include "../test_orbital_space.hpp"
#include "../transform_data.hpp"
#include "libchemist/orbital_space/base_space.hpp"
#include "libchemist/orbital_space/derived_space.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::orbital_space;

TEMPLATE_TEST_CASE("tot_transform", "", double, float) {
    using tensor_type = type::tensor_of_tensors<TestType>;
    using from_space  = BaseSpace_<tensor_type>;
}