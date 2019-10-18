#pragma once
#include "libchemist/basis_set/contracted_gaussian/contracted_gaussian_view.hpp"
#include "libchemist/point/point.hpp"
#include <utilities/containers/indexable_container_base.hpp>

namespace libchemist {
namespace detail_ {
template<typename T>
class ShellPIMPL;
}

template<typename T>
class Shell : public Point<T>,
              public utilities::IndexableContainerBase<Shell<T>> {
private:
    /// Type of this instance
    using my_type = Shell<T>;
    /// Type of the IndexableContainerBase base class
    using container_base = utilities::IndexableContainerBase<my_type>;
    /// Type of the PIMPL implementing the shell part of the class
    using pimpl_t = detail_::ShellPIMPL<T>;
    /// Type of a pointer to this class's PIMPL
    using pimpl_ptr_t = std::unique_ptr<pimpl_t>;
    /// Type of the PIMPL used by the Point<T> class
    using point_pimpl_t = detail_::PointPIMPL<T>;
    /// Type of a pointer to the Point's PIMPL
    using point_pimpl_ptr_t = std::unique_ptr<point_pimpl_t>;

public:
    /// For all intents and purposes the type of CGTOs comprising the shell
    using value_type = ContractedGaussian<T>;

    /// Type of a read/write reference
    using reference = ContractedGaussianView<T>;

    /// Type of a read-only reference
    using const_reference = ContractedGaussianView<const T>;

    /// Unsigned integral type used for indexing and offsets
    using size_type = typename container_base::size_type;

    Shell();
    Shell(const Shell<T>& rhs);
    Shell(Shell<T>&& rhs) noexcept;
    Shell<T>& operator=(const Shell<T>& rhs);
    Shell<T>& operator=(Shell<T>&& rhs) noexcept;
    Shell(bool pure, int l, std::vector<T> coefs, std::vector<T> exps, T x, T y,
          T z);
    Shell(pimpl_ptr_t my_pimpl, point_pimpl_ptr_t base_pimpl);

    /// Default, no-throw dtor
    ~Shell() noexcept;

    bool& pure();
    const bool& pure() const;

    int& l();
    const int& l() const;

private:
    /// Allows the IndexableContainerBase to access the implementations
    friend container_base;
    /// Implements size()
    size_type size_() const noexcept;
    /// Implements operator[]
    reference at_(size_type index);
    /// Implements operator[]const
    const_reference at_(size_type index) const;
    /// The instance that actually implements this class
    std::unique_ptr<pimpl_t> m_pimpl_;
}; // End class Shell

template<typename T>
bool operator==(const Shell<T>& lhs, const Shell<T>& rhs) {
    using base_t = utilities::IndexableContainerBase<Shell<T>>;
    return static_cast<const base_t&>(lhs) == static_cast<const base_t&>(rhs);
}

template<typename T>
bool operator!=(const Shell<T>& lhs, const Shell<T>& rhs) {
    return !(lhs == rhs);
}

extern template class Shell<double>;
extern template class Shell<float>;

} // namespace libchemist
