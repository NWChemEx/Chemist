#pragma once
#include <array>
#include <numeric>
#include <cmath>

///Namespace for all code associated with LibChemist
namespace LibChemist {

  ///Returns the cross product of two vectors
  template<typename T>
  std::array<double,3> cross(const T& v1, const T& v2){
    return {v1[1]*v2[2]-v1[2]*v2[1],
	v1[2]*v2[0]-v1[0]*v2[2],
	v1[0]*v2[1]-v1[1]*v2[0]};
  }
  ///Returns the difference between two vectors, v1-v2
  template<typename T>
  std::array<double,3> diff(const T& v1,const T& v2){
    return {v1[0]-v2[0],v1[1]-v2[1],v1[2]-v2[2]};
  }

  ///Returns the sum of two vectors, v1+v2
  template<typename T>
  std::array<double,3> sum(const T& v1,const T& v2){
    return {v1[0]+v2[0],v1[1]+v2[1],v1[2]+v2[2]};
  }

  ///Returns the dot product of two vectors
  template<typename T>
  inline double dot(const T& v1,
		    const T& v2){
    return std::inner_product(v1.begin(),v1.end(),v2.begin(),0.0);
  }

  ///Returns the magnitude of a vector
  template<typename T>
  inline double mag(const T& v1){
    return std::sqrt(dot(v1,v1));
  }

  ///Returns the angle between two vectors given the vectors \p v1, \p v2)
  template<typename T>
  inline double angle(const T& v1,
		      const T& v2){
    const std::array<double,3> v1crossv2=cross(v1,v2);
    const double v1dotv2=dot(v1,v2);
    const double cosphi=v1dotv2,sinphi=mag(v1crossv2);
    return std::atan2(sinphi,cosphi);   
  }


} //End namespace LibChemist
