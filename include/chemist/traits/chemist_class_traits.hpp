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

/** @brief Used to set the member types for a chemist object of type @p T.
 *
 *  @tparam T The cv-qualified type of the object whose types we want.
 *
 *  In order to set the member types of each chemist object once and then have
 *  them propagate throughout the library we specialize ChemistClassTraits for
 *  every object in the Chemist library.
 */
template<typename T>
class ChemistClassTraits;

} // namespace chemist
