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

#pragma once
#include "chemist/orbital_space.hpp"

namespace chemist {

// This class wraps OrbitalSpace for physically-motivated, type-enforcing
// reasons
template<typename element_type = double,
         typename tensor_type  = type::tensor<element_type>>
class OrthogonalSpace : public OrbitalSpace<element_type, tensor_type> {
public:
    using OrbitalSpace<element_type, tensor_type>::OrbitalSpace;
};

extern template class OrthogonalSpace<float, type::tensor<float>>;
extern template class OrthogonalSpace<float, type::tensor<double>>;
extern template class OrthogonalSpace<double, type::tensor<float>>;
extern template class OrthogonalSpace<double, type::tensor<double>>;

} // namespace chemist