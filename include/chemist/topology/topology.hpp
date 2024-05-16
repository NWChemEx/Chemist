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

/** @file topology.hpp
 *
 *  Convenience header file for including the topology subcomponent of Chemist.
 */

#include <chemist/topology/connectivity_table.hpp>

/** @brief Namespace for classes and functions describing the topology of
 *         chemical systems.
 *
 *  Colloquially, topology is how parts of a system are related/arranged.
 *  Mathematically, topology is the study of the properties of geometric objects
 *  which are preserved under continuous deformations. When we speak of the
 *  "topology of a chemical system" we use the term somewhere in between these
 *  two definitions. More specifically, the topology of a chemical system refers
 *  to the spatial properties of a chemical system that do NOT depend on
 *  how the system is represented, e.g., on the coordinates of the constituent
 *  nuclei, the choice for x-axis, etc. Such properties include, but are not
 *  limited to:
 *
 *  - Which pairs of atoms are bonded
 *  - Which pairs of bonds share a common atom (i.e,. a bonded angle)
 *  - Which pairs of bonded-angles share a common bond (i.e., a proper torsional
 *    angle)
 *
 *  The topology component of Chemist contains classes for working out and
 *  describing the topology of chemical systems.
 */
namespace chemist::topology {}
