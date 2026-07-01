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

namespace chemist {

/** @brief Represents a single electron.
 *
 *  The Electron class is essentially a strong type used to create other types.
 *  Each object has no state and some minimal features.
 */
class Electron {
public:
    /** @brief Determines if *this is value equal to another object.
     *
     *  At present all Electron objects are value equal. This is because
     *  electrons are indistinguishable particles.
     *
     *  @param[in] rhs The object to compare to (at present not actually used).
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Electron&) const noexcept { return true; }

    /** @brief Is *this different from @p rhs?
     *
     *  Two Electron objects are different if they are not equal.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return False if *this and @p rhs are value equal and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const Electron& rhs) const noexcept {
        return !(*this == rhs);
    }
};

} // namespace chemist
