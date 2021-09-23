#include "libchemist/wavefunction/nonrelativistic.hpp"

namespace libchemist::wavefunction {

template class Nonrelativistic<Determinant>;
template class Nonrelativistic<CanonicalDeterminant>;

} // namespace libchemist::wavefunction
