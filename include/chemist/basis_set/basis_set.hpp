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
#include <chemist/basis_set/ao_basis_set.hpp>
#include <chemist/basis_set/atomic_basis_set.hpp>
#include <chemist/basis_set/atomic_basis_set_view.hpp>
#include <chemist/basis_set/contracted_gaussian.hpp>
#include <chemist/basis_set/contracted_gaussian_view.hpp>
#include <chemist/basis_set/primitive.hpp>
#include <chemist/basis_set/primitive_view.hpp>
#include <chemist/basis_set/shell.hpp>
#include <chemist/basis_set/shell_view.hpp>

/** @brief Contains classes associated with the electronic basis set.
 *
 *  The `basis_set` namespace encloses pieces of Chemist which are part of the
 *  basis set component. The architecture of the basis set component is
 *  described in more detail (TODO: Add link)
 */
namespace chemist::basis_set {

/** @brief Contains implementation details for the basis set component.
 *
 *  Source code contained in the basis_set::detail_ namespace is needed for
 *  implementing the basis set component, but should not be considered part of
 *  the public API. This means that code in the detail_ namespace is subject
 *  to change without notice and that users should not directly call it. If as
 *  a user you are trying to call code in the basis_set::detail_ namespace you
 *  are probably doing something wrong.
 */
namespace detail_ {}

} // namespace chemist::basis_set
