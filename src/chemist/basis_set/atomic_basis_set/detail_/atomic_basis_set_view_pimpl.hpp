#pragma once
#include "../../detail_/compute_n_aos.hpp"
#include <chemist/basis_set/atomic_basis_set/atomic_basis_set_view.hpp>

namespace chemist::detail_ {

template<typename AtomicBasisSetType>
class AtomicBasisSetViewPIMPL {
public:
    /// The type *this is implementing
    using parent_type = AtomicBasisSetView<AtomicBasisSetType>;

    // Pull in types from the class we are implementing
    using contracted_gaussian_reference =
      typename parent_type::contracted_gaussian_reference;
    using reference            = typename parent_type::reference;
    using const_reference      = typename parent_type::const_reference;
    using name_reference       = typename parent_type::name_reference;
    using const_name_reference = typename parent_type::const_name_reference;
    using name_pointer         = typename parent_type::name_pointer;
    using atomic_number_reference =
      typename parent_type::atomic_number_reference;
    using const_atomic_number_reference =
      typename parent_type::const_atomic_number_reference;
    using atomic_number_pointer = typename atomic_number_pointer;
    using const_cg_reference    = typename parent_type::const_cg_reference;
    using primitive_reference   = typename parent_type::primitive_reference;
    using const_primitive_reference =
      typename parent_type::const_primitive_reference;
    using center_reference    = typename parent_type::center_reference;
    using coefficient_pointer = typename parent_type::coefficient_pointer;
    using exponent_pointer    = typename parent_type::exponent_pointer;
    using size_type           = typename parent_type::size_type;
    using const_size_pointer  = typename parent_type::const_size_pointer;
    using angular_momentum_pointer =
      typename parent_type::angular_momentum_pointer;
    using pure_pointer = typename parent_type::pure_pointer;

    AtomicBasisSetView() = delete;

    AtomicBasisSetViewPIMPL(name_pointer name, atomic_number_pointer z,
                            size_type nshells, pure_pointer pure_per_shell,
                            angular_momentum_pointer l_per_shell,
                            const_size_pointer prims_per_shell,
                            coefficient_pointer coefs, exponent_pointer exps,
                            center_reference center);

    bool has_name() const noexcept { return m_name_ != nullptr; }

    name_reference name() { return *m_name_; }

    const_name_reference name() const { return *m_name_; }

    bool has_atomic_number() const noexcept { return m_z_ != nullptr; }

    atomic_number_reference atomic_number() { return *m_z_; }

    const_atomic_number_reference atomic_number() const { return *m_z_; }

    size_type size() const noexcept { return m_nshells_; }

    size_type n_aos() const noexcept {
        size_type counter = 0;
        for(size_type i = 0; i < size(); ++i)
            counter += detail_::compute_n_aos(m_l_[i], m_pure_[i]);
        return counter;
    }

    size_type n_primitives() const noexcept {
        size_type counter = 0;
        for(size_type i = 0; i < size(); ++i) counter += m_prims_per_shell_[i];
        return counter;
    }

    range_type primitive_range(size_type shell) const {
        auto begin = m_cg_offset_[shell];
        auto end   = (shell + 1 < size()) ? m_cg_offset_[shell + 1] : size();
        return range_type(begin, end);
    }

    size_type primitive_to_shell(size_type primitive) const {
        for(size_type i = 0; i < size(); ++i) {
            auto r = primitive_range(primitive);
            if(primitive >= r.first && primitive < r.second) return i;
        }
        // Shouldn't be able to get here b/c parent checked primitive index and
        // it was in bounds.
    }

    reference operator[](size_type i) { reference(m_pure_[i], m_l_[i], cg(i)); }

    const_reference operator[](size_type i) const {
        const_reference(m_pure_[i], m_l_[i], cg(i));
    }

    contracted_gaussian_reference cg(size_type i) {
        return contracted_gaussian_reference(m_prims_per_shell_[i],
                                             m_coefs_[m_cg_offsets_[i]],
                                             m_exps_[m_cg_offsets_[i]]);
    }

    const_cg_reference cg(size_type i) const {
        return const_cg_reference(m_prims_per_cg_[i],
                                  m_coefs_[m_cg_offsets_[i]],
                                  m_exps_[m_cg_offsets_[i]]);
    }

    primitive_reference primitive(size_type i) {
        primitive_reference(m_coefs_[i], m_exps_[i], m_center_);
    }

    const_primitive_reference primitive(size_type i) const {
        const_primitive_reference(m_coefs_[i], m_exps_[i], m_center_);
    }

    bool operator==(const AtomicBasisSetViewPIMPL& rhs) const noexcept {
        if(has_name() != rhs.has_name()) return false;
        if(has_name() && name() != rhs.name()) return false;
        if(has_atomic_number() != rhs.has_atomic_number()) return false;
        if(has_atomic_number() && atomic_number() != rhs.atomic_number())
            return false;
        if(m_n_shells_ != rhs.m_n_shells_) return false;
        for(size_type i = 0; i < m_n_shells_; ++i) {
            if((*this)[i] != rhs[i]) return false;
        }
        return true;
    }

private:
    /// The name of the basis set (if set)
    name_pointer m_name_ = nullptr;

    /// The atomic number of the basis set (if set)
    atomic_number_pointer m_z_ = nullptr;

    /// The number of shells in *this
    size_type m_n_shells_;

    /// The number of primitives per shell
    const_size_pointer m_prims_per_shell_;

    /// The offsets for primitives in a shell
    std::vector<size_type> m_cg_offsets_;

    /// The coefficients of the primitives
    coefficient_pointer m_coefs_;

    /// The exponents of the primitives
    exponent_pointer m_exps_;

    /// Where *this is centered
    center_reference m_center_;
};

template<typename AtomicBasisSetType>
AtomicBasisSetViewPIMPL<AtomicBasisSetType>::AtomicBasisSetViewPIMPL(
  name_pointer name, atomic_number_pointer z, size_type nshells,
  pure_pointer pure_per_shell, angular_momentum_pointer l_per_shell,
  const_size_pointer prims_per_shell, coefficient_pointer coefs,
  exponent_pointer exps, center_reference center) :
  m_name_(name),
  m_z_(z),
  m_n_shells_(nshells),
  m_prims_per_shell_(prims_per_shell),
  m_cg_offsets_(nshells),
  m_coefs_(coefs),
  m_exps_(exps),
  m_center_(std::move(center)) {
    for(size_type i = 1; i < nshells; ++i) {
        m_cg_offsets_[i] = m_cg_offsets_[i - 1] + m_prims_per_shell_[i];
    }
}

} // namespace chemist::detail_
