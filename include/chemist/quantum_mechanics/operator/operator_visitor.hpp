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
#include <chemist/chemical_system/electron/electron.hpp>
#include <chemist/chemical_system/nucleus/nucleus.hpp>
#include <chemist/quantum_mechanics/operator/operator_fwd.hpp>

namespace chemist::qm_operator {

#define OVERLOAD(...) virtual void run(__VA_ARGS__&)
#define CONST_OVERLOAD(...) virtual void run(const __VA_ARGS__&);
#define OVERLOADS(...)     \
    OVERLOAD(__VA_ARGS__); \
    CONST_OVERLOAD(__VA_ARGS__)
#define ONE_PARTICLE_OVERLOADS(T) \
    OVERLOADS(T<Electron>);       \
    OVERLOADS(T<ManyElectrons>);  \
    OVERLOADS(T<Nucleus>);        \
    OVERLOADS(T<Nuclei>)
#define TWO_PARTICLE_OVERLOADS(T)               \
    OVERLOADS(T<Electron, Electron>);           \
    OVERLOADS(T<ManyElectrons, ManyElectrons>); \
    OVERLOADS(T<Electron, Nuclei>);             \
    OVERLOADS(T<ManyElectrons, Nuclei>);        \
    OVERLOADS(T<Nuclei, Nuclei>)

/** @brief Base class for all operator visitors.
 *
 *  If a user wants to create a visitor for interacting with operators in a
 *  generic manner they should derive from this class and then override the
 *  `run` methods for the operators they want to handle. For an operator of
 *  type T there are two possible overloads the user can override:
 *
 *  1. `void run(T&)`
 *  2. `void run(const T&)`
 *
 *  It should be noted that OperatorVisitor defines the first overload to call
 *  the second, i.e., if you will not modify the operator just overload the
 *  read-only version. By default OperatorVisitor implements the second overload
 *  to throw an error, i.e., if you don't want to handle an operator no need to
 *  overload the function.
 */
class OperatorVisitor {
public:
    ONE_PARTICLE_OVERLOADS(Kinetic);
    TWO_PARTICLE_OVERLOADS(Coulomb);
    TWO_PARTICLE_OVERLOADS(Exchange);
    TWO_PARTICLE_OVERLOADS(ExchangeCorrelation);
};

#undef TWO_PARTICLE_OVERLOADS
#undef ONE_PARTICLE_OVERLOADS
#undef OVERLOADS
#undef CONST_OVERLOAD
#undef OVERLOAD
} // namespace chemist::qm_operator