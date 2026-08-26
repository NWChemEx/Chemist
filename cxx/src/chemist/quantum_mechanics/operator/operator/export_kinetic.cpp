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

#include "export_operator.hpp"
#include "py_operator_dispatch.hpp"
#include <chemist/quantum_mechanics/operator/kinetic.hpp>

namespace chemist::qm_operator {

void export_kinetic(python_module_reference m) {
    using namespace detail_;
    using table = one_particle_types;

    auto impls = export_instantiations<Kinetic, table>(m, "Kinetic");

    // N.B. The leading parameter is the class CPython passes to __new__. It is
    //      always Kinetic itself, so it is ignored.
    auto dispatch = [](py::object, py::args args,
                       py::kwargs kwargs) -> py::object {
        assert_no_kwargs("Kinetic", kwargs);

        // No particle gives the same object as Kinetic<Electron>'s default
        // ctor.
        if(args.size() == 0) return py::cast(Kinetic<Electron>{});

        if(args.size() != 1)
            throw py::type_error("Kinetic takes at most one particle.");

        auto particle = py::reinterpret_borrow<py::object>(args[0]);

        auto make = [&](auto tag) {
            using T = typename decltype(tag)::type;
            return py::cast(Kinetic<T>(particle.cast<T>()));
        };

        auto result = select_type<table>(particle, make);
        if(!result.is_none()) return result;

        throw py::type_error(
          "Kinetic can not describe the kinetic energy of a " +
          python_type_name(particle) +
          ". Supported particles are: " + supported_particles<table>() + ".");
    };

    export_dispatching_class(
      m, "Kinetic", impls, py::cpp_function(dispatch),
      "Describes the kinetic energy of a particle.\n\n"
      "Kinetic(particle) selects the instantiation the particle implies. "
      "Kinetic() is the same as Kinetic(Electron()).");
}

} // namespace chemist::qm_operator
