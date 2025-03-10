/*
 * Copyright 2024 NWChemEx Community
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

namespace chemist {

void export_chemical_system_class(python_module_reference m);
void export_chemical_system_view(python_module_reference m);

inline void export_chemical_system(python_module_reference m) {
    export_chemical_system_class(m);
    export_chemical_system_view(m);
}

} // namespace chemist
