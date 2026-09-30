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
#include <chemist/experimental/basis_set/ao.hpp>
#include <chemist/experimental/basis_set/ao_common.hpp>
#include <memory>

namespace chemist::experimental {

/** @brief Abstract base class of the atomic orbitals which alias their state.
 *
 *  *this is to CartesianAOView and SphericalAOView what AO is to CartesianAO
 *  and SphericalAO: the base which lets code ask an atomic orbital about
 *  itself without knowing which kind it is. The shared API is in AOCommon,
 *  which *this and AO both derive from, so the two bases answer the same
 *  questions the same way.
 *
 *  The hierarchies are deliberately separate rather than *this deriving from
 *  AO. What an AOView adds is a guarantee --- that it does not own what it
 *  describes, and so is cheap to copy and observes writes made through
 *  whatever does own it --- and a function which requires that guarantee
 *  should be able to say so in its signature. Two consequences follow, and
 *  are the only places the two bases differ:
 *
 *  - clone() returns another AOView aliasing the same state, because a
 *    shallow copy is what copying a view means.
 *  - as_ao() is the way to get an owning AO out of *this, materializing the
 *    aliased state into a new object, exactly as
 *    ContractedGaussianView::as_contracted_gaussian does one level down.
 */
class AOView : public AOCommon<AOView> {
private:
    /// Type *this inherits from
    using base_type = AOCommon<AOView>;

    /// Lets the CRTP base reach the virtual methods below
    friend base_type;

public:
    /// Type of a pointer to an AOView, which is how a polymorphic view is held
    using base_pointer = std::unique_ptr<AOView>;

    /// Type of a, possibly read-only, reference to an AOView
    ///@{
    using base_reference       = AOView&;
    using const_base_reference = const AOView&;
    ///@}

    /// Type of a pointer to the owning AO *this can be materialized into
    using ao_pointer = typename AO::base_pointer;

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

    /// Polymorphic, no-throw dtor. Does not affect the aliased state.
    virtual ~AOView() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a new view aliasing the same state as *this.
     *
     *  The copy is shallow, because that is what copying a view means: the
     *  result aliases exactly what *this aliases, and writes made through
     *  either are visible through the other. Use as_ao when an independent
     *  object is wanted instead.
     *
     *  @return A pointer to a newly allocated view of the same state.
     *
     *  @throw std::bad_alloc if there is a problem allocating the view.
     *                        Strong throw guarantee.
     */
    base_pointer clone() const { return clone_(); }

    /** @brief Returns an AO owning a copy of the state *this aliases.
     *
     *  The result is the same kind of AO as *this --- a CartesianAOView
     *  materializes into a CartesianAO --- and is independent of whatever
     *  *this aliases.
     *
     *  @return A pointer to a newly allocated, owning AO.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    ao_pointer as_ao() const { return as_ao_(); }

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two views are value equal if they are views of the same kind of AO and
     *  if the state they alias is value equal. Whether they alias the *same*
     *  state is not considered, only whether the values agree.
     *
     *  @param[in] rhs The view to compare to *this.
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
    AOView() noexcept                         = default;
    AOView(const AOView&) noexcept            = default;
    AOView(AOView&&) noexcept                 = default;
    AOView& operator=(const AOView&) noexcept = default;
    AOView& operator=(AOView&&) noexcept      = default;
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

    /// Implements as_ao
    virtual ao_pointer as_ao_() const = 0;

    /// Implements are_equal
    virtual bool are_equal_(const_base_reference rhs) const noexcept = 0;
};

} // namespace chemist::experimental
