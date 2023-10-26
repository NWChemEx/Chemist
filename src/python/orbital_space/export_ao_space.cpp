#include "../pychemist.hpp"
#include "chemist/orbital_space/ao_space.hpp"
#include "chemist/orbital_space/base_space.hpp"
#include "export_ao_space.hpp"
namespace chemist {
namespace detail_ {

template<typename T>
void export_ao_space_(const char* name, python_module_reference m) {
    using aos_type   = chemist::orbital_space::AOSpace<T>;
    using basis_type = typename aos_type::basis_type;
    using size_type  = typename aos_type::size_type;

    python_class_type<aos_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<basis_type>()) // Constructor with basis_type argument
      .def("size", &aos_type::size)
      .def_property(
        "basis_set",
        static_cast<basis_type& (aos_type::*)()>(&aos_type::basis_set),
        [](aos_type& self, basis_type& basis) {
            self.basis_set() = std::move(basis);
        })
      .def("__eq__", [](const aos_type& self,
                        const aos_type& other) { return self == other; })
      .def("__ne__", [](const aos_type& self, const aos_type& other) {
          return self != other;
      });
}

} // namespace detail_
void export_ao_space(python_module_reference m) {
    detail_::export_ao_space_<basis_set::AOBasisSetD>("AOSpaceD", m);
    detail_::export_ao_space_<basis_set::AOBasisSetF>("AOSpaceF", m);
}

} // namespace chemist
