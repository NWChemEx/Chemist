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

#pragma once
#include "chemist/pychemist.hpp"
#include <chemist/quantum_mechanics/braket/braket.hpp>
#include <chemist/quantum_mechanics/operator/operator.hpp>
#include <chemist/quantum_mechanics/wavefunction/wavefunction.hpp>

namespace chemist::braket::detail_ {

/** @brief The single, erased BraKet class seen by Python.
 *
 *  `chemist::braket::BraKet` is templated on the types of the bra, the
 *  operator, and the ket. Exporting one Python class per instantiation would
 *  force Python users to know which C++ template instantiation their operator
 *  implies, and the list would grow with every new operator. Instead Python
 *  sees a single `BraKet` class, which is what this class implements.
 *
 *  *this holds the three pieces as Python objects and defers the conversion to
 *  C++ until something asks for a specific `BraKet<Bra, Op, Ket>`. Each
 *  instantiation exported by export_braket_instantiations registers a ctor
 *  taking *this, plus a pybind11 implicit conversion, so
 *  `py_object.cast<BraKet<Bra, Op, Ket>>()` resolves the right instantiation
 *  at run time. That is exactly the call PluginPlay's `PythonWrapper::unwrap`
 *  makes when a module unwraps a Python-provided input, which is what lets one
 *  Python BraKet satisfy any of SimDE's EvaluateBraKet property types.
 *
 *  N.B. *this stores the Python objects it was given, it does not copy them.
 *  Converting to a `BraKet<Bra, Op, Ket>` does copy (and clones the operator),
 *  so the C++ side always gets the value semantics it expects.
 */
class PyBraKet : public TensorRepresentation {
private:
    /// Type of *this
    using my_type = PyBraKet;

    /// Type of the base class
    using base_class_type = TensorRepresentation;

public:
    /// Type of the objects *this holds
    using python_object_type = py::object;

    /// Type of a read-only reference to a python_object_type object
    using const_python_object_reference = const python_object_type&;

    /** @brief Creates a BraKet from the provided bra, operator, and ket.
     *
     *  @param[in] bra The vector space or wavefunction for the bra.
     *  @param[in] op The operator acting on the ket.
     *  @param[in] ket The vector space or wavefunction for the ket.
     *
     *  @throw std::runtime_error if any of the arguments is not of a type a
     *                            BraKet can be formed from. Strong throw
     *                            guarantee.
     */
    PyBraKet(python_object_type bra, python_object_type op,
             python_object_type ket);

    /// Read-only access to the object *this was given for the bra
    const_python_object_reference bra() const noexcept { return m_bra_; }

    /// Read-only access to the object *this was given for the operator
    const_python_object_reference op() const noexcept { return m_op_; }

    /// Read-only access to the object *this was given for the ket
    const_python_object_reference ket() const noexcept { return m_ket_; }

    /** @brief Returns the bra as an object of type @p T.
     *
     *  Bras and kets are returned by value because BraKet's ctor takes them by
     *  value, and because they are not always class types (the three- and
     *  four-center ERIs use `std::pair<AOs, AOs>`, which has no registered
     *  Python class and is converted by pybind11's stl caster).
     *
     *  @tparam T The type to convert the bra to.
     *
     *  @throw std::runtime_error if the bra can not be converted to an object
     *                            of type @p T. Strong throw guarantee.
     */
    template<typename T>
    T bra_as() const {
        return m_bra_.cast<T>();
    }

    /// Same as bra_as, except that it acts on the ket. See bra_as for details.
    template<typename T>
    T ket_as() const {
        return m_ket_.cast<T>();
    }

    /** @brief Returns the operator as a read-only reference to a @p T object.
     *
     *  Unlike the bra and the ket, the operator is returned by reference. This
     *  is required, not stylistic: BraKet's operator type parameter may be
     *  `OperatorBase`, which is abstract and thus can never be cast to by
     *  value. This is the widening that makes SimDE's `aos_op_base_aos` (the
     *  entry point every AO integral driver dispatches from) reachable from
     *  Python. BraKet's ctor clones the operator, so returning a reference
     *  into the Python object *this holds is safe.
     *
     *  @tparam T The type to convert the operator to.
     *
     *  @throw std::runtime_error if the operator can not be converted to a
     *                            @p T object. Strong throw guarantee.
     */
    template<typename T>
    const T& op_as() const {
        return m_op_.cast<const T&>();
    }

    /** @brief Is *this value equal to @p rhs?
     *
     *  Two PyBraKet objects are value equal if the objects they hold for the
     *  bra, the operator, and the ket each compare equal in Python.
     *
     *  @param[in] rhs The PyBraKet to compare to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     */
    bool operator==(const PyBraKet& rhs) const;

    /// Negates operator==. See operator== for the definition of value equal.
    bool operator!=(const PyBraKet& rhs) const { return !(*this == rhs); }

    /// A string representation of *this, e.g. "BraKet(AOs|KineticElectron|AOs)"
    std::string as_string() const;

protected:
    /// Implements clone by calling the copy ctor
    base_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }

    /// Implements are_equal by downcasting @p rhs and comparing via operator==
    bool are_equal_(const_base_reference rhs) const noexcept override {
        auto prhs = dynamic_cast<const my_type*>(&rhs);
        if(prhs == nullptr) return false; // Different types
        return (*this) == *prhs;
    }

private:
    /// The object *this was given for the bra
    python_object_type m_bra_;

    /// The object *this was given for the operator
    python_object_type m_op_;

    /// The object *this was given for the ket
    python_object_type m_ket_;
};

} // namespace chemist::braket::detail_
