#pragma once

#include<chemist/nucleus/detail_/nuclei_view_pimpl.hpp>

namespace chemist::detail_ {

/** @brief Implements a NucleiView returned from a FragmentedNuclei.
 * 
 *  The FragmentedNuclei object ultimately uses indices to determine if a
 *  nucleus is in a fragment or not. When users access the fragment they get
 *  back a NucleiView. 
*/
class FromFragmentedNuclei : public NucleiViewPIMPL {

};

}