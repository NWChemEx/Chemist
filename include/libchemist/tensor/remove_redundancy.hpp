#pragma once
#include "libchemist/tensor/types.hpp"

namespace libchemist::tensor {

type::SparseTensorWrapper remove_redundancy(const type::SparseTensorWrapper& C,
                                            const type::SparseTensorWrapper& S,
                                            double thresh = 1.0E-8);

}