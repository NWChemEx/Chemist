#pragma once
#include "libchemist/operators/hamiltonian/hamiltonian.hpp"

namespace libchemist::operators {

/** @brief Hamiltonian focused exclusively on interactions involving electrons.
 *
 *  The ElectronicHamiltonian is at the moment a strong type meant to signify
 *  that the Hamiltonian shouldn't contain any terms which do not involve
 *  electrons. It's primarily used as inputs to correlated methods.
 */
class ElectronicHamiltonian : public Hamiltonian {
public:
    ElectronicHamiltonian()                             = default;
    ElectronicHamiltonian(const ElectronicHamiltonian&) = default;
    ElectronicHamiltonian(ElectronicHamiltonian&&)      = default;
    ElectronicHamiltonian& operator=(const ElectronicHamiltonian&) = default;
    ElectronicHamiltonian& operator=(ElectronicHamiltonian&&) = default;
    explicit ElectronicHamiltonian(const Hamiltonian&);
};

} // namespace libchemist::operators
