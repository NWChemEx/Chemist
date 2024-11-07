#pragma once

/** @file wavefunction_fwd.hpp
 *
 *  This file contains forward declarations of the classes comprising the
 *  wavefunction sub-component of Chemist.
 */

namespace chemist::wavefunction {

class AOs;
class MOs;
template<typename FromSpaceType>
class Transformed;
class VectorSpace;
class Wavefunction;

} // namespace chemist::wavefunction