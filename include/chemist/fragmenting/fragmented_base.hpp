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
#include <chemist/traits/fragmenting_traits.hpp>
#include <optional>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist::fragmenting {

/** @brief Code factorization for classes containing fragments.
 *
 *  @note Design notes can be found at https://tinyurl.com/2xpbayrp
 *
 *  FragmentedNuclei, FragmentedMolecule, etc. share several common methods.
 *  Rather than re-implementing them for each class, we have factored those
 *  methods into this class and implement them once for all the classes.
 *
 *  @tparam DerivedClass The class *this is implementing. For example, when
 *                       implementing `FragmentedNuclei`, @p DerivedClass will
 *                       be `FragmentedNuclei`.
 *
 */
template<typename DerivedClass>
class FragmentedBase : public utilities::IndexableContainerBase<DerivedClass> {
private:
    // Type of the base class
    using base_type = utilities::IndexableContainerBase<DerivedClass>;

public:
    /// Type defining the traits for the derived class
    using traits_type = ChemistClassTraits<DerivedClass>;

    /// Type defining the traits for the class being fragmented
    using supersystem_traits = typename traits_type::supersystem_traits;

    /// Type the derived class fragments
    using supersystem_type = typename supersystem_traits::value_type;

    /// Type of a mutable reference to the supersystem
    using supersystem_reference = typename supersystem_traits::view_type;

    /// Type of a read-only reference to the supersystem
    using const_supersystem_reference =
      typename supersystem_traits::const_view_type;

    /** @brief Fragments an empty object.
     *
     *
     *  @throw None No throw guarantee.
     */
    FragmentedBase() noexcept = default;

    /** @brief Provides (possibly) mutable access to the supersystem.
     *
     *  This method is used to access the supersystem stored in *this. Whether
     *  the resulting supersystem is mutable or not depends on the type of the
     *  derived class. It should be noted that because NucleiView, MoleculeView,
     *  etc. do not allow you to add new elements to the supersystem, the
     *  resulting reference can only be used to update the properties of
     *  already existing elements.
     *
     *  @warning Modifying the supersystem may invalidate fragment information.
     *           At present this class makes no attempt to ensure the new
     *           supersystem state is consistent with other state in the
     *           derived class.
     *
     *  @return A (possibly) mutable reference to the supersystem.
     *
     *  @throw None No throw guarantee
     */
    supersystem_reference supersystem() { return supersystem_(); }

    /** @brief Provides read-only access to the supersystem.
     *
     *  This method is used to access the supersystem in a read-only manner.
     *
     *  @return A read-only reference to the supersystem.
     *
     *  @throw None No throw guarantee
     */
    const_supersystem_reference supersystem() const { return supersystem_(); }

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  This is a no-op at the moment.
     *
     *  @param[in,out] other The object whose state is being exchanged with.
     *                       After this method is called @p other will contain
     *                       the state which was previously in *this.
     *
     *  @throw None No throw guarantee.
     */
    void swap(FragmentedBase& other) noexcept {}

    /** @brief Are *this and @p rhs value equal?
     *
     *  *this and @p rhs will be considered value equal if both have
     *  supersystems which compare value equal.
     *
     *  @param[in] rhs The object to compare *this against.
     *
     *  @return True if *this and @p rhs are value equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const FragmentedBase& rhs) const noexcept {
        return supersystem() == rhs.supersystem();
    }

    /** @brief Are *this and @p rhs different?
     *
     *  This method defines "different" as not value equal and thus negates
     *  the result of FragmentedBase::operator==. See the description for
     *  FragmentedBase::operator== for more details.
     *
     *  @param[in] rhs The object to compare against.
     *
     *  @return False if *this and @p rhs are value equal and true otherwise.
     *
     *  @throw None No throw guarantee.
     *
     */
    bool operator!=(const FragmentedBase& rhs) const noexcept {
        return !((*this) == rhs);
    }

protected:
    /// Derived class should override to implement supersystem()
    virtual supersystem_reference supersystem_() = 0;

    /// Derived class should override to implement supersystem() const
    virtual const_supersystem_reference supersystem_() const = 0;
};

} // namespace chemist::fragmenting
