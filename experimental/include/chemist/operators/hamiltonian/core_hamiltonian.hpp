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
#include "chemist/operators/hamiltonian/electronic_hamiltonian.hpp"

namespace chemist::operators {

/** @brief Models the one-electron core (electronic) Hamiltonian
 *
 *  The core (electronic) Hamiltonian contains terms which are one-electron
 *  terms or sums of one-electron terms. In practice we rarely need the many-
 *  electron core Hamiltonian so by convention this class is the one-electron
 *  core Hamiltonian.
 */
class CoreHamiltonian : public ElectronicHamiltonian {
private:
    template<typename T>
    static constexpr bool is_hamiltonian_v = std::is_base_of_v<Hamiltonian, T>;

    template<typename T>
    using disable_if_hamiltonian_t = std::enable_if_t<!is_hamiltonian_v<T>>;

public:
    CoreHamiltonian()                       = default;
    CoreHamiltonian(const CoreHamiltonian&) = default;
    CoreHamiltonian(CoreHamiltonian&&)      = default;
    CoreHamiltonian& operator=(const CoreHamiltonian&) = default;
    CoreHamiltonian& operator=(CoreHamiltonian&&) = default;

    /** @brief Convenience ctor for creating a CoreHamiltonian from the
     *         ElectronicHamiltonian.
     *
     *  This function will initialize and add an ElectronKinetic operator
     *  if an NElectronKinetic operator is present in @p He, then loop over
     *  the NElectronNuclearAttraction operators in @p He and initialize and
     *  add corresponding ElectronNuclearAttraction operators for each.
     *
     *  @param[in] He The ElectronicHamiltonian we are converting.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        new instance. Strong throw guarantee.
     */
    explicit CoreHamiltonian(const ElectronicHamiltonian& He);

    template<typename OpType, typename... Args,
             typename = disable_if_hamiltonian_t<std::decay_t<OpType>>>
    explicit CoreHamiltonian(OpType&& op0, Args&&... args);

protected:
    std::unique_ptr<OperatorBase> clone_impl() const override {
        return std::make_unique<CoreHamiltonian>(*this);
    }
};

// -----------------------------------------------------------------------------
// ----------------------- Inline Implementations ------------------------------
// -----------------------------------------------------------------------------

template<typename OpType, typename... Args, typename>
CoreHamiltonian::CoreHamiltonian(OpType&& op0, Args&&... args) :
  ElectronicHamiltonian(std::forward<OpType>(op0),
                        std::forward<Args>(args)...) {}

} // namespace chemist::operators
