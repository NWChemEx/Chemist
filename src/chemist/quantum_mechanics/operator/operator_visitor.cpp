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

namespace chemist::qm_operator {

#define OVERLOAD(T)                                 \
    void OperatorVisitor::run(T& visitor) {         \
        return run(static_cast<const T&>(visitor)); \
    }

#define CONST_OVERLOAD(T)                                                    \
    void OperatorVisitor::run(const T&) {                                    \
        throw std::runtime_error(                                            \
          std::string("Visitor does not support operators of type const ") + \
          std::string(#T));                                                  \
    }

#define OVERLOADS(T) \
    OVERLOAD(T);     \
    CONST_OVERLOAD(T)

#define ONE_PARTICLE_OVERLOADS(T) \
    OVERLOADS(T<Electron>);       \
    OVERLOADS(T<ManyElectrons>);  \
    OVERLOADS(T<Nucleus>);        \
    OVERLOADS(T<Nuclei>)

ONE_PARTICLE_OVERLOADS(Kinetic);

#undef ONE_PARTICLE_OVERLOADS
#undef OVERLOADS
#undef CONST_OVERLOAD
#undef OVERLOAD

} // namespace chemist::qm_operator