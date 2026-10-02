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
#include <chemist/experimental/basis_set/ao.hpp>
#include <chemist/experimental/basis_set/ao_shell.hpp>
#include <chemist/experimental/basis_set/ao_shell_view.hpp>
#include <chemist/experimental/basis_set/ao_view.hpp>
#include <chemist/experimental/basis_set/atomic_basis_set.hpp>
#include <chemist/experimental/basis_set/cartesian_ao.hpp>
#include <chemist/experimental/basis_set/cca_shell.hpp>
#include <chemist/experimental/basis_set/contracted_gaussian.hpp>
#include <chemist/experimental/basis_set/primitive.hpp>
#include <chemist/experimental/basis_set/spherical_ao.hpp>
#include <chemist/experimental/point/point.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <wtf/wtf.hpp>

namespace chemist::experimental {

void export_point_class(python_module_reference m);
void export_point_view(python_module_reference m);
void export_point_set_class(python_module_reference m);
void export_point_set_view(python_module_reference m);
void export_primitive_class(python_module_reference m);
void export_primitive_view(python_module_reference m);
void export_contracted_gaussian_class(python_module_reference m);
void export_contracted_gaussian_view(python_module_reference m);
void export_ao(python_module_reference m);
void export_ao_shell(python_module_reference m);
void export_cartesian_ao_class(python_module_reference m);
void export_cartesian_ao_view(python_module_reference m);
void export_spherical_ao_class(python_module_reference m);
void export_spherical_ao_view(python_module_reference m);
void export_cca_shell_class(python_module_reference m);
void export_cca_shell_view(python_module_reference m);
void export_atomic_basis_set_class(python_module_reference m);
void export_atomic_basis_set_view(python_module_reference m);

/** @brief Exports the whole experimental component into its own submodule.
 *
 *  Everything here is under `chemist.experimental` so that it can not be
 *  confused with the long-standing `chemist.PointD` and friends, which are
 *  unaffected.
 */
inline void export_experimental(python_module_reference m) {
    auto sub = m.def_submodule(
      "experimental",
      "Classes which are still being designed. Their APIs may change.");
    export_point_class(sub);
    export_point_view(sub);
    export_point_set_class(sub);
    export_point_set_view(sub);
    export_primitive_class(sub);
    export_primitive_view(sub);
    export_contracted_gaussian_class(sub);
    export_contracted_gaussian_view(sub);
    // The abstract bases must be registered before the classes deriving from
    // them, since pybind11 needs the base to exist to record the inheritance.
    export_ao(sub);
    export_ao_shell(sub);
    export_cartesian_ao_class(sub);
    export_cartesian_ao_view(sub);
    export_spherical_ao_class(sub);
    export_spherical_ao_view(sub);
    export_cca_shell_class(sub);
    export_cca_shell_view(sub);
    export_atomic_basis_set_class(sub);
    export_atomic_basis_set_view(sub);
}

namespace detail_ {

/** @brief Unwraps a type-erased coordinate into a Python float.
 *
 *  WTF's own Python bindings (wtf.fp.Float, wtf.fp.FloatView) expose the
 *  type-erased classes as opaque objects, not through a caster, and only
 * for the mutable FloatView --- there is no Python binding for
 *  FloatView<const Float> at all, which is exactly what get_coord/get_x/
 *  get_y/get_z/magnitude return. So a coordinate can not simply be returned
 *  from a chemist binding and left to pybind11; it has to be unwrapped to a
 *  double here, the same way WTF's own bindings unwrap FloatBuffer/
 *  BufferView elements.
 *
 *  @tparam FloatValueType A wtf::fp::Float or a wtf::fp::FloatView,
 *                        (possibly const-qualified).
 *
 *  @param[in] value The type-erased value to unwrap.
 *
 *  @return @p value, as a double.
 *
 *  @throw pybind11::type_error if @p value is not holding a double. Mirrors
 *                              the error wtf's own bindings raise for the
 *                              same situation, rather than letting
 *                              std::runtime_error surface as pybind11's
 * less actionable default RuntimeError.
 */
template<typename FloatViewType>
double to_py_float(const FloatViewType& value) {
    try {
        return value.template value<double>();
    } catch(const std::runtime_error&) {
        throw py::type_error(
          "This coordinate holds a " + value.type_info().name() +
          ", which Python can not represent. Only double is supported at "
          "the Python boundary.");
    }
}

/** @brief Overload of to_py_float for an owning wtf::fp::Float.
 *
 *  inner_product and magnitude return an owning Float rather than a view
 *  (there is nothing for them to alias --- the result is freshly computed).
 *  Binding @p value to this parameter extends the life of the temporary the
 *  caller passed in for the duration of this call, which is what makes
 *  taking its view here safe.
 */
inline double to_py_float(const wtf::fp::Float& value) {
    return to_py_float(value.as_view());
}

/** @brief Adds the read-only half of the point API to @p c.
 *
 *  Factored out because Point, PointView, and the read-only PointView all
 *  expose exactly the same accessors --- the same reason PointCommon exists
 *  on the C++ side.
 */
template<typename PyClass>
void add_point_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("get_coord",
          [](const class_type& p, std::size_t q) {
              return to_py_float(p.get_coord(q));
          })
      .def("get_x", [](const class_type& p) { return to_py_float(p.get_x()); })
      .def("get_y", [](const class_type& p) { return to_py_float(p.get_y()); })
      .def("get_z", [](const class_type& p) { return to_py_float(p.get_z()); })
      .def("magnitude",
           [](const class_type& p) { return to_py_float(p.magnitude()); })
      .def("__repr__", [](const class_type& p) {
          std::stringstream ss;
          ss << p;
          return ss.str();
      });
}

/// Adds the writable half of the point API to @p c
template<typename PyClass>
void add_point_writers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("set_coord",
          [](class_type& p, std::size_t q, double v) { p.set_coord(q, v); })
      .def("set_x", [](class_type& p, double v) { p.set_x(v); })
      .def("set_y", [](class_type& p, double v) { p.set_y(v); })
      .def("set_z", [](class_type& p, double v) { p.set_z(v); });
}

/** @brief Adds the read-only half of the API shared by Primitive and
 *         ContractedGaussian to @p c.
 *
 *  Both model a Gaussian function parameterized by a (possibly shared, in
 *  the contracted-Gaussian case) total angular momentum and center, and
 * both expose evaluate/normalized_evaluate/normalization_constant with
 *  identical signatures. Factored out for the same reason add_point_readers
 *  is: so that Primitive/PrimitiveView, and ContractedGaussian/
 *  ContractedGaussianView, are not each declaring these bindings twice.
 */
template<typename PyClass>
void add_gaussian_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("get_l", [](const class_type& g) { return g.get_l(); })
      .def("get_center", [](const class_type& g) { return g.get_center(); })
      .def("evaluate",
           [](const class_type& g, const Point& r) {
               return to_py_float(g.evaluate(r));
           })
      .def("normalized_evaluate",
           [](const class_type& g, const Point& r) {
               return to_py_float(g.normalized_evaluate(r));
           })
      .def("normalization_constant", [](const class_type& g) {
          return to_py_float(g.normalization_constant());
      });
}

/// Adds the writable half of the API shared by Primitive and
/// ContractedGaussian to @p c. See add_gaussian_readers.
template<typename PyClass>
void add_gaussian_writers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("set_l", [](class_type& g, std::size_t v) { g.set_l(v); })
      .def("set_center",
           [](class_type& g, const Point& r0) { g.set_center(r0); });
}

/** @brief Adds the read-only half of the primitive API to @p c.
 *
 *  Factored out for the same reason as add_point_readers: Primitive,
 *  PrimitiveView, and the read-only PrimitiveView all expose exactly the
 *  same accessors.
 */
template<typename PyClass>
void add_primitive_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("get_coefficient",
          [](const class_type& p) { return to_py_float(p.get_coefficient()); })
      .def("get_exponent",
           [](const class_type& p) { return to_py_float(p.get_exponent()); });
    add_gaussian_readers(c);
}

/// Adds the writable half of the primitive API to @p c
template<typename PyClass>
void add_primitive_writers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("set_coefficient",
          [](class_type& p, double v) { p.set_coefficient(v); })
      .def("set_exponent", [](class_type& p, double v) { p.set_exponent(v); });
    add_gaussian_writers(c);
}

/** @brief Adds the read-only half of the contracted-Gaussian API to @p c.
 *
 *  Factored out for the same reason as add_primitive_readers:
 *  ContractedGaussian, ContractedGaussianView, and the read-only
 *  ContractedGaussianView all expose exactly the same accessors.
 *  utilities::IndexableContainerBase already provides begin()/end()/
 *  operator[]/size()/empty() on the C++ side, so this only needs to bind
 *  those, not implement any new iteration machinery.
 */
template<typename PyClass>
void add_contracted_gaussian_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    add_gaussian_readers(c);
    c.def("__len__", [](const class_type& cg) { return cg.size(); })
      .def("empty", [](const class_type& cg) { return cg.empty(); })
      .def(
        "__iter__",
        [](class_type& cg) { return py::make_iterator(cg.begin(), cg.end()); },
        py::keep_alive<0, 1>())
      .def("__getitem__", [](class_type& cg, std::size_t i) { return cg[i]; });
}

/// Adds the writable half of the contracted-Gaussian API to @p c
template<typename PyClass>
void add_contracted_gaussian_writers(PyClass& c) {
    add_gaussian_writers(c);
}

/** @brief Adds the container API shared by PointSet and its views to @p c.
 *
 *  Note that the coordinate accessors are named `get_x_coordinates` rather
 *  than `get_x_buffer`. On the C++ side that method exists to hand a
 * pointer to contiguous storage to code which can not accept an
 * abstraction; Python has no use for such a pointer, so the binding returns
 * a copy as a list and is named so as not to imply otherwise.
 */
template<typename PyClass>
void add_set_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("__len__", [](const class_type& s) { return s.size(); })
      // Note that this takes a mutable reference: iterating a mutable set
      // must yield mutable views of its points, exactly as the C++
      // range-for does. When class_type is itself read-only, its iterators
      // hand out read-only views anyway.
      .def(
        "__iter__",
        [](class_type& s) { return py::make_iterator(s.begin(), s.end()); },
        py::keep_alive<0, 1>())
      .def("empty", [](const class_type& s) { return s.empty(); })
      .def("get_x_coordinates",
           [](const class_type& s) {
               std::vector<double> rv;
               const auto n = s.size();
               for(std::size_t i = 0; i < n; ++i)
                   rv.push_back(to_py_float(s[i].get_x()));
               return rv;
           })
      .def("get_y_coordinates",
           [](const class_type& s) {
               std::vector<double> rv;
               const auto n = s.size();
               for(std::size_t i = 0; i < n; ++i)
                   rv.push_back(to_py_float(s[i].get_y()));
               return rv;
           })
      .def("get_z_coordinates", [](const class_type& s) {
          std::vector<double> rv;
          const auto n = s.size();
          for(std::size_t i = 0; i < n; ++i)
              rv.push_back(to_py_float(s[i].get_z()));
          return rv;
      });
}

/** @brief Unwraps each element of @p values into a Python float.
 *
 *  The AO classes' evaluate/normalized_evaluate overloads which take a point
 *  set return a std::vector of type-erased values, one per point. This is the
 *  element-wise version of to_py_float, so that those overloads can hand
 *  Python a list of floats.
 *
 *  @tparam ContainerType A container of wtf::fp::Float (or views of them).
 *
 *  @param[in] values The type-erased values to unwrap.
 *
 *  @return @p values, as doubles.
 *
 *  @throw pybind11::type_error if any element of @p values is not holding a
 *                              double. See to_py_float.
 */
template<typename ContainerType>
std::vector<double> to_py_floats(const ContainerType& values) {
    std::vector<double> rv;
    rv.reserve(values.size());
    for(const auto& value : values) rv.push_back(to_py_float(value));
    return rv;
}

/** @brief Adds the API shared by every kind of AO, and every kind of AO view,
 *         to @p c.
 *
 *  This is the Python counterpart of AOCommon: AO, AOView, CartesianAO, its
 *  views, SphericalAO, and its view all expose exactly these methods. The
 *  C++ evaluate/normalized_evaluate accept any kind of point, or any kind of
 *  point set; binding them for the read-only views, which Point/PointView
 *  and PointSet/PointSetView implicitly convert to, gives Python the same
 *  flexibility.
 *
 *  Everything which aliases the AO (the center and the contracted Gaussian)
 *  keeps the AO alive for as long as the returned view is alive.
 */
template<typename PyClass>
void add_ao_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("get_l", [](const class_type& ao) { return ao.get_l(); })
      .def(
        "get_center", [](const class_type& ao) { return ao.get_center(); },
        py::keep_alive<0, 1>())
      // Takes a mutable reference so that, when class_type models mutable
      // state, the returned view is mutable too, exactly as in C++.
      .def(
        "get_contracted_gaussian",
        [](class_type& ao) { return ao.get_contracted_gaussian(); },
        py::keep_alive<0, 1>())
      .def("normalization_constant",
           [](const class_type& ao) {
               return to_py_float(ao.normalization_constant());
           })
      .def("evaluate",
           [](const class_type& ao, const const_point_view& r) {
               return to_py_float(ao.evaluate(r));
           })
      .def("evaluate",
           [](const class_type& ao, const const_point_set_view& points) {
               return to_py_floats(ao.evaluate(points));
           })
      .def("normalized_evaluate",
           [](const class_type& ao, const const_point_view& r) {
               return to_py_float(ao.normalized_evaluate(r));
           })
      .def("normalized_evaluate",
           [](const class_type& ao, const const_point_set_view& points) {
               return to_py_floats(ao.normalized_evaluate(points));
           });
}

/// Adds the read-only half of the Cartesian-AO API to @p c
template<typename PyClass>
void add_cartesian_ao_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    add_ao_readers(c);
    c.def("get_i", [](const class_type& ao) { return ao.get_i(); })
      .def("get_j", [](const class_type& ao) { return ao.get_j(); })
      .def("get_k", [](const class_type& ao) { return ao.get_k(); });
}

/// Adds the writable half of the Cartesian-AO API to @p c
template<typename PyClass>
void add_cartesian_ao_writers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("set_i", [](class_type& ao, std::size_t v) { ao.set_i(v); })
      .def("set_j", [](class_type& ao, std::size_t v) { ao.set_j(v); })
      .def("set_k", [](class_type& ao, std::size_t v) { ao.set_k(v); });
}

/** @brief Adds the read-only half of the spherical-AO API to @p c.
 *
 *  get_cartesian_shell is not bound here because SphericalAO and its view
 *  return different types from it; see their exporters.
 */
template<typename PyClass>
void add_spherical_ao_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    add_ao_readers(c);
    c.def("get_m", [](const class_type& ao) { return ao.get_m(); });
}

/// Adds the writable half of the spherical-AO API to @p c
template<typename PyClass>
void add_spherical_ao_writers(PyClass& c) {
    using class_type          = typename PyClass::type;
    using magnetic_index_type = typename class_type::magnetic_index_type;
    c.def("set_m", [](class_type& ao, magnetic_index_type m) { ao.set_m(m); });
}

/** @brief Adds the API shared by every kind of shell, and every kind of shell
 *         view, to @p c.
 *
 *  This is the Python counterpart of AOShellCommon, minus indexing. The C++
 *  at returns a pointer to a newly built AO view, and the type of that view
 *  differs between the abstract and the concrete shells, so each exporter
 *  binds indexing itself. at throws std::out_of_range, which pybind11
 *  turns into IndexError, so binding __getitem__ also makes every shell
 *  iterable.
 */
template<typename PyClass>
void add_ao_shell_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("get_l", [](const class_type& s) { return s.get_l(); })
      .def("is_pure", [](const class_type& s) { return s.is_pure(); })
      .def("is_cartesian", [](const class_type& s) { return s.is_cartesian(); })
      .def("__len__", [](const class_type& s) { return s.size(); })
      .def(
        "get_center", [](const class_type& s) { return s.get_center(); },
        py::keep_alive<0, 1>())
      // Mutable for the same reason as add_ao_readers's
      .def(
        "get_contracted_gaussian",
        [](class_type& s) { return s.get_contracted_gaussian(); },
        py::keep_alive<0, 1>())
      .def("normalization_constant", [](const class_type& s) {
          return to_py_float(s.normalization_constant());
      });
}

/** @brief Adds the read-only half of the CCAShell API to @p c.
 *
 *  Indexing hands Python ownership of a newly built AO view. The view aliases
 *  the shell's contracted Gaussian, so the shell is kept alive for as long as
 *  the view is.
 *
 *  Only the angular index for the shell's purity is bound (cartesian_powers
 *  for a Cartesian shell, magnetic_index for a pure one), mirroring the C++,
 *  where the other one does not exist.
 */
template<typename PyClass>
void add_cca_shell_readers(PyClass& c) {
    using class_type   = typename PyClass::type;
    using shell_traits = ChemistClassTraits<typename class_type::value_type>;
    add_ao_shell_readers(c);

    auto at = [](const class_type& s, std::size_t i) { return s.at(i); };
    c.def("at", at, py::keep_alive<0, 1>())
      .def("__getitem__", at, py::keep_alive<0, 1>())
      .def(
        "get_cartesian_shell",
        [](const class_type& s) { return s.get_cartesian_shell(); },
        py::keep_alive<0, 1>());

    if constexpr(shell_traits::is_pure) {
        c.def("magnetic_index", [](const class_type& s, std::size_t i) {
            return s.magnetic_index(i);
        });
    } else {
        c.def("cartesian_powers", [](const class_type& s, std::size_t i) {
            const auto [x, y, z] = s.cartesian_powers(i);
            return py::make_tuple(x, y, z);
        });
    }
}

/// Adds the writable half of the CCAShell API to @p c
template<typename PyClass>
void add_cca_shell_writers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("set_l", [](class_type& s, std::size_t l) { s.set_l(l); });
}

/** @brief Adds the read-only half of the AtomicBasisSet API to @p c.
 *
 *  This is the Python counterpart of AtomicBasisSetCommon, minus the
 *  comparisons, which each exporter binds against the types it can be
 *  compared to. Everything which aliases the set (shells, primitives, and
 *  the center) keeps the set alive for as long as the returned object is
 *  alive. at throws std::out_of_range, which pybind11 turns into IndexError,
 *  so binding __getitem__ also makes every set iterable.
 *
 *  As with PointSet, the parameter arrays are returned as lists of floats,
 *  named get_coefficients/get_exponents, rather than as buffers.
 */
template<typename PyClass>
void add_atomic_basis_set_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    using size_type  = std::size_t;

    auto at = [](const class_type& abs, size_type i) { return abs.at(i); };
    c.def("get_name", [](const class_type& abs) { return abs.get_name(); })
      .def("get_atomic_number",
           [](const class_type& abs) { return abs.get_atomic_number(); })
      // Mutable for the same reason as add_ao_readers's
      .def(
        "get_center", [](class_type& abs) { return abs.get_center(); },
        py::keep_alive<0, 1>())
      .def("purity", [](const class_type& abs) { return abs.purity(); })
      .def("ordering", [](const class_type& abs) { return abs.ordering(); })
      .def("is_pure", [](const class_type& abs) { return abs.is_pure(); })
      .def("is_cartesian",
           [](const class_type& abs) { return abs.is_cartesian(); })
      .def("__len__", [](const class_type& abs) { return abs.size(); })
      .def("empty", [](const class_type& abs) { return abs.empty(); })
      .def("at", at, py::keep_alive<0, 1>())
      .def("__getitem__", at, py::keep_alive<0, 1>())
      .def("get_l",
           [](const class_type& abs, size_type i) { return abs.get_l(i); })
      .def("n_aos", [](const class_type& abs) { return abs.n_aos(); })
      .def("n_primitives",
           [](const class_type& abs) { return abs.n_primitives(); })
      .def("primitive_range",
           [](const class_type& abs, size_type i) {
               const auto [first, second] = abs.primitive_range(i);
               return py::make_tuple(first, second);
           })
      .def("primitive_to_shell",
           [](const class_type& abs, size_type i) {
               return abs.primitive_to_shell(i);
           })
      // Mutable, so that a mutable set hands out mutable primitives
      .def(
        "primitive",
        [](class_type& abs, size_type i) { return abs.primitive(i); },
        py::keep_alive<0, 1>())
      .def("get_coefficients",
           [](const class_type& abs) {
               const auto buffer = abs.get_coefficient_buffer();
               std::vector<double> rv;
               for(size_type i = 0; i < buffer.size(); ++i)
                   rv.push_back(to_py_float(buffer.at(i)));
               return rv;
           })
      .def("get_exponents", [](const class_type& abs) {
          const auto buffer = abs.get_exponent_buffer();
          std::vector<double> rv;
          for(size_type i = 0; i < buffer.size(); ++i)
              rv.push_back(to_py_float(buffer.at(i)));
          return rv;
      });
}

/// Adds the writable half of the AtomicBasisSet API to @p c
template<typename PyClass>
void add_atomic_basis_set_writers(PyClass& c) {
    using class_type = typename PyClass::type;
    using size_type  = std::size_t;
    c.def("set_name", [](class_type& abs,
                         std::string name) { abs.set_name(std::move(name)); })
      .def("set_atomic_number",
           [](class_type& abs, size_type z) { abs.set_atomic_number(z); })
      // Point and PointView both convert to the read-only view
      .def("set_center", [](class_type& abs,
                            const const_point_view& r0) { abs.set_center(r0); })
      .def("set_l",
           [](class_type& abs, size_type i, size_type l) { abs.set_l(i, l); });
}

/** @brief Adds __eq__/__ne__ against @p OtherType to @p c.
 *
 *  The C++ comparison operators are templated so that, e.g., a view compares
 *  against the object it views. pybind11 can not bind a template, so each
 *  pairing has to be spelled out; this is the same pattern the Primitive and
 *  ContractedGaussian view exporters write out by hand. @p c must already
 *  have py::self == py::self bound, so that a comparison against an
 *  unsupported type returns NotImplemented rather than raising.
 */
template<typename OtherType, typename PyClass>
void add_comparisons_with(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("__eq__", [](const class_type& lhs,
                       const OtherType& rhs) { return lhs == rhs; })
      .def("__ne__", [](const class_type& lhs, const OtherType& rhs) {
          return lhs != rhs;
      });
}

} // namespace detail_
} // namespace chemist::experimental
