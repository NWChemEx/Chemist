#pragma once
#include "libchemist/operators/hamiltonian/electronic_hamiltonian_nuclear.hpp"

namespace libchemist::operators {

/** @brief Models the one-electron core (electronic) Hamiltonian derivative
 *
 *  The core (electronic) Hamiltonian derivative contains terms which are
 * one-electron terms or sums of one-electron terms. In practice we rarely need
 * the many- electron core Hamiltonian derivative so by convention this class is
 * the one-electron core Hamiltonian derivative.
 */
class CoreHamiltonian_Nuclear : public ElectronicHamiltonian_Nuclear {
private:
    template<typename T>
    static constexpr bool is_hamiltonian_v =
      std::is_base_of_v<Hamiltonian_Nuclear, T>;

    template<typename T>
    using disable_if_hamiltonian_t = std::enable_if_t<!is_hamiltonian_v<T>>;

public:
    CoreHamiltonian_Nuclear()                               = default;
    CoreHamiltonian_Nuclear(const CoreHamiltonian_Nuclear&) = default;
    CoreHamiltonian_Nuclear(CoreHamiltonian_Nuclear&&)      = default;
    CoreHamiltonian_Nuclear& operator=(const CoreHamiltonian_Nuclear&) =
      default;
    CoreHamiltonian_Nuclear& operator=(CoreHamiltonian_Nuclear&&) = default;

    /** @brief Convenience ctor for creating a CoreHamiltonian derivative from
     * the ElectronicHamiltonian derivative.
     *
     *  This function will initialize and add an ElectronKinetic derivative
     * operator if an NElectronKinetic derivative operator is present in @p He,
     * then loop over the NElectronNuclearAttraction derivative operators in @p
     * He and initialize and add corresponding ElectronNuclearAttraction
     * derivative operators for each.
     *
     *  @param[in] He The ElectronicHamiltonian derivative we are converting.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        new instance. Strong throw guarantee.
     */
    explicit CoreHamiltonian_Nuclear(const ElectronicHamiltonian_Nuclear& He);

    template<typename OpType, typename... Args,
             typename = disable_if_hamiltonian_t<std::decay_t<OpType>>>
    explicit CoreHamiltonian_Nuclear(OpType&& op0, Args&&... args);

protected:
    std::unique_ptr<OperatorBase> clone_impl() const override {
        return std::make_unique<CoreHamiltonian_Nuclear>(*this);
    }
};

// -----------------------------------------------------------------------------
// ----------------------- Inline Implementations ------------------------------
// -----------------------------------------------------------------------------

template<typename OpType, typename... Args, typename>
CoreHamiltonian_Nuclear::CoreHamiltonian_Nuclear(OpType&& op0, Args&&... args) :
  ElectronicHamiltonian_Nuclear(std::forward<OpType>(op0),
                                std::forward<Args>(args)...) {}

} // namespace libchemist::operators
