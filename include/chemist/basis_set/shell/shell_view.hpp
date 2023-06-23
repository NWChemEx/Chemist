/*
 * Copyright 2022 NWChemEx-Project
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
#include <chemist/basis_set/shell/shell.hpp>
#include <chemist/detail_/view/traits.hpp>

namespace chemist {

template<typename T>
class ShellView {
private:
    /// Works out view types for us
    using traits_type = ViewTraits<T>;

    /// Typedef so we don't need "typename" and "template" below
    template<typename U>
    using apply_const_ref = typename traits_type::template apply_const_ref<U>;

    /// Typedef so we don't need "typename" and "template" below
    template<typename U>
    using ptr_type = typename traits_type::template apply_const_ptr<U>;

    /// The value of the template parameter with all qualifiers removed.
    using clean_value_type = typename traits_type::type;

    /// Is the value of the template parameter const-qualified?
    constexpr bool is_const_v = traits_type::is_const_v;

public:
    /// The type *this is a view of
    using shell_type = Shell<clean_value_type>;

    /// The type of a mutable reference to a Shell
    using shell_reference = shell_type&;

    /// The type of the contracted Gaussian common to AOs in *this
    using contracted_gaussian_type =
      typename shell_type::contracted_gaussian_type;

    /// The type of a read-only reference to a contracted Gaussian
    using const_cg_reference = typename shell_type::const_cg_reference;

    /// The type of a mutable reference to a contracted Gaussian
    using contracted_gaussian_reference =
      std::conditional_t<is_const_v, const_cg_reference,
                         typename shell_type::contracted_gaussian_reference>;

    /// Type used for storing the angular momentum
    using angular_momentum_type = typename shell_type::angular_momentum_type;

    /// Type of a (possibly) mutable reference to the angular momentum
    using angular_momentum_reference = apply_const_ref<angular_momentum_type>;

    /// Type of a read-only reference to the angular momentum
    using const_angular_momentum_reference =
      typename shell_type::const_angular_momentum_reference;

    /// Type used to return Cartesian vs. spherical
    using pure_type = typename shell_type::pure_type;

    /// Type of a (possibly) mutable reference to the purity
    using pure_reference = apply_const_ref<pure_type>;

    /// Type of a read-only reference to the purity
    using const_pure_reference = typename shell_type::const_pure_reference;

    /// Unsigned integral type used for indexing and offsets
    using size_type = typename shell_type::size_type;

    ShellView(shell_reference shell2alias);

    ShellView(contracted_gaussian_reference cg, angular_momentum_reference l,
              pure_reference ao_type);

    /** @brief Returns the purity of the aliased shell.
     *
     *  @return The purity of the aliased shell as a read/write reference if
     *          @p T is non-const and a read-only reference if @p T is const.
     *
     *  @throw none No throw guarantee.
     */
    pure_reference pure() noexcept { return *m_pure_; }

    /** @brief Returns the purity of the aliased shell
     *
     *  @return A read-only reference to whether the aliased shell is pure.
     *
     *  @throw non No throw guarantee.
     */
    const_pure_reference pure() const noexcept { return *m_pure_; }

    /** @brief Returns the total angular momentum of the aliased shell
     *
     *  @return The total angular momentum of the shell as a read/write
     *          reference if @p T is non-const and a read-only reference if it
     *          is const.
     *
     *  @throw none No throw guarantee.
     */
    angular_momentum_reference l() noexcept { return *m_l_; }

    /** @brief Returns the total angular momentum of the aliased shell
     *
     *  @return The total angular momentum of the shell as a read-only reference
     *
     *  @throw none No throw guarantee.
     */
    const_angular_momentum_reference l() const noexcept { return *m_l_; }

    /** @brief Returns the number of unique primitives in the ShellView.
     *
     *  The backend of this class only stores the primitives for one CGTO. This
     *  function can be used to retrieve the number of primitives in that CGTO.
     *
     *  @return The number of primitives in the one unique CGTO.
     *
     *  @throw None.
     *
     *  Complexity: constant
     */
    size_type n_unique_primitives() const noexcept { return *m_cg_.size(); }

    /** @brief Returns the @p i-th unique primitive in this ShellView.
     *
     *  @param[in] i The index of the requested unique primitive. Must be in the
     *               range [0, n_unique_primitives()).
     *
     *  @return A read-/write-able reference to the requested primitive.
     *
     *  @throw std::out_of_range if @p i is not in the range [0,
     *                           n_unique_primitives()). Strong throw guarantee.
     *
     *  Complexity: Constant.
     */
    decltype(auto) unique_primitive(size_type i) { return *m_cg_[i]; }

    /** @brief Returns the @p i-th unique primitive in this ShellView.
     *
     *  @param[in] i The index of the requested unique primitive. Must be in the
     *               range [0, n_unique_primitives()).
     *
     *  @return A read-only reference to the requested primitive.
     *
     *  @throw std::out_of_range if @p i is not in the range [0,
     *                           n_unique_primitives()). Strong throw guarantee.
     *
     *  Complexity: Constant.
     */
    decltype(auto) unique_primitive(size_type i) const { return *m_cg_[i]; }

    bool operator==(const ShellView& rhs) const noexcept;
    bool operator!=(const ShellView& rhs) const noexcept {
        return (*this == rhs);
    }

private:
    contracted_gaussian_reference m_cg_;

    ptr_type<pure_type> m_pure_;

    ptr_type<angular_momentum_type> m_l_;

}; // ShellView class

extern template class ShellView<double>;
extern template class ShellView<const double>;
extern template class ShellView<float>;
extern template class ShellView<const float>;

} // namespace chemist
