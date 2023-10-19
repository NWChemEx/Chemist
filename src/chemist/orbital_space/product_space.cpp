/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "chemist/orbital_space/product_space.hpp"
#include "detail_/make_indices.hpp"
#include <tensorwrapper/tensor/creation.hpp>

namespace chemist::orbital_space {

#define PRODUCT_SPACE ProductSpace<ProductSpaceA, ProductSpaceB>

// -----------------------------------------------------------------------------
// ---------------------------- CTORS ------------------------------------------
// -----------------------------------------------------------------------------

template<typename ProductSpaceA, typename ProductSpaceB>
PRODUCT_SPACE::ProductSpace(ProductSpaceA SpaceA, ProductSpaceB SpaceB) :
  ProductSpace(std::make_shared<space_type_a>(std::move(SpaceA)),
               std::make_shared<space_type_b>(std::move(SpaceB))) {}

template<typename ProductSpaceA, typename ProductSpaceB>
PRODUCT_SPACE::ProductSpace(std::shared_ptr<const ProductSpaceA> pSpaceA, 
                            std::shared_ptr<const ProductSpaceB> pSpaceB) :
  m_pSpaceA_(pSpaceA), m_pSpaceB_(pSpaceB) {}

// -----------------------------------------------------------------------------
// --------------------------------- Accessors ---------------------------------
// -----------------------------------------------------------------------------

template<typename ProductSpaceA, typename ProductSpaceB>
typename PRODUCT_SPACE::const_space_reference_a PRODUCT_SPACE::SpaceRefA() const {
    if(m_pSpaceA_) return *m_pSpaceA_;
    throw std::runtime_error("No first space available!");
}

template<typename ProductSpaceA, typename ProductSpaceB>
typename PRODUCT_SPACE::const_space_reference_b PRODUCT_SPACE::SpaceRefB() const {
    if(m_pSpaceB_) return *m_pSpaceB_;
    throw std::runtime_error("No second space available!");
}

// -----------------------------------------------------------------------------
// ------------------------- Operators -----------------------------------------
// -----------------------------------------------------------------------------

template<typename ProductSpaceA, typename ProductSpaceB>
bool PRODUCT_SPACE::operator==(const ProductSpace& rhs) const noexcept {
    auto plhs_a = space_data_a();
    auto plhs_b = space_data_b();
    auto prhs_a = rhs.space_data_a();
    auto prhs_b = rhs.space_data_b();
    if(plhs_a && plhs_b && prhs_a && prhs_b) { // Both have from_spaces
        if((SpaceRefA() != rhs.SpaceRefA()) || 
           (SpaceRefB() != rhs.SpaceRefB())) return false;
    } else if(plhs_a && !prhs_a)
        return false; // LHS has from, RHS doesn't
    else if(plhs_b && !prhs_b)
        return false; // LHS has from, RHS doesn't
    else if(!plhs_a && prhs_a)
        return false; // RHS has from, LHS doesn't
    else if(!plhs_b && prhs_b)
        return false; // RHS has from, LHS doesn't
    // else is both don't have from-space

    if(size() != rhs.size()) return false;
    // We know they either both have orbitals or both don't
    // else case here covers they both don't
    //return size() ? C() == rhs.C() : true;
}

// Get the space array representing the direct product of two spaces
template<typename ProductSpaceA, typename ProductSpaceB>
ProductSpaceA* PRODUCT_SPACE::getSpaceVec(SpaceVec& PSpace) {
    PSpace.reserve(SpaceRefA().size() * SpaceRefB().size());
    for (int i = 0;i < SpaceRefB().size();i++) {
        PSpace[i] = ProductSpaceA(SpaceRefA());
    }
    return PSpace;
}

// -----------------------------------------------------------------------------
// ------------------------- Virtual Fxn Overrides -----------------------------
// -----------------------------------------------------------------------------

template<typename ProductSpaceA, typename ProductSpaceB>
typename PRODUCT_SPACE::size_type PRODUCT_SPACE::size_() const noexcept {
    if(!m_pSpaceA_ || !m_pSpaceB_) return 0;
    return SpaceRefA().size() * SpaceRefB().size();
}

template<typename ProductSpaceA, typename ProductSpaceB>
bool PRODUCT_SPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

#undef PRODUCT_SPACE

// -----------------------------------------------------------------------------
// ----------------------------- Template Instantiations -----------------------
// -----------------------------------------------------------------------------

template class ProductSpace<AOSpaceD, SpinSpace>;

}
