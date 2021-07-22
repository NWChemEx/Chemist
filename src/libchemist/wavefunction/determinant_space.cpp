#include "libchemist/wavefunction/determinant_space.hpp"

using namespace libchemist::orbital_space;

namespace libchemist::wavefunction {

template class DeterminantSpace<DerivedSpaceD>;
template class DeterminantSpace<IndDerivedSpace, DepDerivedSpace>;

template class DeterminantSpace<CanonicalSpaceD>;
template class DeterminantSpace<CanonicalIndSpace, CanonicalDepSpace>;

} // namespace libchemist::wavefunction
