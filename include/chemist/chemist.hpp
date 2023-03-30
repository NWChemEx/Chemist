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

/**
 * @file chemist.hpp
 *
 * This is a convenience header defining the public API of the chemist
 * library.  It should NOT be included in any other chemist header file,
 * source file, or test (the exception being Testchemist, which tests the
 * functions defined in this header file).
 */

#include "chemist/basis_set/basis_set.hpp"
#include "chemist/chemical_system/chemical_system.hpp"
#include "chemist/integral_factory/integral_factory.hpp"
#include "chemist/integral_factory/integral_factory_pimpl.hpp"
#include "chemist/molecule/molecule.hpp"
#include "chemist/operators/operators.hpp"
#include "chemist/orbital_space/orbital_space.hpp"
#include "chemist/topology/topology.hpp"
#include "chemist/wavefunction/wavefunction.hpp"

/**
 * @brief The primary namespace for the chemist library
 *
 * All functionality of the chemist library intended for end user consumption
 * is defined within this namespace.
 *
 */
namespace chemist {} // namespace chemist
