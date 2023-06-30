#pragma once

namespace chemist::detail_ {

template<typename AtomicBasisSetType>
class AtomicBasisSetViewPIMPL {
public:
    using parent_type = AtomicBasisSetView<AtomicBasisSetType>;

    using contracted_gaussian_reference =
      typename parent_type::contracted_gaussian_reference;
    using const_cg_reference  = typename parent_type::const_cg_reference;
    using primitive_reference = typename parent_type::primitive_reference;
    using const_primitive_reference =
      typename parent_type::const_primitive_reference;
    using center_reference    = typename parent_type::center_reference;
    using coefficient_pointer = typename parent_type::coefficient_pointer;
    using exponent_pointer    = typename parent_type::exponent_pointer;
    using size_type           = typename parent_type::size_type;
    using const_size_pointer  = typename parent_type::const_size_pointer;

    shell_reference operator[](size_type i) {
        shell_reference(m_pure_[i], m_l_[i], cg(i));
    }

    const_shell_reference operator[](size_type i) const {
        const_shell_reference(m_pure_[i], m_l_[i], cg(i));
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

} // namespace chemist::detail_
