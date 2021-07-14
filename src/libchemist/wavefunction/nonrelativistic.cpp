#include "libchemist/wavefunction/nonrelativistic.hpp"

namespace libchemist::wavefunction {

template class Nonrelativistic<DeterminantD>;
template class Nonrelativistic<CanonicalDeterminantD>;
template class Nonrelativistic<LocalDeterminantD>;
template class Nonrelativistic<CanonicalLocalDeterminantD>;

}
