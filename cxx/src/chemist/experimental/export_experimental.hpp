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
#include <chemist/experimental/basis_set/contracted_gaussian.hpp>
#include <chemist/experimental/basis_set/primitive.hpp>
#include <chemist/experimental/point/point.hpp>
#include <sstream>
#include <stdexcept>
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
}

namespace detail_ {

/** @brief Unwraps a type-erased coordinate into a Python float.
 *
 *  WTF's own Python bindings (wtf.fp.Float, wtf.fp.FloatView) expose the
 *  type-erased classes as opaque objects, not through a caster, and only for
 *  the mutable FloatView --- there is no Python binding for
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
 *                              std::runtime_error surface as pybind11's less
 *                              actionable default RuntimeError.
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
 *  the contracted-Gaussian case) total angular momentum and center, and both
 *  expose evaluate/normalized_evaluate/normalization_constant with
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
 *  than `get_x_buffer`. On the C++ side that method exists to hand a pointer
 *  to contiguous storage to code which can not accept an abstraction; Python
 *  has no use for such a pointer, so the binding returns a copy as a list and
 *  is named so as not to imply otherwise.
 */
template<typename PyClass>
void add_set_readers(PyClass& c) {
    using class_type = typename PyClass::type;
    c.def("__len__", [](const class_type& s) { return s.size(); })
      // Note that this takes a mutable reference: iterating a mutable set
      // must yield mutable views of its points, exactly as the C++ range-for
      // does. When class_type is itself read-only, its iterators hand out
      // read-only views anyway.
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

} // namespace detail_
} // namespace chemist::experimental
