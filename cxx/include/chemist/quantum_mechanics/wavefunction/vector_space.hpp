/*
 * Copyright 2024 NWChemEx-Project
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
#include <memory>

namespace chemist::wavefunction {

/** @brief Base class for all vector spaces in the wavefunction component.
 *
 *  This class provides a generic interface for interaction with any vector
 *  space. It is envisioned as being particularly useful for driver routines
 *  which want to work with arbitrary vector spaces.
 *
 *  To define a new vector space:
 *
 *  1. Inherit from this class.
 *  2. Override clone_.
 *  3. Override size_.
 *  4. Override are_equal_.
 *
 *  See
 * https://nwchemex.github.io/Chemist/developer/design/quantum_mechanics/wavefunction/vector_space.html
 *  for design details.
 *
 */
class VectorSpace {
public:
    /// Type all vector spaces derive from
    using base_type = VectorSpace;

    /// Type of a pointer to an object of type *this. Type returned by clone
    using base_pointer = std::unique_ptr<base_type>;

    /// Type behaving like a mutable reference to an object of type base_type
    using base_reference = base_type&;

    /// Type behaving like a read-only reference to an object of type base_type
    using const_base_reference = const base_type&;

    /// Type used for indexing and offsets.
    using size_type = std::size_t;

    /// No-op, no-throw, defaulted default ctor
    VectorSpace() = default;

    /// No-op, no-throw, defaulted, polymorphic dtor
    virtual ~VectorSpace() noexcept = default;

    /** @brief Creates a polymorphic deep copy of *this.
     *
     *  VectorSpace objects are polymorphic. Thus to ensure that copies made
     *  through the base class copy all of the state we must define a
     *  polymorphic copy method. That's this method.
     *
     *  Derived classes should implement this method by overriding clone_.
     *
     *  @return A newly allocated deep copy of *this.
     *
     *  @throw std::bad_alloc if there is a problem copying *this. Strong throw
     *                        guarantee.
     */
    base_pointer clone() const { return clone_(); }

    /** @brief Determines the number of basis functions in *this.
     *
     *  Derived classes should implement this method by overriding size_.
     *
     *  @return The number of basis functions in *this.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return size_(); }

    /** @brief Used to determine if *this is polymorphically equal to @p rhs.
     *
     *  Two VectorSpace objects are polymorphically value equal if:
     *  - They both are instances of the same derived classes.
     *  - If the state of all derived classes compares value equal.
     *
     *  Derived classes should implement this method by overriding are_equal_
     *  so that it calls are_equal_impl_. are_equal_impl_ assumes the derived
     *  class defines operator==. Ensuring that both *this and rhs have the same
     *  most derived class --- i.e., this->are_equal(rhs) and
     * rhs.are_equal(*this) are both true --- is handled by this method and
     * derived classes should NOT test for symmetric equivalence.
     *
     *  @param[in] rhs The VectorSpace to compare to *this.
     *
     *  @return True If *this is polymorphically equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal(const VectorSpace& rhs) const noexcept {
        return are_equal_(rhs) && rhs.are_equal_(*this);
    }

    /** @brief Determines if *this is polymorphically different from @p rhs.
     *
     *  This method defines "polymorphically different" as "not value equal" and
     *  simply negates the result of are_equal.
     *
     *  @param[in] rhs The VectorSpace to compare against @p rhs.
     *
     *  @return False if *this polymorphically equal to @p rhs and true
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_different(const VectorSpace& rhs) const noexcept {
        return !are_equal(rhs);
    }

protected:
    /// Protected to help avoid slicing
    VectorSpace(const VectorSpace& other) = default;

    /// Protected to help avoid slicing
    VectorSpace(VectorSpace&& other) noexcept = default;

    /// Protected to help avoid slicing
    VectorSpace& operator=(const VectorSpace& other) = default;

    /// Protected to help avoid slicing
    VectorSpace& operator=(VectorSpace&& other) noexcept = default;

    /** @brief Common implementation for are_equal_
     *
     *  @tparam DerivedType The type of the derived class we are implementing
     *                      are_equal_ for. The `this` pointer is assumed to be
     *                      downcast-able to `const DerivedType*`.
     *
     *  Assume the derived class implements operator== to implement a const,
     *  no-throw, non-polymorphic value comparison. Then the polymorphic value
     *  comparison can be implemented the same way, regardless of the internals
     *  of the derived class, i.e., downcast to the derived class (if that fails
     *  return false since they're different types) and call operator==. This
     *  method factors out the logic for doing this to make it easier for
     *  derived classes to implement are_equal_.
     *
     *  @param[in] rhs The object to compare polymorphically to *this.
     *
     *  @return True if after downcasting *this to DerivedType *this compares
     *          value equal to @p rhs. False otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename DerivedType>
    bool are_equal_impl_(const VectorSpace& rhs) const noexcept {
        // If this cast fails the derived class made a programming error
        const auto* pthis = static_cast<const DerivedType*>(this);

        // This one can fail if rhs isn't the same derived type
        const auto* prhs = dynamic_cast<const DerivedType*>(&rhs);
        if(prhs == nullptr) return false; // Different types
        return (*pthis) == (*prhs);
    }

    /// Derived classes override this to be consistent with clone()'s docs
    virtual base_pointer clone_() const = 0;

    /// Derived classes override this to be consistent with size()'s docs
    virtual size_type size_() const noexcept = 0;

    /// Derived classes override this to be consistent with are_equal()'s docs
    virtual bool are_equal_(const VectorSpace& rhs) const noexcept = 0;
};

} // namespace chemist::wavefunction
