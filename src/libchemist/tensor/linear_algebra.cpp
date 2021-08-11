#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/linear_algebra.hpp"
#include "libchemist/tensor/tensor.hpp"

namespace libchemist::tensor {

using TWrapper = type::SparseTensorWrapper;

std::pair<TWrapper, TWrapper> eigen_solve(const TWrapper& X) {
    using tensor_type = tensor::type::detail_::tensor<double>;

    const auto& x = X.get<tensor_type>();

    auto [eval_vec, evecs] = TA::math::linalg::heig(x);
    const auto& tr1        = evecs.trange().dim(0);
    auto evals = ta_helpers::array_from_vec(eval_vec, tr1, evecs.world());
    TWrapper EVals(std::move(evals));
    TWrapper EVecs(std::move(evecs));
    return std::make_pair(EVals, EVecs);
}

std::pair<TWrapper, TWrapper> eigen_solve(const TWrapper& X,
                                          const TWrapper& S) {
    using tensor_type = tensor::type::detail_::tensor<double>;

    const auto& x = X.get<tensor_type>();
    const auto& s = S.get<tensor_type>();

    auto [eval_vec, evecs] = TA::math::linalg::heig(x, s);
    const auto& tr1        = evecs.trange().dim(0);
    auto evals = ta_helpers::array_from_vec(eval_vec, tr1, evecs.world());
    TWrapper EVals(std::move(evals));
    TWrapper EVecs(std::move(evecs));
    return std::make_pair(EVals, EVecs);
}

} // namespace libchemist::tensor
