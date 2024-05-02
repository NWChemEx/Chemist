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
#include <chemist/fragmenting/capping/capping.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>

namespace chemist::fragmenting {
namespace detail_ {
class FragmentedNucleiPIMPL;
}

class FragmentedNuclei : public FragmentedBase<FragmentedNuclei> {
private:
    /// Type *this derives from
    using base_type = FragmentedBase<FragmentedNuclei>;

public:
    /// Type of the object defining types associated with *this
    using typename base_type::traits_type;

    /// Type of the object implementing *this
    using pimpl_type = detail_::FragmentedNucleiPIMPL;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of object *this is fragmenting
    using typename base_type::supersystem_type;

    /// Type of a mutable reference to a fragment
    using reference = typename traits_type::fragment_reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename traits_type::const_fragment_reference;

    /// Type of the object holding the caps for the fragments
    using cap_set_type = traits_type::cap_set_type;

    /// Type supersystem uses for indexing and offsets
    using size_type = typename supersystem_type::size_type;

    /// Type of a map from fragment index to nuclei offset
    using fragment_map_type = std::vector<std::vector<size_type>>;

    // -- CTors, assignment, and dtor

    FragmentedNuclei() noexcept;

    explicit FragmentedNuclei(supersystem_type supersystem);

    FragmentedNuclei(supersystem_type supersystem, fragment_map_type frags);

    FragmentedNuclei(supersystem_type supersystem, fragment_map_type frags,
                     cap_set_type caps);

    FragmentedNuclei(pimpl_pointer pimpl);

    ~FragmentedNuclei() noexcept;

protected:
    reference at_(size_type i) override;

    const_reference at_(size_type i) const override;

    size_type size_() const noexcept override;

private:
    /// Factors out the code for checking if *this has a PIMPL
    bool has_pimpl_();

    pimpl_pointer m_pimpl_;
};

} // namespace chemist::fragmenting
