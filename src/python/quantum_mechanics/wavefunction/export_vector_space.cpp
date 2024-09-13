#include "export_wavefunction.hpp"
#include <chemist/quantum_mechanics/wavefunction/vector_space.hpp>

namespace chemist::wavefunction {

class PyVectorSpace : public VectorSpace {
public:
    using VectorSpace::VectorSpace;

protected:
    base_pointer clone_() const override {
        return base_pointer{}; // Won't be exposed to Python so should be okay
    }

    size_type size_() const noexcept override {
        PYBIND11_OVERRIDE_PURE(size_type, VectorSpace, size_, );
    }

    bool are_equal_(const VectorSpace& other) const noexcept override {
        PYBIND11_OVERRIDE_PURE(bool, VectorSpace, are_equal_, other);
    }
};

void export_vector_space(python_module_reference m) {
    python_class_type<VectorSpace, PyVectorSpace>(m, "VectorSpace")
      .def(pybind11::init<>())
      .def("size", &VectorSpace::size)
      .def("are_equal", &VectorSpace::are_equal)
      .def("are_different", &VectorSpace::are_different);
}

} // namespace chemist::wavefunction
