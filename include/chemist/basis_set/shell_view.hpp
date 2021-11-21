#pragma once
#include "chemist/basis_set/detail_/ao_basis_view_base.hpp"
#include "chemist/basis_set/shell.hpp"

namespace chemist {

template<typename T>
class ShellView : public detail_::AOBasisViewBase<T, Shell, ShellView<T>> {
private:
    /// The type of the class this class derives from
    using base_type = detail_::AOBasisViewBase<T, Shell, ShellView<T>>;

public:
    /// The type of the elements in this container
    using value_type = ContractedGaussian<std::remove_cv_t<T>>;

    /// Unsigned integral type used for indexing and offsets
    using size_type = typename base_type::size_type;

    /** @brief Creates a ShellView that does not alias any Shell instance
     *
     *  This ctor can be used to create a placeholder ShellView instance. The
     *  resulting instance does not alias any Shell instance and will likely
     *  segfault if used.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    ShellView();

    /// Pull the base class's ctors into scope
    using base_type::base_type;

    /** @brief Returns the purity of the aliased shell.
     *
     *  @return The purity of the aliased shell as a read/write reference if
     *          @p T is non-const and a read-only reference if @p T is const.
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) pure() noexcept { return this->pimpl().pure(); }

    /** @brief Returns the purity of the aliased shell
     *
     *  @return A read-only reference to whether the aliased shell is pure.
     *
     *  @throw non No throw guarantee.
     */
    decltype(auto) pure() const noexcept { return this->pimpl().pure(); }

    /** @brief Returns the total angular momentum of the aliased shell
     *
     *  @return The total angular momentum of the shell as a read/write
     *          reference if @p T is non-const and a read-only reference if it
     *          is const.
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) l() noexcept { return this->pimpl().l(); }

    /** @brief Returns the total angular momentum of the aliased shell
     *
     *  @return The total angular momentum of the shell as a read-only reference
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) l() const noexcept { return this->pimpl().l(); }

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
    decltype(auto) n_unique_primitives() const noexcept;

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
    decltype(auto) unique_primitive(size_type i);

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
    decltype(auto) unique_primitive(size_type i) const;
}; // ShellView class

template<typename T>
decltype(auto) ShellView<T>::n_unique_primitives() const noexcept {
    return this->pimpl().n_unique_primitives();
}

template<typename T>
decltype(auto) ShellView<T>::unique_primitive(size_type i) {
    return this->pimpl().unique_primitive(i);
}

template<typename T>
decltype(auto) ShellView<T>::unique_primitive(size_type i) const {
    return this->pimpl().unique_primitive(i);
}

extern template class ShellView<double>;
extern template class ShellView<const double>;
extern template class ShellView<float>;
extern template class ShellView<const float>;

} // namespace chemist
