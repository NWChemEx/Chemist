/*
 * This file is autogenerated by generate_basis.py.  Any modifications made in
 * this file will be lost next time generate_basis.py is run.
 */

#include "libchemist/defaults/nwx_basis_list.hpp"

namespace libchemist::detail_ {

AOBasisSet three_dash_21g_star_polarization(std::size_t Z) {
    switch(Z) {
        case(11): {
            return AOBasisSet{
              AOShell{
                2,
                AOPrimitive{0.1750000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(12): {
            return AOBasisSet{
              AOShell{
                2,
                AOPrimitive{0.1750000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(13): {
            return AOBasisSet{
              AOShell{
                2,
                AOPrimitive{0.3250000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(14): {
            return AOBasisSet{
              AOShell{
                2,
                AOPrimitive{0.4500000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(15): {
            return AOBasisSet{
              AOShell{
                2,
                AOPrimitive{0.5500000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(16): {
            return AOBasisSet{
              AOShell{
                2,
                AOPrimitive{0.6500000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(17): {
            return AOBasisSet{
              AOShell{
                2,
                AOPrimitive{0.7500000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(18): {
            return AOBasisSet{
              AOShell{
                2,
                AOPrimitive{0.8500000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        default: { throw std::out_of_range("Basis not available for Z"); }
    } // end switch

} // end function
} // namespace libchemist::detail_
