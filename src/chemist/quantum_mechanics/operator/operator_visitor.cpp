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

#include <chemist/quantum_mechanics/operator/operator.hpp>
#include <chemist/quantum_mechanics/wavefunction/wavefunction.hpp>
namespace chemist::qm_operator {

#define OVERLOAD(...)                                         \
    void OperatorVisitor::run(__VA_ARGS__& visitor) {         \
        return run(static_cast<const __VA_ARGS__&>(visitor)); \
    }

#define CONST_OVERLOAD(...)                                                  \
    void OperatorVisitor::run(const __VA_ARGS__&) {                          \
        if(!m_throw_) return;                                                \
        throw std::runtime_error(                                            \
          std::string("Visitor does not support operators of type const ") + \
          std::string(#__VA_ARGS__));                                        \
    }

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
#define DENSITY_OVERLOADS(T)                                 \
    OVERLOADS(T<Electron, chemist::Density<Electron>>);      \
    OVERLOADS(T<ManyElectrons, chemist::Density<Electron>>); \
    OVERLOADS(T<Electron, DecomposableDensity<Electron>>);   \
    OVERLOADS(T<ManyElectrons, DecomposableDensity<Electron>>)

OVERLOADS(Density<wavefunction::MOs, Electron>);
OVERLOADS(Density<wavefunction::CMOs, Electron>);
ONE_PARTICLE_OVERLOADS(Kinetic);
TWO_PARTICLE_OVERLOADS(Coulomb);
DENSITY_OVERLOADS(Coulomb);
DENSITY_OVERLOADS(Exchange);
TWO_PARTICLE_OVERLOADS(ExchangeCorrelation);

#undef DENSITY_OVERLOADS
#undef TWO_PARTICLE_OVERLOADS
#undef ONE_PARTICLE_OVERLOADS
#undef OVERLOADS
#undef CONST_OVERLOAD
#undef OVERLOAD

} // namespace chemist::qm_operator