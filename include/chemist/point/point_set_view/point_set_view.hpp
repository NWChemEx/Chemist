#pragma once
#include <chemist/point/point_set.hpp>
#include <chemist/traits/point_traits.hpp>
#include <memory>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename PointSetType>
class PointSetViewPIMPL;
}

template<typename PointSetType>
class PointSetView
  : public utilities::IndexableContainerBase<PointSetView<PointSetType>> {
private:
    /// Type of *this
    using my_type = PointSetView<PointSetType>;

    /// Type this class derives from
    using base_type =
      utilities::IndexableContainerBase<PointSetView<PointSetType>>;

public:
    /// Traits associated with PointSetType
    using traits_type = ChemistClassTraits<PointSetType>;

    /// Traits associated with the Points in *this
    using point_traits_type = typename traits_type::point_traits;

    /// *this behaves like a reference to an object of this type
    using point_set_type = typename traits_type::value_type;

    /// *this behaves like this type (acts like point_set_type&)
    using point_set_reference = typename traits_type::reference;

    /// Type of a point stored in *this
    using value_type = typename point_traits_type::value_type;

    /// Type of a reference to a point in *this
    using reference = typename point_traits_type::reference;

    /// Type of a read-only reference
    using const_reference = typename point_traits_type::const_reference;

    /// Type of a PIMPL
    using pimpl_type = detail_::PointSetViewPIMPL<PointSetType>;

    /// Type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type used for indexing and offsets
    using typename base_type::size_type;

    // -------------------------------------------------------------------------
    // -- Ctors, Assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Makes a view of an empty PointSet object.
     *
     *  @throw None No throw guarantee
     */
    PointSetView() noexcept;

    explicit PointSetView(point_set_reference ps);

    explicit PointSetView(pimpl_pointer pimpl) noexcept;

    PointSetView(const PointSetView& other);

    my_type& operator=(const PointSetView& rhs);

    PointSetView(PointSetView&& other) noexcept;

    my_type& operator=(PointSetView&& other) noexcept;

    ~PointSetView() noexcept;

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    void swap(PointSetView& other) noexcept;

private:
    bool has_pimpl_() const noexcept;

    pimpl_pointer clone_pimpl_() const;

    /// Implements at/operator[] for the base class
    reference at_(size_type i);

    /// Implements at/operator[] for the base class
    const_reference at_(size_type i) const;

    /// Implements size for the base class
    size_type size_() const noexcept;

    /// The actual object holding the state of *this
    pimpl_pointer m_pimpl_;
};

extern template class PointSetView<PointSet<float>>;
extern template class PointSetView<const PointSet<float>>;
extern template class PointSetView<PointSet<double>>;
extern template class PointSetView<const PointSet<double>>;

} // namespace chemist
