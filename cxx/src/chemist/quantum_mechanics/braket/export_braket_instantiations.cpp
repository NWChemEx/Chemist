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

#include "export_braket.hpp"
#include "py_braket.hpp"
#include <chemist/quantum_mechanics/operator/typedefs.hpp>

namespace chemist::braket {

namespace detail_ {

/** @brief Exports one BraKet<BraType, OperatorType, KetType> instantiation.
 *
 *  The exported class is an implementation detail: its name is underscore-
 *  prefixed and Python users are never expected to name it. It exists so that
 *  pybind11 has a registered target type to convert the erased BraKet class
 *  into, which is what PluginPlay's PythonWrapper does when a module unwraps a
 *  Python-provided BraKet. See PyBraKet for the full story.
 *
 *  @param[in] name The name to give the class in Python.
 *  @param[in] m The module to add the class to.
 */
template<typename BraType, typename OperatorType, typename KetType>
void export_braket_(const char* name, python_module_reference m) {
    using braket_type = BraKet<BraType, OperatorType, KetType>;

    auto from_erased = [](const PyBraKet& erased) {
        // N.B. The operator comes back by reference because OperatorType may
        // be the abstract OperatorBase; BraKet's ctor clones it.
        return std::make_unique<braket_type>(erased.bra_as<BraType>(),
                                             erased.op_as<OperatorType>(),
                                             erased.ket_as<KetType>());
    };

    auto get_bra = [](const braket_type& self) { return self.bra(); };
    auto get_op  = [](const braket_type& self) -> const OperatorType& {
        return self.op();
    };
    auto get_ket = [](const braket_type& self) { return self.ket(); };

    python_class_type<braket_type, TensorRepresentation, py::smart_holder>(m,
                                                                           name)
      .def(py::init(from_erased))
      .def_property_readonly("bra", get_bra)
      .def_property_readonly("op", get_op)
      .def_property_readonly("ket", get_ket)
      .def(py::self == py::self)
      .def(py::self != py::self);

    // This is the line that makes a single Python BraKet work. With it,
    // `py_object.cast<braket_type>()` on an erased BraKet calls the ctor
    // registered just above, so PluginPlay resolves the instantiation the
    // property type wants without Python ever naming it.
    py::implicitly_convertible<PyBraKet, braket_type>();
}

} // namespace detail_

void export_braket_instantiations(python_module_reference m) {
    using namespace chemist::qm_operator;
    using namespace chemist::qm_operator::types;
    using wavefunction::AOs;
    using wavefunction::CMOs;
    using wavefunction::MOs;

    using aos2     = std::pair<AOs, AOs>;
    using det_mos  = wavefunction::Determinant<MOs>;
    using det_cmos = wavefunction::Determinant<CMOs>;

    // N.B. This list mirrors the EBK(...) table in SimDE's
    // cxx/include/simde/evaluate_braket/evaluate_braket.hpp; the names are the
    // SimDE property type names so the two stay easy to diff. Adding an
    // EvaluateBraKet instantiation there means adding it here as well,
    // otherwise the property type can not be reached from Python.
    //
    // Two of SimDE's entries, aos_rho_e_aos and ESCF, are templated on the
    // orbital space. Both MOs and CMOs are done here, matching the orbital
    // spaces OperatorVisitor dispatches Density on, even though SimDE
    // currently only exports the CMOs property types.
    detail_::export_braket_<AOs, op_base_type, AOs>("_BraKet_aos_op_base_aos",
                                                    m);
    detail_::export_braket_<AOs, s_e_type, AOs>("_BraKet_aos_s_e_aos", m);
    detail_::export_braket_<AOs, t_e_type, AOs>("_BraKet_aos_t_e_aos", m);
    detail_::export_braket_<AOs, v_en_type, AOs>("_BraKet_aos_v_en_aos", m);
    detail_::export_braket_<AOs, CoreHamiltonian, AOs>("_BraKet_aos_h_e_aos",
                                                       m);
    detail_::export_braket_<AOs, Fock, AOs>("_BraKet_aos_f_e_aos", m);
    detail_::export_braket_<AOs, j_e_type, AOs>("_BraKet_aos_j_e_aos", m);
    detail_::export_braket_<AOs, k_e_type, AOs>("_BraKet_aos_k_e_aos", m);
    detail_::export_braket_<AOs, xc_e_type, AOs>("_BraKet_aos_xc_e_aos", m);

    // N.B. Qualified because chemist::Density (the field, in
    // chemist/density/density.hpp) is also in scope here.
    using rho_mos  = qm_operator::Density<MOs, Electron>;
    using rho_cmos = qm_operator::Density<CMOs, Electron>;
    detail_::export_braket_<AOs, rho_mos, AOs>("_BraKet_aos_rho_e_mos_aos", m);
    detail_::export_braket_<AOs, rho_cmos, AOs>("_BraKet_aos_rho_e_cmos_aos",
                                                m);

    detail_::export_braket_<AOs, v_ee_type, AOs>("_BraKet_ERI2", m);
    detail_::export_braket_<AOs, v_ee_type, aos2>("_BraKet_ERI3", m);
    detail_::export_braket_<aos2, v_ee_type, aos2>("_BraKet_ERI4", m);

    detail_::export_braket_<det_mos, Hamiltonian, det_mos>("_BraKet_ESCF_MOs",
                                                           m);
    detail_::export_braket_<det_cmos, Hamiltonian, det_cmos>(
      "_BraKet_ESCF_CMOs", m);
}

} // namespace chemist::braket
