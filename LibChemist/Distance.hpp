#pragma once
#include "LibChemistDefs.hpp"
#include "Molecule.hpp"

namespace LibChemist {

  class Distance {
    const std::string name;
  public:
    /**
       @brief computes the zeroth, first, and second derivative of the distance between two atoms 
       @throws
     */
    Distance(const std::string& namein):
      name(namein){}
    std::vector<double> deriv(size_t deriv_order,const Molecule& molecule,const std::vector<int>& coord_i)const;
  };

  class Bond:public Distance{
  public:
    Bond():Distance(IntCoord_t::BOND){}
  };

  class Pair:public Distance{
  public:
    Pair():Distance(IntCoord_t::PAIR){}
  };

  class Pair13: public Distance{
  public:
    Pair13():Distance(IntCoord_t::PAIR13){}
    std::vector<double> deriv(size_t deriv_order,const Molecule& molecule,const std::vector<int>& coord_i)const;
  };

  class Pair14: public Distance{
  public:
    Pair14():Distance(IntCoord_t::PAIR14){}
  };
    
} // namespace LibChemist
