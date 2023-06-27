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

namespace chemist::detail_ {

template<typename ShellType>
class AtomicBasisSetPIMPL {
public:
    /// Type *this is implementing
    using parent_type = AtomicBasisSet<ShellType>;

    // -------------------------------------------------------------------------
    // -- Pull in types from parent_type
    // -------------------------------------------------------------------------

    using name_type = typename parent_type::name_type;

    using atomic_number_type = typename parent_type::atomic_number_type;

    using center_type = typename parent_type::center_type;

    using coefficient_type = typename parent_type::coefficient_type;

    using shell_reference = typename parent_type::shell_reference;

    using const_shell_reference = typename parent_type::const_shell_reference;

    using size_type = typename parent_type::size_type;

    /// If set, will be the name of the basis set
    std::optional<name_type> m_name;

    /// If set, will be the atomic number associated with this basis set
    std::optional<atomic_number_type> m_z;

    /// If set, where *this is centered
    std::optional<center_type> m_center;

    void set_or_check_center(const_point_view center) {
        if(!m_center.has_value()) {
            m_center.emplace(center);
        } else {
            if(m_center.value() != center)
                throw std::runtime_error("Provided contracted Gaussian is not "
                                         "centered on the same point");
        }
    }

    void add_contracted_gaussian(contracted_gaussian_reference cg) {
        set_or_check_center(cg.center());

        m_prims_per_cg_.push_back(cg.size());
        m_cg_offsets_.push_back(m_coefs_.size());

        for(size_type i = 0; i < cg.size(); ++i) {
            m_coefs_.push_back(cg.coefficient(i));
            m_exps_.push_back(cg.exponent(i));
        }
    }

    contracted_gaussian_reference cg(size_type i) {
        return contracted_gaussian_reference(m_prims_per_cg_[i],
                                             m_coefs_[m_cg_offsets_[i]],
                                             m_exps_[m_cg_offsets_[i]]);
    }

    const_contracted_gaussian_reference cg(size_type i) const {
        return const contracted_gaussian_reference(m_prims_per_cg_[i],
                                                   m_coefs_[m_cg_offsets_[i]],
                                                   m_exps_[m_cg_offsets_[i]]);
    }

    shell_reference operator[](size_type i) {
        shell_reference(m_pure_[i], m_l_[i], cg(i));
    }

    const_shell_reference operator[](size_type i) const {
        const_shell_reference(m_pure_[i], m_l_[i], cg(i));
    }

private:
    /// i-th element is number of primitives in cg i
    std::vector<size_type> m_prims_per_cg_;

    /// i-th element is the index of the first primitive in cg i
    std::vector<size_type> m_cg_offsets_;

    std::vector<coefficient_type> m_coefs_;

    std::vector<exponent_type> m_exps_;

    std::vector<pure_type> m_pure_;

    std::vector<angular_momentum_type> m_l_;
};

} // namespace chemist::detail_
