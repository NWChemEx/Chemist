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

namespace chemist::braket {
namespace detail_ {
namespace {

/** @brief Returns the name of @p o's Python type, for error messages and
 *         __repr__.
 *
 *  A leading underscore is stripped. The template instantiations Python
 *  dispatches to are exported under underscore-prefixed names (e.g., an
 *  operator built by `Kinetic(Electron())` is really a `_KineticElectron`),
 *  and those names are an implementation detail users should not have to read
 *  in a repr.
 */
std::string type_name(const py::object& o) {
    auto name = py::str(py::type::of(o).attr("__name__")).cast<std::string>();
    if(!name.empty() && name.front() == '_') name.erase(0, 1);
    return name;
}

/** @brief Can @p o be the bra or the ket of a BraKet?
 *
 *  Bras and kets are vector spaces (e.g., AOs) or wavefunctions (e.g., a
 *  Determinant). The multi-center integrals additionally use a pair of vector
 *  spaces, which comes in from Python as a two-element tuple.
 */
bool is_bra_or_ket(const py::object& o) {
    if(can_cast<const wavefunction::VectorSpace&>(o)) return true;
    if(can_cast<const wavefunction::Wavefunction&>(o)) return true;

    if(py::isinstance<py::tuple>(o)) {
        auto as_tuple = o.cast<py::tuple>();
        if(as_tuple.size() != 2) return false;
        for(const auto& x : as_tuple)
            if(!is_bra_or_ket(py::reinterpret_borrow<py::object>(x)))
                return false;
        return true;
    }
    return false;
}

/// Throws if @p o can't be the bra or the ket. @p which names it for the error.
void assert_bra_or_ket(const py::object& o, const char* which) {
    if(is_bra_or_ket(o)) return;
    std::string msg("The ");
    msg += which;
    msg += " of a BraKet must be a VectorSpace, a Wavefunction, or a pair of "
           "them, not a ";
    msg += type_name(o);
    msg += ".";
    throw std::runtime_error(msg);
}

} // namespace

PyBraKet::PyBraKet(python_object_type bra, python_object_type op,
                   python_object_type ket) :
  m_bra_(std::move(bra)), m_op_(std::move(op)), m_ket_(std::move(ket)) {
    // Check eagerly. Without this the failure surfaces much later, as a cast
    // error from deep inside a module's run, where it is far harder to read.
    assert_bra_or_ket(m_bra_, "bra");
    assert_bra_or_ket(m_ket_, "ket");

    if(!can_cast<const qm_operator::OperatorBase&>(m_op_)) {
        std::string msg("The operator of a BraKet must derive from "
                        "OperatorBase, not a ");
        msg += type_name(m_op_);
        msg += ".";
        throw std::runtime_error(msg);
    }
}

bool PyBraKet::operator==(const PyBraKet& rhs) const {
    return m_bra_.equal(rhs.m_bra_) && m_op_.equal(rhs.m_op_) &&
           m_ket_.equal(rhs.m_ket_);
}

std::string PyBraKet::as_string() const {
    return "BraKet(" + type_name(m_bra_) + "|" + type_name(m_op_) + "|" +
           type_name(m_ket_) + ")";
}

} // namespace detail_

void export_py_braket(python_module_reference m) {
    using py_braket_type = detail_::PyBraKet;
    using object_type    = typename py_braket_type::python_object_type;

    python_class_type<py_braket_type, TensorRepresentation, py::smart_holder>(
      m, "BraKet")
      .def(py::init<object_type, object_type, object_type>(), py::arg("bra"),
           py::arg("op"), py::arg("ket"))
      .def_property_readonly("bra", &py_braket_type::bra)
      .def_property_readonly("op", &py_braket_type::op)
      .def_property_readonly("ket", &py_braket_type::ket)
      .def("__repr__", &py_braket_type::as_string)
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace chemist::braket
