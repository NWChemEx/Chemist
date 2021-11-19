#pragma once

/** @file fields.hpp
 *
 *  Informally speaking, a field is a set of things for which addition,
 *  multiplication, and their inverses are defined. For our purposes, what we
 *  care about are the things in the field (the elements of the field) as these
 *  are what we can define our tensor components in terms of.
 *
 *  Practically we have two types of tensors:
 *  - Tensors whose elements are numbers (either real or complex)
 *  - Tensors whose elements are tensors of the first kind
 *
 *  In theory, we could continue the nesting (*e.g.*, have tensors whose
 *  elements are tensors of the second kind), but we only support single
 *  nesting as that's all TiledArray supports.
 *
 *  Design wise the fields are (right now) just strong types. Under the hood,
 *  each field maps to a std::variant of possible tile types. The use of the
 *  fields avoids having the variants appear in the TensorWrapper types, i.e.,
 *  instead of:
 *
 *  @code
 *
 *  TensorWrapper<std::variant<TA::Tensor<float>, TA::Tensor<double>, ...>;
 *
 *  @endcode
 *
 *  we get:
 *
 *  @code
 *  TensorWrapper<Scalar>;
 *  @endcode
 */

namespace chemist::tensor::field {

/// Represents a field containing real or complex numbers
struct Scalar {};

/// Represents a field containing tensors
struct Tensor {};

} // namespace chemist::tensor::field
