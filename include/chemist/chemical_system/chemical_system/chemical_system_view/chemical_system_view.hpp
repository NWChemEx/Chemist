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
#include <chemist/chemical_system/chemical_system/chemical_system_class.hpp>
namespace chemist {
namespace detail_ {
template<typename ChemicalSystemType>
class ChemicalSystemViewPIMPL;
}

template<typename ChemicalSystemType>
class ChemicalSystemView {
private:
    /// Type of *this
    using my_type = ChemicalSystemView<ChemicalSystemType>;

    /// Type trait for determining if @p T is cv-qualified
    template<typename T>
    static constexpr bool is_cv_v = !std::is_same_v<std::remove_cv_t<T>, T>;

    /// Is *this aliasing a const ChemicalSystem?
    static constexpr bool const_alias_v = is_cv_v<ChemicalSystemType>;

    /// Enables a function if const_alias_v and !is_cv_v<T>
    template<typename T>
    using is_read_only_conversion_t =
      std::enable_if_t<const_alias_v, is_cv_v<T>>;

public:
    /// Type of the PIMPL
    using pimpl_type = detail_::ChemicalSystemViewPIMPL<ChemicalSystemType>;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of a struct defining the types for the aliased ChemicalSystem
    using type_traits = ChemistClassTraits<ChemicalSystemType>;

    /// Type of a struct defining the types for the Molecule piece of *this
    using molecule_traits = typename type_traits::molecule_traits;

    /// Type *this aliases
    using chemical_system_type = typename type_traits::value_type;

    /// A read-only reference to the type *this aliases
    using const_chemical_system_reference = typename type_traits::reference;

    /// Type of the Molecule piece of *this
    using molecule_type = typename molecule_traits::value_type;

    /// Type of a mutable reference to the Molecule piece of *this
    using molecule_reference = typename molecule_traits::view_type;

    /// Type of a read-only reference to the Molecule piece of *this
    using const_molecule_reference = typename molecule_traits::const_view_type;

    // ChemicalSystemView(chemical_system_type chemical_system);

    // explicit ChemicalSystemView(molecule_reference molecule);

    // template<typename ChemicalSystemType2,
    //          typename = is_read_only_conversion_t<ChemialSystemType2>>
    //  ChemicalSystemView(const ChemicalSystemView<ChemicalSystemType2>&
    //  other);

    // molecule_reference molecule();

    // const_molecule_reference molecule() const;

    // bool operator==(const ChemicalSystemView& rhs) const noexcept;

    // bool operator==(const_chemical_system_reference rhs) const noexcept;
private:
    /// The object actually implementing *this
    // pimpl_pointer m_pimpl_;
};

} // namespace chemist
