#pragma once

namespace chemist::wavefunction {

template<typename OccupiedSpaceType, typename VirtualSpaceType>
class DeterminantSpace;

template<typename InactiveSpaceType, typename ActiveSpaceType, 
         typename VirtualSpaceType, typename CoreSpaceType>
class ActiveSpace;

template<typename ReferenceType>
class Nonrelativistic;

} // namespace chemist::wavefunction
