#pragma once

#include <map>
#include <vector>
#include <set>
#include <memory>

namespace LibChemist {

  ///These are the recognized types of IntCoords
  namespace IntCoord_t {
    constexpr auto BOND="BOND";///<A bond
    constexpr auto PAIR13="PAIR13";///<A 1,3 pair
    constexpr auto PAIR14="PAIR14";///<A 1,4 pair
    constexpr auto PAIR="PAIR";///<A pair that is not a 1,2; 1,3; or 1,4 pair
    constexpr auto ANGLE="ANGLE";///<An angle
    constexpr auto TORSION="TORSION";///<A torsion
    constexpr auto IMPTORSION="IMPTORSION";///<An improper torsion angle
  }

  ///Type of a quantity we are treating as a mathematical vector
  using Vector=std::vector<double>;

}//end namespace
