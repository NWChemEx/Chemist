#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/einsum/einsum.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>

using namespace libchemist::ta_helpers::einsum;

TEST_CASE("einsum"){

  auto& world = TA::get_default_world();

  TA::TSpArrayD vec(world, {1.0, 2.0, 3.0});
  TA::TSpArrayD sq_mat(world, {{1.0, 2.0, 3.0},
                              {4.0, 5.0, 6.0},
                              {7.0, 8.0, 9.0}});
  TA::TSpArrayD mat(world, {{1.0, 2.0, 3.0},
                           {4.0, 5.0, 6.0}});
  TA::TSpArrayD sq_tensor(world, {{{1.0, 2.0, 3.0},
                                  {4.0, 5.0, 6.0},
                                  {7.0, 8.0, 9.0}},

                                 {{10.0, 11.0, 12.0},
                                  {13.0, 14.0, 15.0},
                                  {16.0, 17.0, 18.0}},

                                 {{19.0, 20.0, 21.0},
                                  {22.0, 23.0, 24.0},
                                  {25.0, 26.0, 27.0}}});

  TA::TSpArrayD tensor(world, {{{1.0, 2.0, 3.0},
                               {4.0, 5.0, 6.0},
                               {7.0, 8.0, 9.0}},

                              {{10.0, 11.0, 12.0},
                               {13.0, 14.0, 15.0},
                               {16.0, 17.0, 18.0}}});

    SECTION("tensor-tensor"){
      auto result = einsum("i,j,k", "i, j, k", "l, j, k", sq_tensor, tensor);
      TA::TSpArrayD corr(world, {{{11,26,45,},
                                  {68,95,126,},
                                  {161,200,243,}},

                                 {{110,143,180,},
                                  {221,266,315,},
                                  {368,425,486,}},

                                  {{209,260,315,},
                                   {374,437,504,},
                                   {575,650,729,}}});
      REQUIRE(libchemist::ta_helpers::allclose(result, corr));
    }
} // einsum