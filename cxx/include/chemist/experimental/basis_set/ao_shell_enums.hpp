/*
 * Copyright 2026 NWChemEx-Project
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

namespace chemist::experimental {

/** @brief The kinds of AO a shell can hold.
 *
 *  For a concrete shell the purity is part of its type, e.g.
 *  CCAShell<CartesianAO> versus CCAShell<SphericalAO>. Containers of shells
 *  which are not templated on the shell type, e.g. AtomicBasisSet, take this
 *  enumerator instead and use it to select the shell type at runtime.
 */
enum class ShellPurity {
    /// The shell holds Cartesian AOs
    cartesian,
    /// The shell holds pure (spherical) AOs
    pure
};

/** @brief The orders a shell can enumerate its AOs in.
 *
 *  Per docs/source/developer/design/basis_set/ao_hierarchy.rst, each ordering
 *  is a class template deriving from AOShell<AOType>. This enumerator names
 *  those classes, for the same reason ShellPurity exists.
 */
enum class AOOrdering {
    /// The Common Component Architecture ordering, i.e. CCAShell
    cca
};

} // namespace chemist::experimental
