/*
 * Copyright 2026 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <string>
#include <wtf/wtf.hpp>

/** @file float_arithmetic.hpp
 *
 *  wtf::fp::Float is a type-erased floating-point value and, as such, does
 *  not define any arithmetic operators of its own (it can not know which
 *  concrete type to do the arithmetic in). The functions declared here wrap
 *  the process of restoring the concrete type, doing the arithmetic in it,
 *  and re-erasing the result.
 *
 *  These are implementation details of the experimental point component. They
 *  are declared (and not defined) here so that the visitation machinery, and
 *  the list of floating-point types it must be instantiated for, stays in a
 *  single translation unit instead of being pulled into every header which
 *  needs to add two coordinates together.
 *
 *  @note Every function here requires both operands to be holding the *same*
 *        concrete floating-point type. Mixed-type arithmetic is not defined
 *        for the uncertainty-quantification types in the list and is
 *        therefore not supported for any of them.
 */

namespace chemist::experimental::detail_ {

/// Type used to model a type-erased floating-point value
using float_type = wtf::fp::Float;

/// Type of a read-only, aliasing view of a type-erased floating-point value
using const_float_reference = wtf::fp::FloatView<const float_type>;

/// Type of a mutable, aliasing view of a type-erased floating-point value
using float_reference = wtf::fp::FloatView<float_type>;

/** @brief Overwrites the value @p lhs aliases with the value of @p rhs.
 *
 *  wtf::fp::FloatView::operator= is overloaded on what it is given: handed
 *  another FloatView it *rebinds*, and handed a concrete value it *writes
 *  through*. Views of points always want the latter, and a caller which only
 *  has type-erased values on both sides has no concrete value to hand it.
 *  This function closes that gap: it restores the concrete type of both
 *  operands and does the write through.
 *
 *  @param[in,out] lhs A view of the value to overwrite.
 *  @param[in] rhs The value to copy into what @p lhs aliases.
 *
 *  @throw std::runtime_error if @p lhs and @p rhs are not holding the same
 *                            concrete floating-point type, or if that type is
 *                            not one chemist knows about. Strong throw
 *                            guarantee.
 */
void assign(float_reference lhs, const_float_reference rhs);

/** @brief Deep copies the value @p value aliases into an owning Float.
 *
 *  wtf::fp::Float can only be constructed from a concrete floating-point
 *  value, so copying a view into an owning value requires restoring the
 *  concrete type first.
 *
 *  @param[in] value The view to copy out of.
 *
 *  @return A Float owning a copy of the aliased value.
 *
 *  @throw std::runtime_error if @p value is not holding a floating-point type
 *                            chemist knows about. Strong throw guarantee.
 */
float_type copy(const_float_reference value);

/** @brief Computes @p lhs + @p rhs.
 *
 *  @param[in] lhs The value being added to.
 *  @param[in] rhs The value being added.
 *
 *  @return A new Float holding the sum, in the same concrete type as the
 *          operands.
 *
 *  @throw std::runtime_error if @p lhs and @p rhs are not holding the same
 *                            concrete floating-point type, or if that type is
 *                            not one chemist knows about. Strong throw
 *                            guarantee.
 */
float_type add(const_float_reference lhs, const_float_reference rhs);

/** @brief Computes @p lhs - @p rhs.
 *
 *  @param[in] lhs The value being subtracted from.
 *  @param[in] rhs The value being subtracted.
 *
 *  @return A new Float holding the difference, in the same concrete type as
 *          the operands.
 *
 *  @throw std::runtime_error under the same conditions as add. Strong throw
 *                            guarantee.
 */
float_type subtract(const_float_reference lhs, const_float_reference rhs);

/** @brief Computes @p lhs * @p rhs.
 *
 *  @param[in] lhs The left operand.
 *  @param[in] rhs The right operand.
 *
 *  @return A new Float holding the product, in the same concrete type as the
 *          operands.
 *
 *  @throw std::runtime_error under the same conditions as add. Strong throw
 *                            guarantee.
 */
float_type multiply(const_float_reference lhs, const_float_reference rhs);

/** @brief Computes the square root of @p value.
 *
 *  For the uncertainty-quantification types this dispatches to
 *  tensorwrapper::types::pow(value, 0.5), which knows how to propagate
 *  uncertainty through the operation; for the built-in types it is
 *  std::pow(value, 0.5).
 *
 *  @param[in] value The value to take the square root of.
 *
 *  @return A new Float holding the square root, in the same concrete type as
 *          @p value.
 *
 *  @throw std::runtime_error if @p value is not holding a floating-point type
 *                            chemist knows about. Strong throw guarantee.
 */
float_type sqrt(const_float_reference value);

/** @brief Converts @p value to a string without losing precision.
 *
 *  wtf::fp::FloatView::to_string uses the default stream precision, which is
 *  six significant figures and thus silently truncates a double-precision
 *  coordinate. This function restores the concrete type first so that it can
 *  ask std::numeric_limits how many digits that type actually needs. Types
 *  for which std::numeric_limits is not specialized (the Sigma
 *  uncertainty-quantification types) fall back to the default precision.
 *
 *  @param[in] value The value to convert.
 *
 *  @return A string representation of @p value.
 *
 *  @throw std::runtime_error if @p value is not holding a floating-point type
 *                            chemist knows about. Strong throw guarantee.
 *  @throw std::bad_alloc if there is a problem allocating the string. Strong
 *                        throw guarantee.
 */
std::string to_string(const_float_reference value);

} // namespace chemist::experimental::detail_
