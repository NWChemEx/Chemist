#include "chemist/orbital_space/detail_/make_indices.hpp"
#include <catch2/catch.hpp>

using namespace chemist::orbital_space::detail_;

TEST_CASE("make_tensor_tensor_indices") {
    const std::string lhs = "i0,i1";

    SECTION("Transform a vector") {
        const std::string rhs          = "j0";
        const std::string corr_new_rhs = "i0";
        const std::string corr_result  = "i1";
        auto [result, new_rhs] = make_tensor_tensor_indices(lhs, rhs, 0);
        REQUIRE(result == corr_result);
        REQUIRE(new_rhs == corr_new_rhs);
    }

    SECTION("Transform a matrix") {
        const std::string rhs = "j0,j1";
        SECTION("Mode 0") {
            const std::string corr_new_rhs = "i0,j1";
            const std::string corr_result  = "i1,j1";
            auto [result, new_rhs] = make_tensor_tensor_indices(lhs, rhs, 0);
            REQUIRE(result == corr_result);
            REQUIRE(new_rhs == corr_new_rhs);
        }
        SECTION("Mode 1") {
            const std::string corr_new_rhs = "j0,i0";
            const std::string corr_result  = "j0,i1";
            auto [result, new_rhs] = make_tensor_tensor_indices(lhs, rhs, 1);
            REQUIRE(result == corr_result);
            REQUIRE(new_rhs == corr_new_rhs);
        }
    }

    SECTION("Transform a tensor") {
        const std::string rhs = "j0,j1,j2";
        SECTION("Mode 0") {
            const std::string corr_new_rhs = "i0,j1,j2";
            const std::string corr_result  = "i1,j1,j2";
            auto [result, new_rhs] = make_tensor_tensor_indices(lhs, rhs, 0);
            REQUIRE(result == corr_result);
            REQUIRE(new_rhs == corr_new_rhs);
        }
        SECTION("Mode 1") {
            const std::string corr_new_rhs = "j0,i0,j2";
            const std::string corr_result  = "j0,i1,j2";
            auto [result, new_rhs] = make_tensor_tensor_indices(lhs, rhs, 1);
            REQUIRE(result == corr_result);
            REQUIRE(new_rhs == corr_new_rhs);
        }
        SECTION("Mode 2") {
            const std::string corr_new_rhs = "j0,j1,i0";
            const std::string corr_result  = "j0,j1,i1";
            auto [result, new_rhs] = make_tensor_tensor_indices(lhs, rhs, 2);
            REQUIRE(result == corr_result);
            REQUIRE(new_rhs == corr_new_rhs);
        }
    }

    SECTION("Throws if requested mode is out of range") {
        using except_t = std::out_of_range;
        REQUIRE_THROWS_AS(make_tensor_tensor_indices(lhs, "j0", 1), except_t);
    }

    SECTION("Throws if LHS is not a matrix") {
        using except_t = std::runtime_error;
        REQUIRE_THROWS_AS(make_tensor_tensor_indices("i0", "j0", 0), except_t);
    }
}

TEST_CASE("make_tensor_tot_indices") {
    const std::string lhs = "i0,i1";

    SECTION("Transform vector of vectors") {
        const std::string rhs          = "j0;j1";
        const std::string corr_result  = "i1;j1";
        const std::string corr_new_rhs = "i0;j1";
        auto [result, new_rhs]         = make_tensor_tot_indices(lhs, rhs, 0);
        REQUIRE(result == corr_result);
        REQUIRE(new_rhs == corr_new_rhs);
    }

    SECTION("Transform vector of matricies") {
        const std::string rhs          = "j0;j1,j2";
        const std::string corr_result  = "i1;j1,j2";
        const std::string corr_new_rhs = "i0;j1,j2";
        auto [result, new_rhs]         = make_tensor_tot_indices(lhs, rhs, 0);
        REQUIRE(result == corr_result);
        REQUIRE(new_rhs == corr_new_rhs);
    }

    SECTION("Transform matrix of vectors") {
        const std::string rhs = "j0,j1;j2";

        SECTION("Mode 0") {
            const std::string corr_result  = "i1,j1;j2";
            const std::string corr_new_rhs = "i0,j1;j2";
            auto [result, new_rhs] = make_tensor_tot_indices(lhs, rhs, 0);
            REQUIRE(result == corr_result);
            REQUIRE(new_rhs == corr_new_rhs);
        }

        SECTION("Mode 1") {
            const std::string corr_result  = "j0,i1;j2";
            const std::string corr_new_rhs = "j0,i0;j2";
            auto [result, new_rhs] = make_tensor_tot_indices(lhs, rhs, 1);
            REQUIRE(result == corr_result);
            REQUIRE(new_rhs == corr_new_rhs);
        }
    }

    SECTION("Throws if transforming dependent mode") {
        using except_t = std::runtime_error;
        REQUIRE_THROWS_AS(make_tensor_tot_indices(lhs, "j0;j1", 1), except_t);
    }
}

TEST_CASE("make_tot_tot_indices") {
    // SECTION("LHS == vector-of-vectors") {
    //     const std::string lhs = "i0;i1";
    // }

    SECTION("LHS == vector-of-matrices") {
        const std::string lhs = "i0;i1,i2";

        SECTION("RHS == vector-of-vectors") {
            const std::string rhs          = "j0;j1";
            const std::string corr_result  = "i0;i2";
            const std::string corr_new_rhs = "i0;i1";
            auto [result, new_rhs]         = make_tot_tot_indices(lhs, rhs, 1);
            REQUIRE(result == corr_result);
            REQUIRE(new_rhs == corr_new_rhs);
        }

        SECTION("RHS == vector-of-matrices") {
            const std::string rhs = "j0;j1,j2";
            SECTION("Mode 1") {
                const std::string corr_result  = "i0;i2,j2";
                const std::string corr_new_rhs = "i0;i1,j2";
                auto [result, new_rhs] = make_tot_tot_indices(lhs, rhs, 1);
                REQUIRE(result == corr_result);
                REQUIRE(new_rhs == corr_new_rhs);
            }
            SECTION("Mode 2") {
                const std::string corr_result  = "i0;j1,i2";
                const std::string corr_new_rhs = "i0;j1,i1";
                auto [result, new_rhs] = make_tot_tot_indices(lhs, rhs, 2);
                REQUIRE(result == corr_result);
                REQUIRE(new_rhs == corr_new_rhs);
            }
        }
    }

    // SECTION("LHS == matrix-of-vectors") {
    //     const std::string lhs = "i0,i1;i2";
    // }

    SECTION("LHS == matirx-of-matrices") {
        const std::string lhs = "i0,i1;i2,i3";

        SECTION("RHS == matrix-of-vectors") {
            const std::string rhs          = "j0,j1;j2";
            const std::string corr_result  = "i0,i1;i3";
            const std::string corr_new_rhs = "i0,i1;i2";
            auto [result, new_rhs]         = make_tot_tot_indices(lhs, rhs, 2);
            REQUIRE(result == corr_result);
            REQUIRE(new_rhs == corr_new_rhs);
        }

        SECTION("RHS == matrix-of-matrices") {
            const std::string rhs = "j0,j1;j2,j3";
            SECTION("Mode 2") {
                const std::string corr_result  = "i0,i1;i3,j3";
                const std::string corr_new_rhs = "i0,i1;i2,j3";
                auto [result, new_rhs] = make_tot_tot_indices(lhs, rhs, 2);
                REQUIRE(result == corr_result);
                REQUIRE(new_rhs == corr_new_rhs);
            }
            SECTION("Mode 3") {
                const std::string corr_result  = "i0,i1;j2,i3";
                const std::string corr_new_rhs = "i0,i1;j2,i2";
                auto [result, new_rhs] = make_tot_tot_indices(lhs, rhs, 3);
                REQUIRE(result == corr_result);
                REQUIRE(new_rhs == corr_new_rhs);
            }
        }
    }

    SECTION("Throws if attempting to transform independent mode") {
        using except_t = std::runtime_error;
        REQUIRE_THROWS_AS(make_tot_tot_indices("i0;i1,i2", "j0;j1", 0),
                          except_t);
    }

    SECTION("Throws if attempting x-of-vectors transform") {
        using except_t = std::runtime_error;
        REQUIRE_THROWS_AS(make_tot_tot_indices("i0;i1", "j0;j1", 1), except_t);
    }

    SECTION("Throws if ind-ranks aren't equal") {
        using except_t = std::runtime_error;
        REQUIRE_THROWS_AS(make_tot_tot_indices("i0,i1;i2,i3", "j0;j1", 1),
                          except_t);
    }
}

TEST_CASE("make_indices") {
    // We've tested the kernels already so we just need to make sure we dispatch
    // correctly

    SECTION("tensor by tensor") {
        const std::string lhs          = "i0,i1";
        const std::string rhs          = "j0,j1,j2,j3";
        const std::string corr_result  = "j0,j1,i1,j3";
        const std::string corr_new_rhs = "j0,j1,i0,j3";
        auto [result, new_rhs]         = make_indices(lhs, rhs, 2);
        REQUIRE(result == corr_result);
        REQUIRE(new_rhs == corr_new_rhs);
    }

    SECTION("ToT by tensor") {
        const std::string lhs          = "i0,i1";
        const std::string rhs          = "j0,j1;j2,j3";
        const std::string corr_result  = "j0,i1;j2,j3";
        const std::string corr_new_rhs = "j0,i0;j2,j3";
        auto [result, new_rhs]         = make_indices(lhs, rhs, 1);
        REQUIRE(result == corr_result);
        REQUIRE(new_rhs == corr_new_rhs);
    }

    SECTION("Throws if attempting to transform tensor by a ToT") {
        using except_t = std::runtime_error;
        REQUIRE_THROWS_AS(make_indices("i0;i1", "j0,j1", 0), except_t);
    }

    SECTION("ToT by ToT") {
        const std::string lhs          = "i0,i1;i2,i3";
        const std::string rhs          = "j0,j1;j2,j3";
        const std::string corr_result  = "i0,i1;j2,i3";
        const std::string corr_new_rhs = "i0,i1;j2,i2";
        auto [result, new_rhs]         = make_indices(lhs, rhs, 3);
        REQUIRE(result == corr_result);
        REQUIRE(new_rhs == corr_new_rhs);
    }
}
