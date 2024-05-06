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

    /** @brief Creates a null object.
     *
     *  Null objects in this class hierarchy have no supersystem associated
     *  with them. This should be contrasted with empty objects, which have a
     *  supersystem, but no fragments.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedBase() noexcept = default;

    /** @brief Creates and object which will hold fragments of @p supersystem.
     *
     *  The FragmentedBase class stores the object to fragment. This ctor,
     *  is used to initialize *this with the object to fragment. Of note, *this
     *  will own the object. This is because the lifetime of the fragments will
     *  be coupled to the object and to help ensure the user doesn't
     *  accidentally invalidate the fragments by modifying @p supersystem.
     *
     *  @param[in] supersystem The object which will be fragmented.
     *
     *  @throw None No throw guarantee.
     */
    explicit FragmentedBase(supersystem_type supersystem) noexcept :
      m_supersystem_(std::move(supersystem)) {}

    // FragmentedBase& operator=(const FragmentedBase& rhs) = default;

    // FragmentedBase& operator=(FragmentedBase&& rhs) noexcept = default;

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
     *  @throw std::bad_optional_access if *this is a null object. Strong throw
     *                                  guarantee.
     */
    supersystem_reference supersystem() { return m_supersystem_.value(); }

    /** @brief Provides read-only access to the supersystem.
     *
     *  This method is used to access the supersystem in a read-only manner.
     *
     *  @return A read-only reference to the supersystem.
     *
     *  @throw std::bad_optional_access if *this is a null object. Strong throw
     *                                  guarantee.
     */
    const_supersystem_reference supersystem() const {
        return m_supersystem_.value();
    }

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @param[in,out] other The object whose state is being exchanged with.
     *                       After this method is called @p other will contain
     *                       the state which was previously in *this.
     *
     *  @throw None No throw guarantee.
     */
    void swap(FragmentedBase& other) noexcept {
        m_supersystem_.swap(other.m_supersystem_);
    }

    /** @brief Is *this a null object?
     *
     *  Null objects have no supersystem associated with them. This method is
     *  used to determine if *this is a null object, i.e., if it doesn't have a
     *  supersystem.
     *
     *  @return True if *this is a null object and false otherwise.
     *
     *  @throw None No throw guarantee
     *
     */
    bool is_null() const noexcept { return !m_supersystem_.has_value(); }

    /** @brief Are *this and @p rhs value equal?
     *
     *  *this and @p rhs will be considered value equal if one of the following
     *  is true:
     *
     *  - Both are null
     *  - Both have supersystems and the supersystems are value equal
     *
     *  @param[in] rhs The object to compare *this against.
     *
     *  @return True if *this and @p rhs are value equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const FragmentedBase& rhs) const noexcept {
        if(is_null() != rhs.is_null()) return false;
        if(is_null()) return true;
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
    /// Code factorization for asserting *this has a supersystem
    void assert_supersystem_() const {
        if(!is_null()) return;
        throw std::runtime_error("Does not have a supersystem!");
    }

private:
    /// The supersystem being fragmented
    std::optional<supersystem_type> m_supersystem_;
};

} // namespace chemist::fragmenting
