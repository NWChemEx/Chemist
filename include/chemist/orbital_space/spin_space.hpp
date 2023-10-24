/*
 * Copyright 2022 NWChemEx-Project
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
#include "chemist/orbital_space/base_space.hpp"

namespace chemist::orbital_space {

/** @brief Models a space spanned by the spin state of a particle.
 *
 *  Here the spin space is not the space consist of the spin eigenfunctions
 *  of an N-electron system with a dimension of 2^N.
 *
 *  @tparam N Dimension of the space.
 */
class SpinSpace : public BaseSpace {
private:
    /// enum variable to represent alpha and beta spin functions
    enum spin_type { alpha, beta };
    // spin_type spin;
    double tot_spin = 0.5;

public:
    /// Type used for indexing and offsets
    using size_type = typename BaseSpace::size_type;

    /** @brief Creates a SpinSpace.
     *
     *  @throw ??? Throws if initialization of the enum and double
     *  variables throws.
     */
    SpinSpace(){};

    /** @brief Dimension of the cartesian space
     *
     *  @return The dimension.
     */
    auto TSpin() { return tot_spin; }

    const char* EnumToStr(spin_type type) {
        switch(type) {
            case spin_type::alpha: return "alpha";
            case spin_type::beta: return "beta";
        }
    }

    auto SpinType(int i) { return EnumToStr(static_cast<spin_type>(i)); }

protected:
    size_type size_() const noexcept override { return 2; }
    bool equal_(const BaseSpace& rhs) const noexcept override {
        return this->equal_common(*this, rhs);
    }
};

// bool operator==(const SpinSpace& lhs,const SpinSpace& rhs) {
//     return true;
//}

} // namespace chemist::orbital_space