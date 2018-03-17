#include "Distance.hpp"
#include "Molecule.hpp"
#include "Util.hpp"

namespace LibChemist {

inline Vector dist(const std::array<double,3> q1, const std::array<double,3> q2){
  return {mag(diff(q1,q2))};
}

inline Vector d1_dist(const std::array<double,3> q1,const std::array<double,3> q2)
{
  const double d=1.0/dist(q1,q2)[0];
  const auto dq=diff(q1,q2);
  return {dq[0]*d,dq[1]*d,dq[2]*d,-dq[0]*d,-dq[1]*d,-dq[2]*d};
}

inline Vector d2_dist(const std::array<double,3> q1,const std::array<double,3> q2)
{
  const double d=1/dist(q1,q2)[0];
  const auto dq=diff(q1,q2);
  const Vector dqdr=d1_dist(q1,q2);
  Vector deriv(36,0.0);
  for(size_t i=0;i<6;++i)
    for(size_t j=0;j<6;++j)
      {
	deriv[i*6+j]=-d*dqdr[i]*dqdr[j];
	if(i==j)deriv[i*6+i]+=d;
	else if(i+3==j||j+3==i)deriv[i*6+j]-=d;
      }
  return deriv;
}

//Vector Distance::deriv(size_t deriv_order,const Vector& sys,const IVector& coord_i)const{
Vector Distance::deriv(size_t deriv_order,const Molecule& molecule,const std::vector<int>& coord_i)const{
  if( deriv_order>=3 )throw std::runtime_error( "Higher order derivatives are not yet implemented!!!" );
  const size_t atomi=coord_i[0],atomj=coord_i[1];
//  const double* q1=&(sys[atomi*3]), *q2=&(sys[atomj*3]);
  const std::array<double,3> q1=molecule.atoms[atomi].coords, q2=molecule.atoms[atomj].coords;
  if(deriv_order==0) return dist(q1,q2);
  if(deriv_order==1) return d1_dist(q1,q2);
  if(deriv_order==2) return d2_dist(q1,q2);
}

//Vector Pair13::deriv(size_t deriv_order,const Vector& sys,const IVector& coord_i)const{
Vector Pair13::deriv(size_t deriv_order,const Molecule& molecule,const std::vector<int>& coord_i)const{
  const Vector d=Distance::deriv(deriv_order,molecule,{coord_i[0],coord_i[2]});
  if( deriv_order>=3 )throw std::runtime_error( "Derivatives higher than 2 are NYI" );
  if(deriv_order==0)return d;
  else if(deriv_order==1)
    {
      Vector dp(9);
      for(size_t i=0;i<6;++i)
	dp[i<3?i:3+i]=d[i];
      return dp;
    }
  else if(deriv_order==2)
    {
      Vector dp(81);
      for(size_t i=0;i<6;++i)
        {
	  const size_t i_=i<3?i:i+3;
	  for(size_t j=0;j<6;++j)
            {
	      const size_t j_=j<3?j:j+3;
	      dp[i_*9+j_]=dp[i*6+j];
            }
        }
      return dp;
    }

}
    
} // namespace LibChemist

