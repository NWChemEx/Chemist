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
    using size_type       = typename container_base::size_type;
    using pure_type       = ShellType;
    using am_type         = size_type;
    using param_set       = std::vector<T>;

    Center();
    Center(const Center<T>& rhs);
    Center(Center<T>&& rhs) noexcept;
    Center<T>& operator=(const Center<T>& rhs);
    Center<T>& operator=(Center<T>&& rhs) noexcept;
    Center(T x, T y, T z);
    Center(center_pimpl_ptr_t cpimpl, point_pimpl_ptr_t ppimpl) noexcept;
    ~Center() noexcept;

    void add_shell(pure_type pure, am_type l, param_set cs, param_set es);

private:
    friend container_base;
    size_type size_() const noexcept;
    reference at_(size_type i);
    const_reference at_(size_type i) const;
    /// The instance that actually implements this class
    center_pimpl_ptr_t m_pimpl_;
};

extern template class Center<double>;
extern template class Center<float>;

} // namespace libchemist
