#include "LibChemist/BasisShell.hpp"
#include "LibChemist/Utilities.hpp"
#include <tuple>

namespace LibChemist {

bool BasisShell::operator==(const BasisShell& rhs) const noexcept
{
    return std::tie(type, l, ngen, nprim, cs_, alphas_)
           == std::tie(
                  rhs.type, rhs.l, rhs.ngen, rhs.nprim, rhs.cs_, rhs.alphas_);
}

size_t BasisShell::nfunctions(size_t i) const noexcept
{
    const size_t temp_l = am_2int(l, i);
    if(type != ShellType::CartesianGaussian)
        return 2 * temp_l + 1;
    return multinomial_coefficient(3ul, temp_l);
}

} // End namespace LibChemist
