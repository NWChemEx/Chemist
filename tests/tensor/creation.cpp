#include "libchemist/tensor/creation.hpp"
#include "libchemist/tensor/tensor_wrapper.hpp"
#include "test_tensor.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

TEST_CASE("concatenate(Tensor)") {
    using tensor_t = libchemist::tensor::type::SparseTensorWrapper;
    auto tensors   = testing::get_tensors<TA::TSpArrayD>();
    auto& world    = TA::get_default_world();

    tensor_t v(tensors.at("vector"));
    tensor_t m(tensors.at("matrix"));

    using v_il = TA::detail::vector_il<double>;
    using m_il = TA::detail::matrix_il<double>;
    using t_il = TA::detail::tensor3_il<double>;

    SECTION("Vectors") {
        TA::TSpArrayD corr_ta(world, v_il{1.0, 2.0, 3.0, 1.0, 2.0, 3.0});
        tensor_t corr(corr_ta);
        auto rv = concatenate(v, v, 0);
        REQUIRE(rv == corr);
    }

    SECTION("Matrix") {
        SECTION("Mode 0") {
            m_il il{v_il{1.0, 2.0}, v_il{3.0, 4.0}, v_il{1.0, 2.0},
                    v_il{3.0, 4.0}};
            tensor_t corr(TA::TSpArrayD(world, il));
            auto rv = concatenate(m, m, 0);
            REQUIRE(rv == corr);
        }
        SECTION("Mode 1") {
            m_il il{v_il{1.0, 2.0, 1.0, 2.0}, v_il{3.0, 4.0, 3.0, 4.0}};
            tensor_t corr(TA::TSpArrayD(world, il));
            auto rv = concatenate(m, m, 1);
            REQUIRE(rv == corr);
        }
    }

    SECTION("Throws if attempting to concatenate different ranks") {
        REQUIRE_THROWS_AS(concatenate(v, m, 1), std::runtime_error);
    }

    SECTION("Throws if dimension is not valid") {
        REQUIRE_THROWS_AS(concatenate(v, v, 1), std::runtime_error);
    }

    SECTION("Throws if shapes are not compatible") {
        m_il il{v_il{1.0}};
        TA::TSpArrayD other_m_ta(world, il);
        tensor_t other_m(other_m_ta);
        REQUIRE_THROWS_AS(concatenate(m, other_m, 0), std::runtime_error);
    }
}

TEST_CASE("concatenate(ToT)") {
    type::ToTWrapper A, B;
    REQUIRE_THROWS_AS(concatenate(A, B, 0), std::runtime_error);
}
