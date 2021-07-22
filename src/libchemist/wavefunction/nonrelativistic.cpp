#include "libchemist/wavefunction/nonrelativistic.hpp"

namespace libchemist::wavefunction {

template class Nonrelativistic<Determinant>;
template class Nonrelativistic<CanonicalDeterminant>;
template class Nonrelativistic<LocalDeterminant>;
template class Nonrelativistic<CanonicalLocalDeterminant>;

} // namespace libchemist::wavefunction
