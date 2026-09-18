/*
 * Copyright 2026 NWChemEx-Project
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

#include "../export_experimental.hpp"
#include <vector>

namespace chemist::experimental {

void export_contracted_gaussian_class(python_module_reference m) {
    auto c = python_class_type<ContractedGaussian>(m, "ContractedGaussian")
               .def(py::init<>())
               // The templated range ctor is not directly bindable; this
               // overload narrows it to double, the same way Primitive's
               // binding narrows its templated ctor.
               .def(py::init([](std::vector<double> cs, std::vector<double> es,
                                std::size_t l, double x, double y, double z) {
                   return ContractedGaussian(cs.begin(), cs.end(), es.begin(),
                                             es.end(), l, x, y, z);
               }))
               .def(py::self == py::self)
               .def(py::self != py::self);

    detail_::add_contracted_gaussian_readers(c);
    detail_::add_contracted_gaussian_writers(c);
}

} // namespace chemist::experimental
