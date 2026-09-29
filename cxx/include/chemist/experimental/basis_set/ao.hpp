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
#include <chemist/experimental/basis_set/ao_common.hpp>
#include <memory>

namespace chemist::experimental {

/** @brief Abstract base class of the atomic orbitals which own their state.
 *
 *  An atomic orbital comes in two kinds: a CartesianAO, which pairs a
 *  contracted Gaussian with a monomial @f$x^i y^j z^k@f$, and a SphericalAO,
 *  which is a linear combination of the Cartesian AOs of its shell. They are
 *  built differently and carry different state, but most code does not care
 *  which it has --- it wants the angular momentum, the center, the contracted
 *  Gaussian, the normalization constant, or the value at a point, all of
 *  which both kinds have. *this is the common base which lets such code be
 *  written once, following the same idiom as VectorSpace and OperatorBase.
 *
 *  The shared API itself lives in AOCommon, which *this and AOView both
 *  derive from; what *this adds is the polymorphism: the pure virtual methods
 *  AOCommon forwards to, plus clone and comparison.
 *
 *  Note *this can not be instantiated, and a caller therefore always holds an
 *  AO by reference or through a base_pointer. See AOView for the counterpart
 *  hierarchy whose members alias state owned by something else.
 */
class AO : public AOCommon<AO> {
private:
    /// Type *this inherits from
    using base_type = AOCommon<AO>;

    /// Lets the CRTP base reach the virtual methods below
    friend base_type;

public:
    /// Type of a pointer to an AO, which is how a polymorphic AO is owned
    using base_pointer = std::unique_ptr<AO>;

    /// Type of a, possibly read-only, reference to an AO
    ///@{
    using base_reference       = AO&;
    using const_base_reference = const AO&;
    ///@}

    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::angular_momentum_type;
    using typename base_type::center_type;
    using typename base_type::const_center_reference;
    using typename base_type::const_contracted_gaussian_reference;
    using typename base_type::const_point_reference;
    using typename base_type::contracted_gaussian_type;
    using typename base_type::coord_type;
    using typename base_type::numerical_value;
    using typename base_type::numerical_vector;
    using typename base_type::size_type;
    ///@}

    /// Polymorphic, no-throw dtor
    virtual ~AO() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a deep copy of *this, as an AO.
     *
     *  Because *this owns its state the copy is independent of *this:
     *  mutating one does not affect the other.
     *
     *  @return A pointer to a newly allocated copy of *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    base_pointer clone() const { return clone_(); }

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two AOs are value equal if they are the same kind of AO and if the
     *  state of that kind is value equal. An AO is never value equal to an AO
     *  of a different kind, even if the two happen to describe the same
     *  function, e.g., a CartesianAO s-orbital and a SphericalAO s-orbital are
     *  not value equal even though they are mathematically identical.
     *
     *  @param[in] rhs The AO to compare to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal(const_base_reference rhs) const noexcept {
        return are_equal_(rhs);
    }

    /** @brief Determines if *this differs from @p rhs.
     *
     *  This method defines "different" as not value equal. See are_equal for
     *  the definition of value equal.
     */
    bool are_different(const_base_reference rhs) const noexcept {
        return !are_equal(rhs);
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    AO() noexcept                     = default;
    AO(const AO&) noexcept            = default;
    AO(AO&&) noexcept                 = default;
    AO& operator=(const AO&) noexcept = default;
    AO& operator=(AO&&) noexcept      = default;
    ///@}

    /// Implements get_l
    virtual angular_momentum_type get_l_() const noexcept = 0;

    /// Implements get_center
    virtual const_center_reference get_center_() const = 0;

    /// Implements get_contracted_gaussian
    virtual const_contracted_gaussian_reference get_contracted_gaussian_()
      const = 0;

    /// Implements normalization_constant
    virtual numerical_value normalization_constant_() const = 0;

    /// Implements evaluate(point)
    virtual numerical_value evaluate_(const_point_reference r) const = 0;

    /// Implements normalized_evaluate(point)
    virtual numerical_value normalized_evaluate_(
      const_point_reference r) const = 0;

    /// Implements clone
    virtual base_pointer clone_() const = 0;

    /// Implements are_equal
    virtual bool are_equal_(const_base_reference rhs) const noexcept = 0;
};

} // namespace chemist::experimental
