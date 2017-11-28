#include "LibChemist/BasisSet.hpp"
#include "LibChemist/Utilities.hpp"
#include <algorithm>
#include <tuple>

namespace LibChemist {

bool BasisSet::operator==(const BasisSet& rhs)const noexcept
{
    return std::tie(centers,ngens,nprims,coefs,alphas,types,ls)==
           std::tie(rhs.centers,rhs.ngens,rhs.nprims,rhs.coefs,rhs.alphas,
                    rhs.types,rhs.ls);
}


void BasisSet::add_shell(const double* center,
                         const BasisShell& shell)
{
    const size_t nprim=shell.nprim;
    const size_t ngen=shell.ngen;

    for(size_t i=0;i<3;++i)
        centers.push_back(center[i]);

    for(size_t i=0;i<nprim;++i)
        alphas.push_back(shell.alpha(i));

    for(size_t i=0;i<ngen;++i)
    {
        for(size_t j=0;j<nprim;++j)
            coefs.push_back(shell.coef(j,i));
    }

    ls.push_back(shell.l);
    nprims.push_back(nprim);
    ngens.push_back(ngen);
    types.push_back(shell.type);
}

size_t BasisSet::max_am()const noexcept
{
    //Need min in case it's negative
    int min=*std::min_element(ls.begin(),ls.end());
    int max=*std::max_element(ls.begin(),ls.end());
    //If min is negative, -1 times its value is the max angular momentum it holds
    return (min<0?std::max(-1*min,max):max);
}

size_t BasisSet::size()const
{
    size_t total=0;
    for(size_t i=0;i<ngens.size();++i)
        total+=this->shellsize(i);
    return total;
}

size_t BasisSet::shellsize(const size_t i)const
{
    size_t total=0;
        for(size_t j=0;j<ngens[i];++j)
        {
            const bool is_cart=types[i]==ShellType::CartesianGaussian;
            const size_t l=am_2int(ls[i],j);
            total+=(is_cart?multinomial_coefficient(3ul,l):2*l+1);
        }
    return total;
}

namespace detail_{
    template<typename T>
    void vector_cat(std::vector<T>& lhs, const std::vector<T>& rhs)
    {
        lhs.insert(lhs.end(),rhs.begin(),rhs.end());
    }

}

BasisSet ungeneralize_basis_set(const BasisSet &bs)
{
    BasisSet rv;
    size_t offset=0;
    const size_t nshells=bs.ngens.size();
    for(size_t shell=0; shell<nshells; ++shell)
    {
        //coef runs from 0 to ngen*nprims
        for(size_t cont=0,coef=0;cont<bs.ngens[shell];++cont)
        {
            rv.ngens.push_back(1);
            //For each general contraction need to duplicate the center,
            for(size_t i=0;i<3;i++)
                rv.centers.push_back(bs.centers[3*shell+i]);
            //...the number of primitives,
            rv.nprims.push_back(bs.nprims[shell]);
            //...the type
            rv.types.push_back(bs.types[shell]);
            //...the angular momentum
            rv.ls.push_back(am_2int(bs.ls[shell],cont));
            for(size_t prim=0;prim<bs.nprims[shell];++prim,++coef)
            {
                //...and the exponents
                rv.alphas.push_back(bs.alphas[offset+prim]);

                rv.coefs.push_back(bs.coefs[offset+coef]);
            }
        }
        offset+=bs.nprims[shell];
    }
    return rv;
}

BasisSet& basis_set_concatenate(BasisSet& lhs, const BasisSet& rhs)
{
    using detail_::vector_cat;
    vector_cat(lhs.centers,rhs.centers);
    vector_cat(lhs.coefs,rhs.coefs);
    vector_cat(lhs.alphas,rhs.alphas);
    vector_cat(lhs.nprims,rhs.nprims);
    vector_cat(lhs.ngens,rhs.ngens);
    vector_cat(lhs.types,rhs.types);
    vector_cat(lhs.ls,rhs.ls);
    return lhs;
}

}//End namespace
