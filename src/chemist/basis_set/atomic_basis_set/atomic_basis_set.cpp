#include "../../point/point_pimpl.hpp"
#include "atomic_basis_set_pimpl.hpp"
#include "chemist/basis_set/atomic_basis_set.hpp"
#include <cassert>
namespace chemist {

template<typename T>
AtomicBasisSet<T>::AtomicBasisSet() :
  AtomicBasisSet(std::make_unique<center_pimpl_t>(),
                 std::make_unique<point_pimpl_t>()) {}

template<typename T>
AtomicBasisSet<T>::AtomicBasisSet(const AtomicBasisSet<T>& rhs) :
  AtomicBasisSet(std::make_unique<center_pimpl_t>(*rhs.m_pimpl_),
                 std::make_unique<point_pimpl_t>(rhs.x(), rhs.y(), rhs.z())) {}

template<typename T>
AtomicBasisSet<T>::AtomicBasisSet(AtomicBasisSet<T>&& rhs) noexcept = default;

template<typename T>
AtomicBasisSet<T>::AtomicBasisSet(const std::string& name, size_type atomic_n,
                                  T x, T y, T z) :
  AtomicBasisSet(std::make_unique<center_pimpl_t>(name, atomic_n),
                 std::make_unique<point_pimpl_t>(x, y, z)) {}

template<typename T>
AtomicBasisSet<T>::AtomicBasisSet(T x, T y, T z) :
  AtomicBasisSet(std::make_unique<center_pimpl_t>(),
                 std::make_unique<point_pimpl_t>(x, y, z)) {}

template<typename T>
AtomicBasisSet<T>::AtomicBasisSet(const std::string& name, size_type atomic_n) :
  AtomicBasisSet(std::make_unique<center_pimpl_t>(name, atomic_n),
                 std::make_unique<point_pimpl_t>()) {}

template<typename T>
AtomicBasisSet<T>::AtomicBasisSet(center_pimpl_ptr_t cpimpl,
                                  point_pimpl_ptr_t ppimpl) noexcept :
  m_pimpl_(std::move(cpimpl)),
  Point<T>(std::move(ppimpl)),
  utilities::IndexableContainerBase<AtomicBasisSet<T>>() {}

template<typename T>
AtomicBasisSet<T>& AtomicBasisSet<T>::operator=(const AtomicBasisSet<T>& rhs) {
    return *this = std::move(AtomicBasisSet<T>(rhs));
}

template<typename T>
AtomicBasisSet<T>& AtomicBasisSet<T>::operator=(
  AtomicBasisSet<T>&& rhs) noexcept = default;

template<typename T>
AtomicBasisSet<T>::~AtomicBasisSet() noexcept = default;

template<typename T>
void AtomicBasisSet<T>::add_shell(pure_type pure, am_type l, param_set cs,
                                  param_set es) {
    assert(m_pimpl_ != nullptr);
    m_pimpl_->add_shell(pure, l, std::move(cs), std::move(es));
}

template<typename T>
std::string AtomicBasisSet<T>::basis_set_name() const {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->basis_set_name();
}

template<typename T>
typename AtomicBasisSet<T>::size_type AtomicBasisSet<T>::atomic_number() const {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->atomic_number();
}

template<typename T>
typename AtomicBasisSet<T>::size_type AtomicBasisSet<T>::n_aos()
  const noexcept {
    size_type counter = 0;
    for(auto&& shell_i : *this) counter += shell_i.size();
    return counter;
}

template<typename T>
typename AtomicBasisSet<T>::ao_reference AtomicBasisSet<T>::ao(size_type i) {
    for(ShellView<T> shell_i : *this) {
        if(i < shell_i.size())
            return shell_i[i];
        else
            i -= shell_i.size();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, naos())");
}

template<typename T>
typename AtomicBasisSet<T>::const_ao_reference AtomicBasisSet<T>::ao(
  size_type i) const {
    for(auto&& shell_i : *this) {
        if(i < shell_i.size())
            return shell_i[i];
        else
            i -= shell_i.size();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, naos())");
}

template<typename T>
typename AtomicBasisSet<T>::size_type AtomicBasisSet<T>::n_unique_primitives()
  const noexcept {
    size_type counter = 0;
    for(auto&& shell_i : *this) counter += shell_i.n_unique_primitives();
    return counter;
}

template<typename T>
typename AtomicBasisSet<T>::primitive_reference
AtomicBasisSet<T>::unique_primitive(size_type i) {
    for(ShellView<T> shell_i : *this) {
        if(i < shell_i.n_unique_primitives())
            return shell_i.unique_primitive(i);
        else
            i -= shell_i.n_unique_primitives();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_unique_primitives())");
}

template<typename T>
typename AtomicBasisSet<T>::const_primitive_reference
AtomicBasisSet<T>::unique_primitive(size_type i) const {
    for(auto&& shell_i : *this) {
        if(i < shell_i.n_unique_primitives())
            return shell_i.unique_primitive(i);
        else
            i -= shell_i.n_unique_primitives();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_unique_primitives())");
}

template<typename T>
typename AtomicBasisSet<T>::size_type AtomicBasisSet<T>::size_()
  const noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->size();
}

template<typename T>
typename AtomicBasisSet<T>::reference AtomicBasisSet<T>::at_(size_type i) {
    assert(m_pimpl_ != nullptr);
    auto ptr1 = m_pimpl_->at(i);
    auto ptr2 = this->point_alias();
    Shell<T> shell(std::move(ptr1), std::move(ptr2));
    return reference(std::move(shell));
}

template<typename T>
typename AtomicBasisSet<T>::const_reference AtomicBasisSet<T>::at_(
  size_type i) const {
    assert(m_pimpl_ != nullptr);
    auto ptr1 = m_pimpl_->at(i);
    auto ptr2 = this->point_alias();
    Shell<T> shell(std::move(ptr1), std::move(ptr2));
    return const_reference(std::move(shell));
}

template class AtomicBasisSet<double>;
template class AtomicBasisSet<float>;

} // namespace chemist
