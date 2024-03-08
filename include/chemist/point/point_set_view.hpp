#pragma once
#include <chemist/traits/point_traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {

template<typename PointSetType>
class PointSetView
  : public utilities::IndexableContainerBase<PointSetView<PointSetType>> {
private:
    /// Type of *this
    using my_type = PointSetView<PointSetType>;

    /// Type this class derives from
    using base_type =
      utilities::IndexableContainerBase<PointSetView<PointSetType>>;

    /// Traits associated with PointSetType
    using traits_type = ChemistClassTraits<PointSetType>;

    /// Traits associated with the Points in *this
    using point_traits_type = typename traits_type::point_traits;

public:
    /// Type *this is behaving like
    using point_set_type = typename traits_type::value_type;

    /// Type of an element in *this
    using value_type = typename point_traits_type::value_type;

    // -------------------------------------------------------------------------
    // -- Ctors, Assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Makes a view of an empty PointSet object.
     *
     *  @throw None No throw guarantee
     */
    PointSetView() noexcept;
};

} // namespace chemist
