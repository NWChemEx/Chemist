#include <chemist/quantum_mechanics/wavefunction/wavefunction.hpp>
#include <chemist/quantum_mechanics/wavefunction/wavefunction_fwd.hpp>

namespace chemist::wavefunction {

/** @brief Type-erased base class for all determinants. */
class DeterminantBase : public Wavefunction {
public:
};

template<typename OneParticleBasis>
class Determinant : public DeterminantBase {
public:
    using one_particle_basis_type            = VectorSpace;
    using one_particle_basis_reference       = one_particle_basis_type&;
    using one_particle_basis_const_reference = const one_particle_basis_type&;
};

} // namespace chemist::wavefunction