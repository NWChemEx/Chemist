
#pragma once
#include "libchemist/operators/hamiltonian/hamiltonian_nuclear.hpp"

namespace libchemist::operators {

/** @brief Hamiltonian derivatives focused exclusively on interactions involving
 * electrons.
 *
 *  The ElectronicHamiltonian derivatives is at the moment a strong type meant
 * to signify that the Hamiltonian derivatives shouldn't contain any terms which
 * do not involve electrons. It's primarily used as an input to correlated
 * methods.
 */
/*
class ElectronicHamiltonian_Nuclear : public Hamiltonian_Nuclear {
private:
    template<typename T>
    static constexpr bool is_hamiltonian_v =
      std::is_base_of_v<Hamiltonian_Nuclear, T>;

    template<typename T>
    using disable_if_hamiltonian_t = std::enable_if_t<!is_hamiltonian_v<T>>;

public:
    ElectronicHamiltonian_Nuclear() = default;
    ElectronicHamiltonian_Nuclear(const ElectronicHamiltonian_Nuclear&) =
      default;
    ElectronicHamiltonian_Nuclear(ElectronicHamiltonian_Nuclear&&) = default;
    ElectronicHamiltonian_Nuclear& operator                        =(
      const ElectronicHamiltonian_Nuclear&) = default;
    ElectronicHamiltonian_Nuclear& operator=(ElectronicHamiltonian_Nuclear&&) =
      default;

    /** @brief Convenience ctor for creating an ElectronicHamiltonian derivative
     * from the full system Hamiltonian derivative.
     *
     *  This function will loop over the terms in @p H and pull out those which
     *  depend on one or more electrons. The resulting ElectronicHamiltonian
     * derivative instance will be initialized with copies of those terms.
     *
     *  @param[in] H The Hamiltonian derivative we are partitioning.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        new instance. Strong throw guarantee.
     * /
    explicit ElectronicHamiltonian_Nuclear(const Hamiltonian_Nuclear& H);

    template<typename OpType, typename... Args,
             typename = disable_if_hamiltonian_t<std::decay_t<OpType>>>
    explicit ElectronicHamiltonian_Nuclear(OpType&& op0, Args&&... args);

protected:
    std::unique_ptr<OperatorBase> clone_impl() const override {
        return std::make_unique<ElectronicHamiltonian_Nuclear>(*this);
    }

};
*/

// -----------------------------------------------------------------------------
// ----------------------- Inline Implementations ------------------------------
// -----------------------------------------------------------------------------
/*
template<typename OpType, typename... Args, typename>
ElectronicHamiltonian_Nuclear::ElectronicHamiltonian_Nuclear(OpType&& op0,
                                                             Args&&... args) :
  Hamiltonian_Nuclear(std::forward<OpType>(op0), std::forward<Args>(args)...) {}
*/
} // namespace libchemist::operators

