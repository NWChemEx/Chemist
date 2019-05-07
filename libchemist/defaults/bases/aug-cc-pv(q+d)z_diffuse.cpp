/*
 * This file is autogenerated by generate_basis.py.  Any modifications made in
 * this file will be lost next time generate_basis.py is run.
 */

#include "libchemist/defaults/nwx_basis_list.hpp"

namespace libchemist::detail_ {

AOBasisSet aug_dash_cc_dash_pv(q_plus_d) z_diffuse(std::size_t Z) {
    switch(Z) {
        case(13): {
            return AOBasisSet{
              AOShell{
                0,
                AOPrimitive{0.0183000, 1.0000000},
              },
              AOShell{
                1,
                AOPrimitive{0.0121000, 1.0000000},
              },
              AOShell{
                2,
                AOPrimitive{0.0274000, 1.0000000},
              },
              AOShell{
                3,
                AOPrimitive{0.0582000, 1.0000000},
              },
              AOShell{
                4,
                AOPrimitive{0.1530000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(14): {
            return AOBasisSet{
              AOShell{
                0,
                AOPrimitive{0.0275000, 1.0000000},
              },
              AOShell{
                1,
                AOPrimitive{0.0200000, 1.0000000},
              },
              AOShell{
                2,
                AOPrimitive{0.0414000, 1.0000000},
              },
              AOShell{
                3,
                AOPrimitive{0.0846000, 1.0000000},
              },
              AOShell{
                4,
                AOPrimitive{0.2120000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(15): {
            return AOBasisSet{
              AOShell{
                0,
                AOPrimitive{0.0354000, 1.0000000},
              },
              AOShell{
                1,
                AOPrimitive{0.0272000, 1.0000000},
              },
              AOShell{
                2,
                AOPrimitive{0.0570000, 1.0000000},
              },
              AOShell{
                3,
                AOPrimitive{0.1090000, 1.0000000},
              },
              AOShell{
                4,
                AOPrimitive{0.2500000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(16): {
            return AOBasisSet{
              AOShell{
                0,
                AOPrimitive{0.0428000, 1.0000000},
              },
              AOShell{
                1,
                AOPrimitive{0.0317000, 1.0000000},
              },
              AOShell{
                2,
                AOPrimitive{0.0722000, 1.0000000},
              },
              AOShell{
                3,
                AOPrimitive{0.1400000, 1.0000000},
              },
              AOShell{
                4,
                AOPrimitive{0.2970000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(17): {
            return AOBasisSet{
              AOShell{
                0,
                AOPrimitive{0.0519000, 1.0000000},
              },
              AOShell{
                1,
                AOPrimitive{0.0376000, 1.0000000},
              },
              AOShell{
                2,
                AOPrimitive{0.0912000, 1.0000000},
              },
              AOShell{
                3,
                AOPrimitive{0.2170000, 1.0000000},
              },
              AOShell{
                4,
                AOPrimitive{0.3780000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        case(18): {
            return AOBasisSet{
              AOShell{
                0,
                AOPrimitive{0.0610000, 1.0000000},
              },
              AOShell{
                1,
                AOPrimitive{0.0435000, 1.0000000},
              },
              AOShell{
                2,
                AOPrimitive{0.1080000, 1.0000000},
              },
              AOShell{
                3,
                AOPrimitive{0.2940000, 1.0000000},
              },
              AOShell{
                4,
                AOPrimitive{0.4590000, 1.0000000},
              },
            }; // End AOBasis
        }      // End case
        default: { throw std::out_of_range("Basis not available for Z"); }
    } // end switch

} // end function
} // namespace libchemist::detail_