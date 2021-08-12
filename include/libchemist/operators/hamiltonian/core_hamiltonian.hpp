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
    explicit CoreHamiltonian(const ElectronicHamiltonian&);

    template<typename OpType, typename... Args,
             typename = disable_if_hamiltonian_t<std::decay_t<OpType>>>
    explicit CoreHamiltonian(OpType&& op0, Args&&... args);
};

// -----------------------------------------------------------------------------
// ----------------------- Inline Implementations ------------------------------
// -----------------------------------------------------------------------------

template<typename OpType, typename... Args, typename>
CoreHamiltonian::CoreHamiltonian(OpType&& op0, Args&&... args) :
  ElectronicHamiltonian(std::forward<OpType>(op0),
                        std::forward<Args>(args)...) {}

} // namespace libchemist::operators
