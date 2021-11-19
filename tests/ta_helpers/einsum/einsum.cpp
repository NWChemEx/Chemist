#include "chemist/ta_helpers/einsum/einsum.hpp"
#include "chemist/ta_helpers/ta_helpers.hpp"
#include <catch2/catch.hpp>

using namespace chemist::ta_helpers::einsum;

TEST_CASE("einsum") {
    using vector_il = TA::detail::vector_il<double>;
    using matrix_il = TA::detail::matrix_il<double>;
    using tensor_il = TA::detail::tensor3_il<double>;
    auto& world     = TA::get_default_world();

    TA::TSpArrayD vec(world, vector_il{1.0, 2.0, 3.0});
    TA::TSpArrayD sq_mat(world, matrix_il{vector_il{1.0, 2.0, 3.0},
                                          vector_il{4.0, 5.0, 6.0},
                                          vector_il{7.0, 8.0, 9.0}});
    TA::TSpArrayD mat(
      world, matrix_il{vector_il{1.0, 2.0, 3.0}, vector_il{4.0, 5.0, 6.0}});
    TA::TSpArrayD sq_tensor(
      world,
      tensor_il{
        matrix_il{vector_il{1.0, 2.0, 3.0}, vector_il{4.0, 5.0, 6.0},
                  vector_il{7.0, 8.0, 9.0}},

        matrix_il{vector_il{10.0, 11.0, 12.0}, vector_il{13.0, 14.0, 15.0},
                  vector_il{16.0, 17.0, 18.0}},

        matrix_il{vector_il{19.0, 20.0, 21.0}, vector_il{22.0, 23.0, 24.0},
                  vector_il{25.0, 26.0, 27.0}}});

    TA::TSpArrayD tensor(
      world,
      tensor_il{matrix_il{vector_il{1.0, 2.0, 3.0}, vector_il{4.0, 5.0, 6.0},
                          vector_il{7.0, 8.0, 9.0}},

                matrix_il{vector_il{10.0, 11.0, 12.0},
                          vector_il{13.0, 14.0, 15.0},
                          vector_il{16.0, 17.0, 18.0}}});

    SECTION("tensor-tensor") {
        auto result = einsum("i,j,k", "i, j, k", "l, j, k", sq_tensor, tensor);
        TA::TSpArrayD corr(world, tensor_il{matrix_il{vector_il{
                                                        11,
                                                        26,
                                                        45,
                                                      },
                                                      vector_il{
                                                        68,
                                                        95,
                                                        126,
                                                      },
                                                      vector_il{
                                                        161,
                                                        200,
                                                        243,
                                                      }},

                                            matrix_il{vector_il{
                                                        110,
                                                        143,
                                                        180,
                                                      },
                                                      vector_il{
                                                        221,
                                                        266,
                                                        315,
                                                      },
                                                      vector_il{
                                                        368,
                                                        425,
                                                        486,
                                                      }},

                                            matrix_il{vector_il{
                                                        209,
                                                        260,
                                                        315,
                                                      },
                                                      vector_il{
                                                        374,
                                                        437,
                                                        504,
                                                      },
                                                      vector_il{
                                                        575,
                                                        650,
                                                        729,
                                                      }}});
        REQUIRE(chemist::ta_helpers::allclose(result, corr));
    }
} // einsum
