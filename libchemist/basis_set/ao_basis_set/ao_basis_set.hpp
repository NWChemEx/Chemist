#pragma once
#include "libchemist/basis_set/center/center.hpp"
#include "libchemist/basis_set/detail_/flattened_view.hpp"
#include <utilities/containers/indexable_container_base.hpp>

namespace libchemist {
namespace detail_ {
template<typename T>
class AOBasisSetPIMPL;
} // namespace detail_

template<typename T>
class AOBasisSet : public utilities::IndexableContainerBase<AOBasisSet<T>> {
private:
    /// Type of this instance
    using my_type = AOBasisSet<T>;
    /// Type of the base class implementing Container API
    using base_type = utilities::IndexableContainerBase<my_type>;
    /// Type of the PIMPL implementing this class
    using pimpl_type = detail_::AOBasisSetPIMPL<T>;

public:
    /// Type of the centers in this basis set
    using value_type = Center<T>;
    /// Type of a read-/write-able reference to a Center
    using reference = Center<T>&;
    /// Type of a read-only reference to a Center
    using const_reference = const Center<T>&;
    /// Unsigned integral type used for indexing/offsets
    using size_type = typename base_type::size_type;

    AOBasisSet();
    AOBasisSet(const AOBasisSet<T>& rhs);
    AOBasisSet(AOBasisSet<T>&& rhs) noexcept;
    AOBasisSet& operator=(const AOBasisSet<T>& rhs);
    AOBasisSet& operator=(AOBasisSet<T>&& rhs) noexcept;
    ~AOBasisSet() noexcept;

    void add_center(value_type center);

    /** @brief Determines the largest total angular momentum of the shells in
     *         the basis set.
     *
     *  Many integral libraries preallocate a scratch buffer given the maximum
     *  total angular momentum of any particular shell. This function is a
     *  convenience function for determining that value for the current basis
     *  set.
     *
     *  @return The maximum total angular momentum of the shell with the largest
     *          total angular momentum.
     *
     *  @throw std::runtime_error if this basis set does not contain any shells.
     *         Strong throw guarantee.
     *
     *  @note Since a total angular momentum of 0 is a valid result, we instead
     *        opt to throw if this function is called on an empty basis set.
     */
    size_type max_l() const;

    // ------------------------- Shells ----------------------------------------
    /// Type used to model a Shell
    using shell_type = Shell<T>;
    /// Type used to model a read-/write-able reference to a Shell
    using shell_reference = ShellView<T>;
    /// Type used to model a read-only reference to a Shell
    using const_shell_reference = ShellView<const T>;
    /// Type of a read-/write-able view of the basis set as a set of Shells
    using flattened_shells =
      detail_::FlattenedView<shell_reference, const_shell_reference>;
    /// Type of a read-only view of the basis set as a set of Shells
    using const_flattened_shells =
      detail_::FlattenedView<const_shell_reference, const_shell_reference>;

    size_type n_shells() const noexcept;
    shell_reference shell(size_type i);
    const_shell_reference shell(size_type i) const;
    flattened_shells shells();
    const_flattened_shells shells() const;

    // ---------------------------- AOs ----------------------------------------
    using ao_type            = ContractedGaussian<T>;
    using ao_reference       = ContractedGaussianView<T>;
    using const_ao_reference = ContractedGaussianView<const T>;
    using flattened_aos =
      detail_::FlattenedView<ao_reference, const_ao_reference>;
    using const_flattened_aos =
      detail_::FlattenedView<const_ao_reference, const_ao_reference>;

    size_type n_aos() const noexcept;
    ao_reference ao(size_type i);
    const_ao_reference ao(size_type i) const;
    flattened_aos aos();
    const_flattened_aos aos() const;

    // ---------------------------- Primitives ---------------------------------
    using primitive_type            = Primitive<T>;
    using primitive_reference       = PrimitiveView<T>;
    using const_primitive_reference = PrimitiveView<const T>;
    using flattened_primitives =
      detail_::FlattenedView<primitive_reference, const_primitive_reference>;
    using const_flattened_primitives =
      detail_::FlattenedView<const_primitive_reference,
                             const_primitive_reference>;

    size_type n_unique_primitives() const noexcept;
    primitive_reference unique_primitive(size_type i);
    const_primitive_reference unique_primitive(size_type i) const;
    flattened_primitives unique_primitives();
    const_flattened_primitives unique_primitives() const;

private:
    /// Allows the base class to implement container API
    friend base_type;
    /// Implements `size()` function
    size_type size_() const noexcept;
    /// Implements `operator[](size_type i)`
    reference at_(size_type i);
    /// Implements `operator[](size_type i)const`
    const_reference at_(size_type i) const;
    /// The instance actually implementing the API
    std::unique_ptr<detail_::AOBasisSetPIMPL<T>> m_pimpl_;
}; // class AOBasisSet

extern template class AOBasisSet<double>;
extern template class AOBasisSet<float>;

} // namespace libchemist
