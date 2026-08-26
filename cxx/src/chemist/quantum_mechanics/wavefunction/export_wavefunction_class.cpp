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

#include "export_wavefunction.hpp"
#include <chemist/quantum_mechanics/wavefunction/wavefunction_class.hpp>

namespace chemist::wavefunction {

class PyWavefunction : public Wavefunction {
public:
    using Wavefunction::Wavefunction;

protected:
    base_pointer clone_() const override {
        // unique_ptr isn't exposed so can't use PYBIND11_OVERRIDE_PURE
        // This is okay as long as Python classes don't call this function
        return base_pointer{};
    }

    bool are_equal_(const_base_reference other) const noexcept override {
        PYBIND11_OVERRIDE_PURE(bool, Wavefunction, are_equal_, other);
    }
};

void export_wavefunction_class(python_module_reference m) {
    python_class_type<Wavefunction, PyWavefunction>(m, "Wavefunction")
      .def(py::init<>())
      .def("clone", &Wavefunction::clone)
      .def("are_equal", &Wavefunction::are_equal)
      .def("are_different", &Wavefunction::are_different);
}

} // namespace chemist::wavefunction
