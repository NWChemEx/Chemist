#include "ao_basis_set_pimpl.hpp"
#include "chemist/basis_set/ao_basis_set.hpp"
#include "chemist/detail_/hashing.hpp"
#include <cassert>

namespace chemist {

#define AO_BS AOBasisSet<T>

// ------------------ Constructors/Assignment Operators -----------------------
template<typename T>
AO_BS::AOBasisSet() : m_pimpl_(std::make_unique<pimpl_type>()) {}

template<typename T>
AO_BS::AOBasisSet(const AO_BS& rhs) :
  m_pimpl_(std::make_unique<pimpl_type>(*(rhs.m_pimpl_))) {}

template<typename T>
AO_BS::AOBasisSet(AO_BS&& rhs) noexcept = default;

template<typename T>
AO_BS& AO_BS::operator=(const AO_BS& rhs) {
    return *this = std::move(AOBasisSet<T>(rhs));
}

template<typename T>
AO_BS& AO_BS::operator=(AO_BS&& rhs) noexcept = default;

template<typename T>
AO_BS::~AOBasisSet() noexcept = default;

template<typename T>
void AO_BS::add_center(value_type center) {
    assert(m_pimpl_ != nullptr);
    m_pimpl_->add_center(std::move(center));
}

template<typename T>
typename AO_BS::size_type AO_BS::max_l() const {
    if(this->empty())
        throw std::runtime_error("Basis set contains no shells!!!");
    size_type l = 0;
    for(auto&& shell_i : shells()) l = std::max(l, shell_i.l());
    return l;
}

// ------------------------- Shell Fxns ----------------------------------------
template<typename T>
typename AO_BS::size_type AO_BS::n_shells() const noexcept {
    size_type counter = 0;
    for(auto&& x : *this) counter += x.size();
    return counter;
}

template<typename T>
typename AO_BS::shell_reference AO_BS::shell(size_type i) {
    for(auto&& x : *this) {
        if(i < x.size())
            return x[i];
        else
            i -= x.size();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_shells())");
}

template<typename T>
typename AO_BS::const_shell_reference AO_BS::shell(size_type i) const {
    for(auto&& x : *this) {
        if(i < x.size())
            return x[i];
        else
            i -= x.size();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_shells())");
}

template<typename T>
typename AO_BS::flattened_shells AO_BS::shells() noexcept {
    return flattened_shells(
      [&]() { return n_shells(); }, [&](size_type i) { return shell(i); },
      [&](size_type i) { return std::as_const(*this).shell(i); });
}

template<typename T>
typename AO_BS::const_flattened_shells AO_BS::shells() const noexcept {
    return const_flattened_shells([&]() { return n_shells(); },
                                  [&](size_type i) { return shell(i); },
                                  [&](size_type i) { return shell(i); });
}

template<typename T>
std::vector<typename AO_BS::size_type> AO_BS::shell_offsets() const {
    std::vector<size_type> rv{0};
    for(auto&& x : *this) { rv.push_back(rv.back() + x.size()); }
    return rv;
}

// ---------------------------- AOs --------------------------------------------

template<typename T>
typename AO_BS::size_type AO_BS::n_aos() const noexcept {
    size_type counter = 0;
    for(auto&& x : *this) counter += x.n_aos();
    return counter;
}

template<typename T>
typename AO_BS::ao_reference AO_BS::ao(size_type i) {
    for(auto&& x : *this) {
        if(i < x.n_aos())
            return x.ao(i);
        else
            i -= x.n_aos();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_aos())");
}

template<typename T>
typename AO_BS::const_ao_reference AO_BS::ao(size_type i) const {
    for(auto&& x : *this) {
        if(i < x.n_aos())
            return x.ao(i);
        else
            i -= x.n_aos();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_aos())");
}

template<typename T>
typename AOBasisSet<T>::flattened_aos AO_BS::aos() noexcept {
    return flattened_aos(
      [&]() { return n_aos(); }, [&](size_type i) { return ao(i); },
      [&](size_type i) { return std::as_const(*this).ao(i); });
}

template<typename T>
typename AOBasisSet<T>::const_flattened_aos AO_BS::aos() const noexcept {
    return const_flattened_aos([&]() { return n_aos(); },
                               [&](size_type i) { return ao(i); },
                               [&](size_type i) { return ao(i); });
}

template<typename T>
std::vector<typename AO_BS::size_type> AO_BS::ao_offsets() const {
    std::vector<size_type> rv{0};
    for(auto&& x : *this) { rv.push_back(rv.back() + x.n_aos()); }
    return rv;
}

// -------------------------- Primitives --------------------------------------

template<typename T>
typename AO_BS::size_type AO_BS::n_unique_primitives() const noexcept {
    size_type counter = 0;
    for(auto&& x : *this) counter += x.n_unique_primitives();
    return counter;
}

template<typename T>
typename AO_BS::primitive_reference AO_BS::unique_primitive(size_type i) {
    for(auto&& x : *this) {
        if(i < x.n_unique_primitives())
            return x.unique_primitive(i);
        else
            i -= x.n_unique_primitives();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_unique_primitives())");
}

template<typename T>
typename AO_BS::const_primitive_reference AO_BS::unique_primitive(
  size_type i) const {
    for(auto&& x : *this) {
        if(i < x.n_unique_primitives())
            return x.unique_primitive(i);
        else
            i -= x.n_unique_primitives();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_unique_primitives())");
}

template<typename T>
typename AOBasisSet<T>::flattened_primitives
AO_BS::unique_primitives() noexcept {
    return flattened_primitives(
      [&]() { return n_unique_primitives(); },
      [&](size_type i) { return unique_primitive(i); },
      [&](size_type i) { return std::as_const(*this).unique_primitive(i); });
}

template<typename T>
typename AOBasisSet<T>::const_flattened_primitives AO_BS::unique_primitives()
  const noexcept {
    return const_flattened_primitives(
      [&]() { return n_unique_primitives(); },
      [&](size_type i) { return unique_primitive(i); },
      [&](size_type i) { return unique_primitive(i); });
}

// -------------------------- Private Fxns -------------------------------------
template<typename T>
typename AOBasisSet<T>::size_type AO_BS::size_() const noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->size();
}

template<typename T>
typename AOBasisSet<T>::reference AO_BS::at_(size_type i) {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->at(i);
}

template<typename T>
typename AOBasisSet<T>::const_reference AO_BS::at_(size_type i) const {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->at(i);
}

template<typename T>
void AOBasisSet<T>::hash(detail_::Hasher& h) const {
    for(const auto& c : *this) { h(c.x(), c.y(), c.z()); }
    for(const auto&& s : this->shells()) { h(s.pure(), s.l()); }
    for(const auto&& p : this->unique_primitives()) {
        h(p.coefficient(), p.exponent());
    }
}

template class AOBasisSet<double>;
template class AOBasisSet<float>;

} // namespace chemist
