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

namespace chemist {

/** @brief Class representing a single Electron
 *
 */
struct Electron {};

/** @brief Compares two Electron instances.
 *
 *  @relates Electron
 *
 *  Electron are indistinguishable particles and thus always compare
 *  equal.
 *
 *  @param[in] <anonymous> The Electron to compare. Since Electron instances
 *                         have no state the provided instance is not
 *                         considered.
 *
 * @return True.
 *
 * @throw None No throw guarantee.
 */
inline bool operator==(const Electron&, const Electron&) { return true; }

/** @brief Determines if two Electron instances are different.
 *
 *  @relates Electron
 *
 *  Electron are indistinguishable particles so they can't be different.
 *
 *  @param[in] <anonymous> The Electron instances to compare. The instances
 *                         provided will not be considered since it has no
 *                         state.
 *
 *  @return False.
 *
 *  @throw None No throw guaranee.
 */
inline bool operator!=(const Electron&, const Electron&) { return false; }

} // namespace chemist
