/*
 * Copyright 2023 NWChemEx-Project
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
#include "../pychemist.hpp"
#include <chemist/point_charge/point_charge.hpp>

namespace chemist {

void export_charge_view(python_module_reference m);
void export_charges(python_module_reference m);
void export_charges_view(python_module_reference m);
void export_point_charge_class(python_module_reference m);

inline void export_point_charge(python_module_reference m) {
    export_point_charge_class(m);
    export_charge_view(m);
    export_charges(m);
    export_charges_view(m);
}

} // namespace chemist
