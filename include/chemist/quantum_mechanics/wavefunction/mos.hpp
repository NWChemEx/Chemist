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
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>
#include <chemist/quantum_mechanics/wavefunction/transformed.hpp>

namespace chemist::wavefunction {

/** @brief Space spanned by the molecular orbitals (MOs).
 *
 *  This is a strong type to represent that a particular set of transformed
 *  AOs are MOs.
 */
class MOs : public Transformed<AOs> {
private:
    /// Type of the base class
    using transformed_type = Transformed<AOs>;

public:
    /// Pull in types from the base class
    ///@{
    using typename transformed_type::base_pointer;
    using typename transformed_type::size_type;
    ///@}

    /// Reuse base class's ctor
    using transformed_type::transformed_type;

protected:
    /// Implements clone by calling copy ctor
    base_pointer clone_() const override {
        return std::make_unique<MOs>(*this);
    }

    /// Implements are_equal by calling are_equal_impl_
    bool are_equal_(const VectorSpace& rhs) const noexcept override {
        return are_equal_impl_<MOs>(rhs);
    }
};

} // namespace chemist::wavefunction