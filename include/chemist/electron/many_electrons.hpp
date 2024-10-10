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
#include <chemist/electron/electron_class.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {

/** @brief A container filled with Electron objects.
 *
 *  Electron objects represent a single electron. The ManyElectron object
 *  describes an indeterminate number of electrons.
 */
class ManyElectrons : public utilities::IndexableContainerBase<ManyElectrons> {
private:
    /// Type *this derives from
    using base_type = utilities::IndexableContainerBase<ManyElectrons>;

public:
    /// Type of each electron in *this
    using value_type = Electron;

    /// Type acting like a mutable reference to an object of type value_type.
    using reference_type = value_type;

    /// Type acting like a read-only reference to an object of type value_type.
    using const_reference_type = value_type;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /** @brief Creates a new ManyElectrons object managing @p n_electrons.
     *
     *  At present the only state of a ManyElectrons object is the number of
     *  electrons. This ctor allows you to create a ManyElectrons object with
     *  the specified number of electrons.
     *
     *  @param[in] n_electrons The number of electrons. If not provided defaults
     *                         to 0.
     *
     *  @throw None No throw guarantee.
     */
    explicit ManyElectrons(size_type n_electrons = 0) :
      m_n_electrons_(n_electrons) {}

    /** @brief Determines if *this and @p rhs are value equal.
     *
     *  Two ManyElectrons objects are value equal if they both represent the
     *  same number of electrons (and if the electrons in them are value equal).
     *  Since Electron objects are all identical this method simply ensures
     *  that the number of electrons is the same.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ManyElectrons& rhs) const noexcept {
        return m_n_electrons_ == rhs.m_n_electrons_;
    }

    /** @brief Determines if *this is different than @p rhs.
     *
     *  Two ManyElectrons objects are different if they are not value equal.
     *  This method simply negates the result of operator==, see operator== for
     *  the definition of value equal.
     *
     *  @param[in] rhs The object to compare against.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const ManyElectrons& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// Allows base class to access implementations
    friend base_type;

    /// Implements "mutable" at and operator[]
    reference_type at_(size_type) { return Electron{}; }

    /// Implements read-only at and operator[]
    const_reference_type at_(size_type) const { return Electron{}; }

    /// Implements size
    size_type size_() const noexcept { return m_n_electrons_; }

    /// The number of electrons in *this
    size_type m_n_electrons_ = 0;
};

} // namespace chemist