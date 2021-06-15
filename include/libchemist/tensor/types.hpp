#pragma once
#include "libchemist/types.hpp"
#include <utilities/type_traits/variant/cat.hpp>
#include <variant>

namespace libchemist::tensor::type {

/// Type of a variant with all possible non-hierarchal tensor types in it
using tensor_variant = std::variant<libchemist::type::tensor<double>>;

/// Type of a variant with all possible hierarchal tensor_types in it
using tot_variant = std::variant<libchemist::type::tensor_of_tensors<double>>;

/// Type of a variant with all possible tensor types in it
using all_tensor_variant =
  utilities::type_traits::variant::cat_t<tensor_variant, tot_variant>;

} // namespace libchemist::tensor::type