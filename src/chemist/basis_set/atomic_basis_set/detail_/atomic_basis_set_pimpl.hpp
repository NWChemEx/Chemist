/*
 * Copyright 2023 NWChemEx-Project
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

#include <chemist/basis_set/atomic_basis_set/atomic_basis_set.hpp>
#include <optional>

namespace chemist::detail_ {

/** @brief Holds the state for an AtomicBasisSet object.
 *
 *  @tparam ShellType expected to be a non-cv qualified specialization of Shell.
 */
template<typename ShellType>
class AtomicBasisSetPIMPL {
public:
    /// Type *this is implementing
    using parent_type = AtomicBasisSet<ShellType>;

    // -------------------------------------------------------------------------
    // -- Pull in types from parent_type
    // -------------------------------------------------------------------------

    using name_type             = typename parent_type::name_type;
    using atomic_number_type    = typename parent_type::atomic_number_type;
    using pure_type             = typename parent_type::pure_type;
    using pure_reference        = typename parent_type::pure_reference;
    using const_pure_reference  = typename parent_type::const_pure_reference;
    using angular_momentum_type = typename parent_type::angular_momentum_type;
    using angular_momentum_reference =
      typename parent_type::angular_momentum_reference;
    using const_angular_momentum_reference =
      typename parent_type::const_angular_momentum_reference;
    using center_type      = typename parent_type::center_type;
    using coefficient_type = typename parent_type::coefficient_type;
    using exponent_type    = typename parent_type::exponent_type;
    using contracted_gaussian_reference =
      typename parent_type::contracted_gaussian_reference;
    using const_cg_reference    = typename parent_type::const_cg_reference;
    using shell_reference       = typename parent_type::reference;
    using const_shell_reference = typename parent_type::const_reference;
    using size_type             = typename parent_type::size_type;
    using range_type            = typename parent_type::range_type;
    using primitive_reference   = typename parent_type::primitive_reference;
    using const_primitive_reference =
      typename parent_type::const_primitive_reference;

    explicit AtomicBasisSetPIMPL(center_type center) noexcept :
      m_center(std::move(center)) {}

    /// If set, will be the name of the basis set
    std::optional<name_type> m_name;

    /// If set, will be the atomic number associated with this basis set
    std::optional<atomic_number_type> m_z;

    /// If set, where *this is centered
    std::optional<center_type> m_center;

    void add_shell(pure_type pure, angular_momentum_type l,
                   contracted_gaussian_reference cg);

    shell_reference operator[](size_type i) {
        shell_reference(pure(i), l(i), cg(i));
    }

    const_shell_reference operator[](size_type i) const {
        const_shell_reference(pure(i), l(i), cg(i));
    }

    angular_momentum_reference l(size_type i) noexcept { return m_l_[i]; }

    const_angular_momentum_reference l(size_type i) const noexcept {
        return m_l_[i];
    }

    pure_reference pure(size_type i) noexcept { return m_pure_[i]; }

    const_pure_reference pure(size_type i) const noexcept { return m_pure_[i]; }

    contracted_gaussian_reference cg(size_type i) {
        return contracted_gaussian_reference(
          m_prims_per_cg_[i], m_coefs_[m_cg_offsets_[i]],
          m_exps_[m_cg_offsets_[i]], m_center.value());
    }

    const_cg_reference cg(size_type i) const {
        return const_cg_reference(m_prims_per_cg_[i],
                                  m_coefs_[m_cg_offsets_[i]],
                                  m_exps_[m_cg_offsets_[i]], m_center.value());
    }

    range_type primitive_range(size_type shell) const {
        const auto begin   = m_cg_offsets_[shell];
        const bool is_last = shell < size();
        const auto end = (is_last ? m_cg_offsets_[shell + 1] : n_primitives());
        return range_type{begin, end};
    }

    size_type prim2shell(size_type i) const { return m_prim2shell_[i]; }

    primitive_reference primitive(size_type i) {
        primitive_reference(m_coefs_[i], m_exps_[i], m_center.value());
    }

    const_primitive_reference primitive(size_type i) const {
        const_primitive_reference(m_coefs_[i], m_exps_[i]);
    }

    size_type size() const noexcept { return m_pure_.size(); }

    size_type n_primitives() const noexcept { return m_exps_.size(); }

    bool operator==(const AtomicBasisSetPIMPL& rhs) const noexcept;

private:
    template<typename CenterType>
    void set_or_check_center_(CenterType&& center);

    /// i-th element is number of primitives in cg i
    std::vector<size_type> m_prims_per_cg_;

    /// i-th element is the index of the first primitive in cg i
    std::vector<size_type> m_cg_offsets_;

    /// i-th element is the shell index of primitive i
    std::vector<size_type> m_prim2shell_;

    /// i-th element is the coefficient of the i-th primitive
    std::vector<coefficient_type> m_coefs_;

    std::vector<exponent_type> m_exps_;

    std::vector<pure_type> m_pure_;

    std::vector<angular_momentum_type> m_l_;
};

#define ATOMIC_BASIS_SET_PIMPL AtomicBasisSetPIMPL<ShellType>

template<typename ShellType>
void ATOMIC_BASIS_SET_PIMPL::add_shell(pure_type pure, angular_momentum_type l,
                                       contracted_gaussian_reference cg) {
    set_or_check_center(cg.center());

    // This shell will have index...
    auto shell_i = size();

    // Add the purity/angular momentum
    m_pure_.push_back(pure);
    m_l_.push_back(l);

    // Record how many primitives the shell's cg has
    m_prims_per_cg_.push_back(cg.size());

    /// The first primitive will go at the end of cg
    m_cg_offsets_.push_back(m_coefs_.size());

    for(size_type i = 0; i < cg.size(); ++i) {
        m_coefs_.push_back(cg.coefficient(i));
        m_exps_.push_back(cg.exponent(i));
        m_prim2shell_.push_back(shell_i);
    }
}

template<typename ShellType>
bool ATOMIC_BASIS_SET_PIMPL::operator==(
  const AtomicBasisSetPIMPL& rhs) const noexcept {
    // N.B. cg_offsets and prim2shell exist for convenience. Their stati is
    // fully determined by prims_per_cg
    if(m_name != rhs.m_name) return false;
    if(m_z != rhs.m_z) return false;
    if(m_center != rhs.m_center) return false;
    if(m_prims_per_cg_ != rhs.m_prims_per_cg_) return false;
    if(m_coefs_ != rhs.m_coefs_) return false;
    if(m_exps_ != rhs.m_exps_) return false;
    if(m_pure_ != rhs.m_pure_) return false;
    if(m_l_ != rhs.m_l_) return false;
    return true;
}

template<typename ShellType>
template<typename CenterType>
void ATOMIC_BASIS_SET_PIMPL::set_or_check_center_(CenterType&& center) {
    if(!m_center.has_value()) {
        m_center.emplace(std::forward<CenterType>(center));
    } else {
        if(m_center.value() != center)
            throw std::runtime_error("Provided contracted Gaussian is not "
                                     "centered on the same point");
    }
}

#undef ATOMIC_BASIS_SET_PIMPL

} // namespace chemist::detail_
