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
#include <chemist/density/decomposable_density.hpp>
#include <chemist/density/density.hpp>
#include <chemist/electron/electron.hpp>
#include <chemist/nucleus/nuclei.hpp>
#include <chemist/quantum_mechanics/operator/operator_base.hpp>
#include <string>
#include <tuple>
#include <utility>

namespace chemist::qm_operator::detail_ {

// -----------------------------------------------------------------------------
// Naming
// -----------------------------------------------------------------------------

/** @brief How a particle type is named in Python.
 *
 *  @p suffix is appended to the operator's name to build the name of an
 *  instantiation, e.g. Coulomb plus Electron plus DensityElectron gives
 *  `_CoulombElectronDensityElectron`. @p py_name is the name of the particle's
 *  own Python class, which is what error messages should say; the two differ
 *  for the density types because their Python classes are not templated.
 */
template<typename ParticleType>
struct particle_traits;

#define CHEMIST_PARTICLE_TRAITS(cxx_type, name_suffix, python_name) \
    template<>                                                      \
    struct particle_traits<cxx_type> {                              \
        static constexpr const char* suffix  = name_suffix;         \
        static constexpr const char* py_name = python_name;         \
    }

CHEMIST_PARTICLE_TRAITS(Electron, "Electron", "Electron");
CHEMIST_PARTICLE_TRAITS(ManyElectrons, "ManyElectrons", "ManyElectrons");
CHEMIST_PARTICLE_TRAITS(Nucleus, "Nucleus", "Nucleus");
CHEMIST_PARTICLE_TRAITS(Nuclei, "Nuclei", "Nuclei");
CHEMIST_PARTICLE_TRAITS(chemist::Density<Electron>, "DensityElectron",
                        "Density");
CHEMIST_PARTICLE_TRAITS(chemist::DecomposableDensity<Electron>,
                        "DecomposableDensityElectron", "DecomposableDensity");

#undef CHEMIST_PARTICLE_TRAITS

// -----------------------------------------------------------------------------
// The instantiation tables
// -----------------------------------------------------------------------------

/// Compile-time tag naming the particle a one-particle operator acts on.
template<typename T>
struct type_tag {
    using type = T;
};

/// Compile-time tag naming the two particles a two-particle operator relates.
template<typename T, typename U>
struct type_pair {
    using lhs_type = T;
    using rhs_type = U;
};

/** @brief The instantiations an operator is exported for.
 *
 *  N.B. These mirror, one for one, the macros OperatorVisitor is defined with
 *  in include/chemist/quantum_mechanics/operator/operator_visitor.hpp, and the
 *  explicit instantiations in the operators' .cpp files. Those three lists have
 *  to agree: an instantiation missing from the visitor can not be dispatched,
 *  and one missing from the .cpp does not link. Change them together.
 */
///@{

/// Mirrors ONE_PARTICLE_OVERLOADS.
using one_particle_types =
  std::tuple<type_tag<Electron>, type_tag<ManyElectrons>, type_tag<Nucleus>,
             type_tag<Nuclei>>;

/// Mirrors TWO_PARTICLE_OVERLOADS.
using two_particle_pairs =
  std::tuple<type_pair<Electron, Electron>,
             type_pair<ManyElectrons, ManyElectrons>,
             type_pair<Electron, Nuclei>, type_pair<ManyElectrons, Nuclei>,
             type_pair<Nuclei, Nuclei>>;

/// Mirrors DENSITY_OVERLOADS.
using density_pairs =
  std::tuple<type_pair<Electron, chemist::Density<Electron>>,
             type_pair<ManyElectrons, chemist::Density<Electron>>,
             type_pair<Electron, chemist::DecomposableDensity<Electron>>,
             type_pair<ManyElectrons, chemist::DecomposableDensity<Electron>>>;

///@}

/// Joins instantiation tables, for operators covered by more than one macro.
template<typename... Tables>
using join_tables = decltype(std::tuple_cat(std::declval<Tables>()...));

// -----------------------------------------------------------------------------
// Matching a Python object to an instantiation
// -----------------------------------------------------------------------------

/// Is @p o exactly a @p T, as opposed to something derived from a @p T?
template<typename T>
bool is_exactly(const py::object& o) {
    return py::type::of(o).is(py::type::of<T>());
}

/** @brief Does @p o select the instantiation for @p T?
 *
 *  @param[in] exact Require @p o's type to be exactly @p T. See select_pair
 *                   for why the search is run once with this set and once
 *                   without.
 */
template<typename T>
bool particle_matches(const py::object& o, bool exact) {
    return exact ? is_exactly<T>(o) : py::isinstance<T>(o);
}

/// Implements select_pair for one value of @p exact.
template<typename Table, typename Make, std::size_t... I>
py::object select_pair_(const py::object& lhs, const py::object& rhs,
                        bool exact, Make&& make, std::index_sequence<I...>) {
    py::object result = py::none();
    auto try_one      = [&](auto tag) {
        using T = typename decltype(tag)::lhs_type;
        using U = typename decltype(tag)::rhs_type;
        if(!result.is_none()) return;
        if(particle_matches<T>(lhs, exact) && particle_matches<U>(rhs, exact))
            result = make(tag);
    };
    (try_one(std::tuple_element_t<I, Table>{}), ...);
    return result;
}

/** @brief Builds the instantiation in @p Table that @p lhs and @p rhs select.
 *
 *  @tparam Table The instantiation table to search.
 *  @tparam Make Callable taking a type_pair tag and returning the operator.
 *
 *  @return The operator @p make built, or None if no instantiation matches.
 *
 *  N.B. The table is searched twice: once requiring an exact type match, then
 *  once allowing derived types. The exact pass is what makes a
 *  DecomposableDensity, which derives from Density, select its own
 *  instantiation instead of whichever of the two happens to be listed first.
 *  The second pass then still accepts a type derived from a particle.
 */
template<typename Table, typename Make>
py::object select_pair(const py::object& lhs, const py::object& rhs,
                       Make&& make) {
    constexpr auto n = std::tuple_size_v<Table>;
    auto indices     = std::make_index_sequence<n>{};

    auto result = select_pair_<Table>(lhs, rhs, true, make, indices);
    if(result.is_none())
        result = select_pair_<Table>(lhs, rhs, false, make, indices);
    return result;
}

/// Same as select_pair, except for a one-particle operator. See select_pair.
template<typename Table, typename Make, std::size_t... I>
py::object select_type_(const py::object& particle, bool exact, Make&& make,
                        std::index_sequence<I...>) {
    py::object result = py::none();
    auto try_one      = [&](auto tag) {
        using T = typename decltype(tag)::type;
        if(!result.is_none()) return;
        if(particle_matches<T>(particle, exact)) result = make(tag);
    };
    (try_one(std::tuple_element_t<I, Table>{}), ...);
    return result;
}

/// Same as select_pair, except for a one-particle operator. See select_pair.
template<typename Table, typename Make>
py::object select_type(const py::object& particle, Make&& make) {
    constexpr auto n = std::tuple_size_v<Table>;
    auto indices     = std::make_index_sequence<n>{};

    auto result = select_type_<Table>(particle, true, make, indices);
    if(result.is_none())
        result = select_type_<Table>(particle, false, make, indices);
    return result;
}

// -----------------------------------------------------------------------------
// Exporting the instantiations
// -----------------------------------------------------------------------------

/// Exports Op<T> under the name @p op_name and @p T imply. Triggers for one-
/// particle operators.
template<template<typename...> class Op, typename Customize, typename T>
py::object export_instantiation_(python_module_reference m, const char* op_name,
                                 Customize& customize, type_tag<T> tag) {
    using op_type = Op<T>;

    std::string name("_");
    name += op_name;
    name += particle_traits<T>::suffix;

    auto get_particle = [](const op_type& o) { return o.get_particle(); };
    auto set_particle = [](op_type& o, T& p) { o.set_particle(p); };

    python_class_type<op_type, OperatorBase, py::smart_holder> cls(
      m, name.c_str());
    cls.def(py::init<>())
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_property("particle", get_particle, set_particle);

    customize(cls, tag);
    return cls;
}

/// Exports Op<T, U> under the name @p op_name, @p T, and @p U imply. Triggers
/// for two-particle operators.
template<template<typename...> class Op, typename Customize, typename T,
         typename U>
py::object export_instantiation_(python_module_reference m, const char* op_name,
                                 Customize& customize, type_pair<T, U> tag) {
    using op_type = Op<T, U>;

    std::string name("_");
    name += op_name;
    name += particle_traits<T>::suffix;
    name += particle_traits<U>::suffix;

    auto get_lhs = [](const op_type& o) { return o.get_lhs_particle(); };
    auto set_lhs = [](op_type& o, T& p) { o.set_lhs_particle(p); };
    auto get_rhs = [](const op_type& o) { return o.get_rhs_particle(); };
    auto set_rhs = [](op_type& o, U& p) { o.set_rhs_particle(p); };

    python_class_type<op_type, OperatorBase, py::smart_holder> cls(
      m, name.c_str());
    cls.def(py::init<>())
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_property("lhs_particle", get_lhs, set_lhs)
      .def_property("rhs_particle", get_rhs, set_rhs);

    customize(cls, tag);
    return cls;
}

/** @brief The customization every operator but ExchangeCorrelation needs.
 *
 *  All that is left over after export_instantiations has done the common part
 *  is the ctor taking the particles by value.
 */
struct default_customize {
    template<typename Class, typename T>
    void operator()(Class& cls, type_tag<T>) const {
        cls.def(py::init<T>());
    }

    template<typename Class, typename T, typename U>
    void operator()(Class& cls, type_pair<T, U>) const {
        cls.def(py::init<T, U>());
    }
};

/// Implements export_instantiations.
template<template<typename...> class Op, typename Table, typename Customize,
         std::size_t... I>
py::tuple export_instantiations_(python_module_reference m, const char* op_name,
                                 Customize& customize,
                                 std::index_sequence<I...>) {
    return py::make_tuple(export_instantiation_<Op>(
      m, op_name, customize, std::tuple_element_t<I, Table>{})...);
}

/** @brief Exports every instantiation in @p Table.
 *
 *  Every operator's instantiations are exported the same way: a default ctor,
 *  comparisons, and a property per particle. This does that part; whatever is
 *  left over, namely the value ctor and any properties the operator adds, is
 *  the job of @p customize.
 *
 *  The exported classes are implementation details: their names are
 *  underscore-prefixed and Python users are never expected to name them. They
 *  use the single class export_dispatching_class adds, which dispatches to
 *  one of these.
 *
 *  @tparam Op The operator class template, e.g. Coulomb.
 *  @tparam Table The instantiation table to export, e.g. density_pairs.
 *
 *  @param[in,out] m The module to add the classes to.
 *  @param[in] op_name The operator's name, e.g. "Coulomb". The instantiations
 *                     are named by appending each particle's suffix to it.
 *  @param[in] customize Called with each exported class and its tag. Defaults
 *                       to adding the ctor taking the particles by value,
 *                       which is all any operator but ExchangeCorrelation
 *                       needs.
 *
 *  @return The exported classes, in the order @p Table lists them, ready to be
 *          handed to export_dispatching_class.
 */
template<template<typename...> class Op, typename Table,
         typename Customize = default_customize>
py::tuple export_instantiations(python_module_reference m, const char* op_name,
                                Customize customize = Customize{}) {
    constexpr auto n = std::tuple_size_v<Table>;
    return export_instantiations_<Op, Table>(m, op_name, customize,
                                             std::make_index_sequence<n>{});
}

// -----------------------------------------------------------------------------
// Error reporting
// -----------------------------------------------------------------------------

/// Implements supported_combinations for a table of type_pair tags.
template<typename Table, std::size_t... I>
std::string supported_pairs_(std::index_sequence<I...>) {
    std::string out;
    auto add = [&](auto tag) {
        using T = typename decltype(tag)::lhs_type;
        using U = typename decltype(tag)::rhs_type;
        if(!out.empty()) out += ", ";
        out += "(";
        out += particle_traits<T>::py_name;
        out += ", ";
        out += particle_traits<U>::py_name;
        out += ")";
    };
    (add(std::tuple_element_t<I, Table>{}), ...);
    return out;
}

/// Lists the combinations in @p Table, for an error message.
template<typename Table>
std::string supported_combinations() {
    constexpr auto n = std::tuple_size_v<Table>;
    return supported_pairs_<Table>(std::make_index_sequence<n>{});
}

/// Implements supported_particles for a table of type_tag tags.
template<typename Table, std::size_t... I>
std::string supported_types_(std::index_sequence<I...>) {
    std::string out;
    auto add = [&](auto tag) {
        using T = typename decltype(tag)::type;
        if(!out.empty()) out += ", ";
        out += particle_traits<T>::py_name;
    };
    (add(std::tuple_element_t<I, Table>{}), ...);
    return out;
}

/// Lists the particles in @p Table, for an error message.
template<typename Table>
std::string supported_particles() {
    constexpr auto n = std::tuple_size_v<Table>;
    return supported_types_<Table>(std::make_index_sequence<n>{});
}

/** @brief Returns the name of @p o's Python type.
 *
 *  Used to build the error messages the dispatchers raise when they are given
 *  a particle they can not form an operator from.
 *
 *  @param[in] o The object to name the type of.
 *
 *  @return The value of `type(o).__name__`.
 */
std::string python_type_name(const py::object& o);

/** @brief Throws if @p kwargs is non-empty.
 *
 *  None of the operators take keyword arguments. Dispatching on positional
 *  arguments only keeps the dispatchers simple, so this rejects keyword
 *  arguments up front rather than silently ignoring them.
 *
 *  @param[in] name The name of the operator, for the error message.
 *  @param[in] kwargs The keyword arguments the operator was called with.
 *
 *  @throw pybind11::type_error if @p kwargs is non-empty. Strong throw
 *                              guarantee.
 */
void assert_no_kwargs(const char* name, const py::kwargs& kwargs);

// -----------------------------------------------------------------------------
// The dispatch function
// -----------------------------------------------------------------------------

/// Builds Op<T, U> from the leading arguments in @p args plus the particles.
template<template<typename...> class Op, typename LeadingArgs, typename T,
         typename U, std::size_t... L>
py::object construct_(const py::args& args, const py::object& lhs,
                      const py::object& rhs, std::index_sequence<L...>) {
    return py::cast(
      Op<T, U>(args[L].template cast<std::tuple_element_t<L, LeadingArgs>>()...,
               lhs.template cast<T>(), rhs.template cast<U>()));
}

/** @brief Builds the function the Python class for a two-particle operator
 *         dispatches through.
 *
 *  Every two-particle operator dispatches the same way: reject keyword
 *  arguments, treat no arguments as the default instantiation, check the
 *  argument count, then find the instantiation the two particles select and
 *  build it. The operators differ only in which instantiations they have,
 *  which one is the default, and whether the ctor takes anything before the
 *  particles, so those are the template parameters.
 *
 *  @tparam Op The operator class template, e.g. Coulomb.
 *  @tparam Table The instantiation table to dispatch over.
 *  @tparam DefaultPair The type_pair a no-argument call builds. Mirrors the
 *                      C++ default ctor's particle types.
 *  @tparam LeadingArgs The ctor arguments preceding the particles, as a tuple
 *                      of their types. Empty for every operator except
 *                      ExchangeCorrelation, whose functional comes first.
 *
 *  @param[in] name The operator's Python name, used in every error message.
 *  @param[in] arg_error How to finish "<name> ..." when the argument count is
 *                       wrong, e.g. "takes two particles".
 *
 *  @return The dispatch function, ready to hand to export_dispatching_class.
 */
template<template<typename...> class Op, typename Table, typename DefaultPair,
         typename LeadingArgs = std::tuple<>>
py::cpp_function make_two_particle_dispatch(const char* name,
                                            const char* arg_error) {
    constexpr auto n_leading = std::tuple_size_v<LeadingArgs>;

    // N.B. The leading parameter is the class CPython passes to __new__. It is
    //      always the dispatching class itself, so it is ignored.
    auto impl = [name, arg_error](py::object, py::args args,
                                  py::kwargs kwargs) -> py::object {
        assert_no_kwargs(name, kwargs);

        // No arguments gives the same object as the C++ default ctor.
        if(args.size() == 0)
            return py::cast(Op<typename DefaultPair::lhs_type,
                               typename DefaultPair::rhs_type>{});

        if(args.size() != n_leading + 2)
            throw py::type_error(std::string(name) + " " + arg_error + ".");

        auto lhs = py::reinterpret_borrow<py::object>(args[n_leading]);
        auto rhs = py::reinterpret_borrow<py::object>(args[n_leading + 1]);

        auto make = [&](auto tag) {
            using T = typename decltype(tag)::lhs_type;
            using U = typename decltype(tag)::rhs_type;
            return construct_<Op, LeadingArgs, T, U>(
              args, lhs, rhs, std::make_index_sequence<n_leading>{});
        };

        auto result = select_pair<Table>(lhs, rhs, make);
        if(!result.is_none()) return result;

        throw py::type_error(
          std::string(name) + " can not describe the interaction of a " +
          python_type_name(lhs) + " with a " + python_type_name(rhs) +
          ". Supported combinations are: " + supported_combinations<Table>() +
          ".");
    };

    return py::cpp_function(impl);
}

// -----------------------------------------------------------------------------
// The dispatching class
// -----------------------------------------------------------------------------

/** @brief Adds a single, dispatching class named @p name to @p m.
 *
 *  Classes like `Kinetic` are templated on the type of the particle they
 *  describe, and pybind11 can only export one Python class per instantiation.
 *  Exporting them under their instantiation-specific names forces Python users
 *  to know which C++ instantiation their particle implies, and the list of
 *  names grows every time an instantiation is added. This function creates the
 *  single class Python sees instead; the instantiations stay exported, but
 *  under underscore-prefixed names Python users are never expected to type.
 *
 *  The returned class uses @p dispatch as its `__new__`, so calling it returns
 *  an instance of one of @p impls rather than of itself. CPython skips
 *  `__init__` in that case, because `type.__call__` only runs it when the
 *  object `__new__` returned passes `PyObject_TypeCheck`, a real, MRO-based
 *  subtype test. The instantiations are separately registered as virtual
 *  subclasses, which is what makes `isinstance(Kinetic(), Kinetic)` true even
 *  though the object is really a `_KineticElectron`; registration changes what
 *  isinstance answers without touching the MRO, so it does not disturb the
 *  skipped `__init__`.
 *
 *  N.B. That leaves one invariant to respect: the classes in @p impls must not
 *  become actual subclasses of the class created here. If one does,
 *  `PyObject_TypeCheck` starts passing and pybind11's `__init__` runs a second
 *  time on an already-constructed object.
 *
 *  N.B. This is the operator analog of what `PyBraKet` does for `BraKet`, but
 *  the resolution happens eagerly, at construction, instead of being deferred
 *  to a cast. Operators are embedded in `Fock`/`Hamiltonian`, cloned, and
 *  dispatched through `OperatorVisitor`, so Python has to be handed a real
 *  `Kinetic<Electron>`, not something that only becomes one at a boundary.
 *
 *  @param[in,out] m The module to add the class to.
 *  @param[in] name The name to give the class in Python.
 *  @param[in] impls The concrete classes @p dispatch can return, as returned
 *                   by export_instantiations. Registered as virtual subclasses
 *                   so that isinstance works.
 *  @param[in] dispatch Installed as `__new__`, so it is called with the class
 *                      followed by the constructor's arguments, and returns
 *                      the instance of the concrete class they select. Taken as
 * a py::object, not a py::cpp_function, because passing an existing
 * cpp_function by value picks pybind11's templated ctor over its copy ctor and
 * then fails to deduce a signature from cpp_function's own operator().
 *  @param[in] doc The docstring to give the class, or nullptr for none.
 *
 *  @return The class that was added to @p m.
 *
 *  @throw std::bad_alloc if there is a problem allocating. Weak throw
 *                        guarantee.
 */
py::object export_dispatching_class(python_module_reference m, const char* name,
                                    py::tuple impls, py::object dispatch,
                                    const char* doc = nullptr);

} // namespace chemist::qm_operator::detail_
