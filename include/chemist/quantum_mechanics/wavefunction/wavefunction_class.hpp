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

/** @brief Represents a wavefunction from a VectorSpace.
 *
 *  VectorSpace objects are spanned by vectors. This class represents one of
 *  those vectors with the name "wavefunction" chosen to avoid collisions with
 *  std::vector and/or vector classes which live in math libraries.
 */
class Wavefunction {
public:
    /// Type all wavefunctions inherit from
    using base_type = Wavefunction;

    /// Type acting like a mutable reference to a base_type object
    using base_reference = base_type&;

    /// Type acting like a read-only reference to a base_type object
    using const_base_reference = const base_type&;

    /// Type of a pointer to an object of base_type
    using base_pointer = std::unique_ptr<base_type>;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /// Defaulted polymorphic dtor
    virtual ~Wavefunction() noexcept = default;

    /** @brief Returns a deep polymorphic copy of *this.
     *
     *  A deep polymorphic copy of an object is a deep copy of an entire object
     *  including state which resides in derived classes. By contrast a non-
     *  polymorphic deep copy excludes state which resides in derived classes.
     *  This function performs a deep polymorphic copy; for non-polymorphic
     *  deep copies use the copy ctor.
     *
     *  @return A deep polymorphic copy of *this returned as a pointer to the
     *          base object.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     */
    base_pointer clone() const { return clone_(); }

    /** @brief Implements polymorphic value equality.
     *
     *  Two Wavefunction objects are polymorphically value equal if their most-
     *  derived parts are of the same type and if when compared via their most-
     *  derived class they compare value equal.
     *
     *  @param[in] rhs The wavefunction to compare to.
     *
     *  @return True if *this is polymorphically value equal to @p rhs and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal(const_base_reference rhs) const noexcept {
        return are_equal_(rhs);
    }

    /** @brief Determines if *this is polymorphically different than @p rhs.
     *
     *  Two Wavefunction objects are polymorphically different if they are not
     *  polymorphically value equal. See are_equal() for the definition of
     *  polymorphically value_equal.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return False if *this is polymorphically value equal to @p rhs and
     *          true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_different(const_base_reference rhs) const noexcept {
        return !are_equal(rhs);
    }

protected:
    /// To be overridden by the derived class to be consistent with clone()
    virtual base_pointer clone_() const = 0;

    /// To be overriden by the derived class to be consistent with are_equal()
    virtual bool are_equal_(const_base_reference rhs) const noexcept = 0;
};

} // namespace chemist::wavefunction