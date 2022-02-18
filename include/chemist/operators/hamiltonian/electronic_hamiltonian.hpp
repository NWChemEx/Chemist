#pragma once
#include "chemist/operators/hamiltonian/hamiltonian.hpp"

namespace chemist::operators {

/** @brief Hamiltonian focused exclusively on interactions involving electrons.
 *
 *  The ElectronicHamiltonian is at the moment a strong type meant to signify
 *  that the Hamiltonian shouldn't contain any terms which do not involve
 *  electrons. It's primarily used as an input to correlated methods.
 */
class ElectronicHamiltonian : public Hamiltonian {
private:
    template<typename T>
    static constexpr bool is_hamiltonian_v = std::is_base_of_v<Hamiltonian, T>;

    template<typename T>
    using disable_if_hamiltonian_t = std::enable_if_t<!is_hamiltonian_v<T>>;

public:
    ElectronicHamiltonian()                             = default;
    ElectronicHamiltonian(const ElectronicHamiltonian&) = default;
    ElectronicHamiltonian(ElectronicHamiltonian&&)      = default;
    ElectronicHamiltonian& operator=(const ElectronicHamiltonian&) = default;
    ElectronicHamiltonian& operator=(ElectronicHamiltonian&&) = default;

    /** @brief Convenience ctor for creating an ElectronicHamiltonian from the
     *         full system Hamiltonian.
     *
     *  This function will loop over the terms in @p H and pull out those which
     *  depend on one or more electrons. The resulting ElectronicHamiltonian
     *  instance will be initialized with copies of those terms.
     *
     *  @param[in] H The Hamiltonian we are partitioning.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        new instance. Strong throw guarantee.
     */
    explicit ElectronicHamiltonian(const Hamiltonian& H);

    template<typename OpType, typename... Args,
             typename = disable_if_hamiltonian_t<std::decay_t<OpType>>>
    explicit ElectronicHamiltonian(OpType&& op0, Args&&... args);

protected:
    std::unique_ptr<OperatorBase> clone_impl() const override {
        return std::make_unique<ElectronicHamiltonian>(*this);
    }
};

// -----------------------------------------------------------------------------
// ----------------------- Inline Implementations ------------------------------
// -----------------------------------------------------------------------------

template<typename OpType, typename... Args, typename>
ElectronicHamiltonian::ElectronicHamiltonian(OpType&& op0, Args&&... args) :
  Hamiltonian(std::forward<OpType>(op0), std::forward<Args>(args)...) {}

} // namespace chemist::operators
