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
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <cstddef>
#include <wtf/wtf.hpp>

/** @file gaussian_arithmetic.hpp
 *
 *  Implements the primitive normalization constant described in
 *  docs/source/developer/design/basis_set/normalization.rst. Declared here,
 *  rather than in float_arithmetic.hpp, because this is specific to the
 *  Gaussian primitive component and not something a bare coordinate would
 *  ever need.
 */

namespace chemist::experimental::detail_ {

/** @brief Computes the primitive normalization constant, @f$N^{\chi}@f$.
 *
 *  @f[
 *    N^{\chi}(\zeta; \ell) =
 *      \left(\frac{2\zeta}{\pi}\right)^{3/4}
 *      \sqrt{\frac{(4\zeta)^{\ell}}{(2\ell-1)!!}}
 *  @f]
 *
 *  This is the factor which scales a single Gaussian primitive, paired with
 *  its Cartesian polynomial, to unit norm. It does not include the
 *  contracted-Gaussian factor @f$N^{G}@f$ or the Cartesian-AO factor
 *  @f$N^{AO}_{ijk}@f$; those belong to the contraction and to the shell,
 *  respectively, and are not computable from a single primitive's parameters
 *  alone.
 *
 *  @param[in] exponent The primitive's exponent, @f$\zeta@f$.
 *  @param[in] l The total angular momentum, @f$\ell@f$, of the primitive.
 *
 *  @return The normalization constant, in the same concrete floating-point
 *          type as @p exponent.
 *
 *  @throw std::runtime_error if @p exponent is not holding a floating-point
 *                            type chemist knows about. Strong throw
 *                            guarantee.
 */
float_type primitive_normalization(const_float_reference exponent,
                                   std::size_t l);

/** @brief Computes the contracted-Gaussian normalization constant, N^G.
 *
 *  @f[
 *    N^{G}(\vec{d}, \vec{\zeta}; \ell) =
 *      \left[\sum_{p,q} d_p d_q\, S_{pq}\right]^{-1/2},
 *    \qquad
 *    S_{pq} = \left(\frac{2\sqrt{\zeta_p \zeta_q}}
 *                        {\zeta_p + \zeta_q}\right)^{\ell + 3/2}
 *  @f]
 *
 *  @p S_pq is the overlap of the @f$p@f$-th and @f$q@f$-th primitives, each
 *  already scaled by its own primitive_normalization; this does not need to
 *  call primitive_normalization itself since the @f$\ell + 3/2@f$ exponent
 *  already encodes it. See
 *  docs/source/developer/design/basis_set/normalization.rst for the
 *  derivation.
 *
 *  @param[in] coefficients The contraction coefficients, @f$\vec{d}@f$.
 *  @param[in] exponents The primitive exponents, @f$\vec{\zeta}@f$. Must be
 *                        the same length as @p coefficients and hold the
 *                        same concrete floating-point type.
 *  @param[in] l The (shared) total angular momentum of the contraction.
 *
 *  @return The normalization constant, in the same concrete floating-point
 *          type as @p coefficients and @p exponents.
 *
 *  @throw std::runtime_error if @p coefficients and @p exponents are not the
 *                            same length, do not hold the same concrete
 *                            floating-point type, or hold a type chemist
 *                            does not know about. Strong throw guarantee.
 */
float_type contracted_gaussian_normalization(
  wtf::buffer::BufferView<const wtf::fp::Float> coefficients,
  wtf::buffer::BufferView<const wtf::fp::Float> exponents, std::size_t l);

} // namespace chemist::experimental::detail_
