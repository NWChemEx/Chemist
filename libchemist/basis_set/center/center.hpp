#pragma once
#include "libchemist/basis_set/shell/shell_view.hpp"
#include "libchemist/point/point.hpp"
#include <utilities/containers/indexable_container_base.hpp>

namespace libchemist {
namespace detail_ {
template<typename T>
class CenterPIMPL;
}
template<typename T>
class Center : public Point<T>,
               public utilities::IndexableContainerBase<Center<T>> {
private:
    /// Type of this class
    using my_type = Center<T>;
    /// Type of the IndexableContainerBase base class
    using container_base = utilities::IndexableContainerBase<my_type>;
    /// Type of the PIMPL implementing the center part
    using center_pimpl_t = detail_::CenterPIMPL<T>;
    /// Type of a pointer to the PIMPL for the center
    using center_pimpl_ptr_t = std::unique_ptr<center_pimpl_t>;
    /// Type of the PIMPL for the Point part
    using point_pimpl_t = detail_::PointPIMPL<T>;
    /// Type of a pointer to the PIMPL for the Point part
    using point_pimpl_ptr_t = std::unique_ptr<point_pimpl_t>;

public:
    using value_type      = Shell<T>;
    using reference       = ShellView<T>;
    using const_reference = ShellView<const T>;

    using ao_type            = typename value_type::value_type;
    using ao_reference       = typename value_type::reference;
    using const_ao_reference = typename value_type::const_reference;

    using primitive_type      = typename value_type::primitive_type;
    using primitive_reference = typename value_type::primitive_reference;
    using const_primitive_reference =
      typename value_type::const_primitive_reference;

    using size_type = typename container_base::size_type;
    using pure_type = ShellType;
    using am_type   = size_type;
    using param_set = std::vector<T>;

    Center();
    Center(const Center<T>& rhs);
    Center(Center<T>&& rhs) noexcept;
    Center<T>& operator=(const Center<T>& rhs);
    Center<T>& operator=(Center<T>&& rhs) noexcept;
    Center(T x, T y, T z);
    Center(center_pimpl_ptr_t cpimpl, point_pimpl_ptr_t ppimpl) noexcept;
    ~Center() noexcept;

    void add_shell(pure_type pure, am_type l, param_set cs, param_set es);

    size_type n_aos() const noexcept;
    ao_reference ao(size_type i);
    const_ao_reference ao(size_type i) const;

    size_type n_unique_primitives() const noexcept;
    primitive_reference unique_primitive(size_type i);
    const_primitive_reference unique_primitive(size_type i) const;

private:
    friend container_base;
    size_type size_() const noexcept;
    reference at_(size_type i);
    const_reference at_(size_type i) const;
    /// The instance that actually implements this class
    center_pimpl_ptr_t m_pimpl_;
};

/** @brief Compares two Center instances for equality.
 *
 *  Two Center instances are considered equal if they contain the
 *  same number of shells and if the i-th shell of @p lhs is equal to
 *  the i-th shell of @p rhs for all i in the range `[0, lhs.size())`.
 *
 *  @tparam T The type used to hold the Center's parameters.
 *
 *  @param[in] lhs The instance on the left side of the operator.
 *  @param[in] rhs The instance on the right side of the operator.
 *
 *  @return True if the the two instances are equal and false otherwise.
 *
 *  @throw std::bad_alloc if there is insufficient memory to make the PIMPL for
 *         one of the ShellView instances. Strong throw guarantee.
 */
template<typename T>
bool operator==(const Center<T>& lhs, const Center<T>& rhs) {
    using base_t = utilities::IndexableContainerBase<Center<T>>;
    return static_cast<const base_t&>(lhs) == static_cast<const base_t&>(rhs);
}

/** @brief Determines if two Center instances are different.
 *
 *  Two Center instances are considered equal if they contain the
 *  same number of shells and if the i-th shell of @p lhs is equal to
 *  the i-th shell of @p rhs for all i in the range `[0, lhs.size())`.
 *  Different is defined as not equal.
 *
 *  @tparam T The type used to hold the Center's parameters.
 *
 *  @param[in] lhs The instance on the left side of the operator.
 *  @param[in] rhs The instance on the right side of the operator.
 *
 *  @return False if the the two instances are equal and true otherwise.
 *
 *  @throw std::bad_alloc if there is insufficient memory to make the PIMPL for
 *         one of the ShellView instances. Strong throw guarantee.
 */
template<typename T>
bool operator!=(const Center<T>& lhs, const Center<T>& rhs) {
    return !(lhs == rhs);
}

extern template class Center<double>;
extern template class Center<float>;

} // namespace libchemist
