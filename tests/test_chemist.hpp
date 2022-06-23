#include "chemist/types.hpp"
#include <catch2/catch.hpp>

namespace testing {

/// This version of generate_tensor needs to be uncommented once the TW pin is
/// removed
// inline auto generate_tensor(unsigned int rank, double seed = 1.0) {
//     using vector_il = TA::detail::vector_il<double>;
//     using matrix_il = TA::detail::matrix_il<double>;
//     using tensor_il = TA::detail::tensor3_il<double>;

//     if(rank == 1u) {
//         return chemist::type::tensor(vector_il{seed, seed + 1.0, seed
//         + 2.0});
//     } else if(rank == 2u) {
//         return chemist::type::tensor(
//           matrix_il{vector_il{seed + 0.0, seed + 1.0, seed + 2.0},
//                     vector_il{seed + 3.0, seed + 4.0, seed + 5.0},
//                     vector_il{seed + 6.0, seed + 7.0, seed + 8.0}});
//     } else if(rank == 3ul) {
//         return chemist::type::tensor(tensor_il{
//           matrix_il{vector_il{seed + 00.0, seed + 01.0, seed + 02.0},
//                     vector_il{seed + 03.0, seed + 04.0, seed + 05.0},
//                     vector_il{seed + 06.0, seed + 07.0, seed + 08.0}},
//           matrix_il{vector_il{seed + 09.0, seed + 10.0, seed + 11.0},
//                     vector_il{seed + 12.0, seed + 13.0, seed + 14.0},
//                     vector_il{seed + 15.0, seed + 16.0, seed + 17.0}},
//           matrix_il{vector_il{seed + 18.0, seed + 19.0, seed + 20.0},
//                     vector_il{seed + 21.0, seed + 22.0, seed + 23.0},
//                     vector_il{seed + 24.0, seed + 25.0, seed + 26.0}}});
//     } else {
//         throw std::runtime_error("Ranks of 0, or higher than 3, are NYI.");
//     }
//     return chemist::type::tensor{};
// }

/// This version of generate_tensor needs to be removed once the TW pin is
/// removed
inline auto generate_tensor(unsigned int rank, double seed = 1.0) {
    auto& world = TA::get_default_world();

    using ta_tensor_type = TA::TSpArrayD;
    ta_tensor_type t;

    using vector_il = TA::detail::vector_il<double>;
    using matrix_il = TA::detail::matrix_il<double>;
    using tensor_il = TA::detail::tensor3_il<double>;

    if(rank == 1u) {
        t = ta_tensor_type(world, vector_il{seed, seed + 1.0, seed + 2.0});
    } else if(rank == 2u) {
        t = ta_tensor_type(
          world, matrix_il{vector_il{seed + 0.0, seed + 1.0, seed + 2.0},
                           vector_il{seed + 3.0, seed + 4.0, seed + 5.0},
                           vector_il{seed + 6.0, seed + 7.0, seed + 8.0}});
    } else if(rank == 3ul) {
        t = ta_tensor_type(
          world,
          tensor_il{
            matrix_il{vector_il{seed + 00.0, seed + 01.0, seed + 02.0},
                      vector_il{seed + 03.0, seed + 04.0, seed + 05.0},
                      vector_il{seed + 06.0, seed + 07.0, seed + 08.0}},
            matrix_il{vector_il{seed + 09.0, seed + 10.0, seed + 11.0},
                      vector_il{seed + 12.0, seed + 13.0, seed + 14.0},
                      vector_il{seed + 15.0, seed + 16.0, seed + 17.0}},
            matrix_il{vector_il{seed + 18.0, seed + 19.0, seed + 20.0},
                      vector_il{seed + 21.0, seed + 22.0, seed + 23.0},
                      vector_il{seed + 24.0, seed + 25.0, seed + 26.0}}});
    } else {
        throw std::runtime_error("Ranks of 0, or higher than 3, are NYI.");
    }
    return chemist::type::tensor(t);
}

} // namespace testing
