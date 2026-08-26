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

#include "py_operator_dispatch.hpp"

namespace chemist::qm_operator::detail_ {

std::string python_type_name(const py::object& o) {
    return py::str(py::type::of(o).attr("__name__")).cast<std::string>();
}

void assert_no_kwargs(const char* name, const py::kwargs& kwargs) {
    if(kwargs.size() == 0) return;
    std::string msg(name);
    msg += " does not take keyword arguments.";
    throw py::type_error(msg);
}

py::object export_dispatching_class(python_module_reference m, const char* name,
                                    py::tuple impls, py::object dispatch,
                                    const char* doc) {
    auto abc_meta = py::module_::import("abc").attr("ABCMeta");

    py::dict ns;

    // __new__ is what CPython calls to make the object, and returning an
    // instance of a different class from it is how the dispatch happens.
    //
    // N.B. This works because of an invariant that is easy to break: the
    //      instantiations are NOT subclasses of the class being created here.
    //      type.__call__ only runs __init__ when the object __new__ returned
    //      passes PyObject_TypeCheck, which is a real, MRO-based subtype test
    //      and does not consult isinstance() or the registrations below. Make
    //      an instantiation an actual subclass and pybind11's __init__ starts
    //      running a second time on an already-constructed object.
    ns["__new__"] = py::staticmethod(dispatch);

    ns["_impls"]     = impls;
    ns["__module__"] = m.attr("__name__");
    if(doc != nullptr) ns["__doc__"] = py::str(doc);

    py::object cls = abc_meta(py::str(name), py::make_tuple(), ns);

    // Registering the instantiations as virtual subclasses is what makes
    // isinstance(Kinetic(), Kinetic) true. Registration only affects
    // isinstance/issubclass; it leaves the MRO, and therefore the invariant
    // above, alone.
    for(auto impl : impls) cls.attr("register")(impl);

    m.attr(name) = cls;
    return cls;
}

} // namespace chemist::qm_operator::detail_
