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

namespace chemist::wavefunction {

/** @brief Represents a wavefunction from a VectorSpace.
 *
 *  This class is at present a stub that we need so we can implement BraKet.
 *  It will be fleshed out in a forthcoming PR.
 */
class Wavefunction {
public:
    /// Type all wavefunctions inherit from
    using base_type = Wavefunction;

    /// Type acting like a mutable reference to a base_type object
    using base_reference = base_type&;

    /// Type acting like a read-only reference to a base_type object
    using const_base_reference = const base_type&;
};

} // namespace chemist::wavefunction