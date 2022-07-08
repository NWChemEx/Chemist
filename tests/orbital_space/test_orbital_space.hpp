#pragma once
#include "chemist/orbital_space/orbital_space.hpp"

namespace testing {

inline auto get_canonical() {
    using tensor_type = chemist::type::tensor;
    using from_space  = chemist::orbital_space::AOSpaceD;
    using vector_il   = TA::detail::vector_il<double>;
    using matrix_il   = TA::detail::matrix_il<double>;

    tensor_type ei(vector_il{1.0, 2.0, 3.0});
    tensor_type c(matrix_il{vector_il{1.0, 2.0}, vector_il{3.0, 4.0}});

    return chemist::orbital_space::CanonicalSpaceD(ei, c, from_space{});
}

} // namespace testing
