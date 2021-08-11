#pragma once
#include "libchemist/operators/hamiltonian/electronic_hamiltonian.hpp"

namespace libchemist::operators {

/** @brief Models the one-electron core (electronic) Hamiltonian
 *
 *  The core (electronic) Hamiltonian contains terms which are one-electron
 *  terms or sums of one-electron terms. In practice we rarely need the many-
 *  electron core Hamiltonian so by convention this class is the one-electron
 *  core Hamiltonian.
 */
class CoreHamiltonian : public ElectronicHamiltonian {
public:
    CoreHamiltonian()                       = default;
    CoreHamiltonian(const CoreHamiltonian&) = default;
    CoreHamiltonian(CoreHamiltonian&&)      = default;
    CoreHamiltonian& operator=(const CoreHamiltonian&) = default;
    CoreHamiltonian& operator=(CoreHamiltonian&&) = default;
    explicit CoreHamiltonian(const ElectronicHamiltonian&);
};

} // namespace libchemist::operators
