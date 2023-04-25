#pragma once
#include <array>
#include <chemist/point/point.hpp>
#include <type_traits>

namespace chemist {

/** @brief Allows viewing data as if it was a PointType object.
 *
 *  TODO: This class should replace PointView, at which point we should drop the
 *        2 from the name.
 *
 *  @tparam PointType The type of Point<T> object this class is behaving like a
 *                    view of. Assumed to be either Point<T> or const Point<T>
 *                    (T being either float or double).
 */
template<typename PointType>
class PointView2 {
private:
    /// Type of a non-CV qualified Point<T> object
    using point_type = std::remove_cv_t<PointType>;

    /// Was @p PointType set to "const Point<T>"
    static constexpr bool is_const_v = !std::is_same_v<PointType, point_type>;

    template<typename c_opt, typename non_c_opt>
    using apply_const = std::conditional_t<is_const_v, c_opt, non_c_opt>;

public:
    /// Forward types from Point<T> class
    ///@{
    using coord_type            = typename point_type::coord_type;
    using const_coord_reference = typename point_type::const_coord_reference;
    using size_type             = typename point_type::size_type;
    ///@}

    using coord_reference = apply_const<const coord_type&, coord_type&>;

    PointView2(coord_reference x, coord_reference y, coord_reference z) :
      m_pr_{&x, &y, &z} {}

    PointView2(const PointView2& other) noexcept = default;
    PointView2(PointView2&& other) noexcept      = default;

    PointView2& operator=(const PointView2& rhs) noexcept = default;
    PointView2& operator=(PointView2&& rhs) noexcept      = default;

    coord_reference coord(size_type q) { return m_pr_[q]; }
    const_coord_reference coord(size_type q) const { return m_pr_[q]; }

    coord_reference x() noexcept { return *m_pr_[0]; }
    coord_reference y() noexcept { return *m_pr_[1]; }
    coord_reference z() noexcept { return *m_pr_[2]; }

    const_coord_reference x() const noexcept { return *m_pr_[0]; }
    const_coord_reference y() const noexcept { return *m_pr_[1]; }
    const_coord_reference z() const noexcept { return *m_pr_[2]; }

    coord_type magnitude() const noexcept {
        return std::sqrt(x() * x() + y() * y() + z() * z());
    }

    bool operator==(const Point<coord_type>& rhs) const noexcept {
        return std::tie(x(), y(), z()) == std::tie(rhs.x(), rhs.y(), rhs.z());
    }

    operator Point<coord_type>() const {
        return Point<coord_type>(x(), y(), z());
    }

private:
    using internal_pointer = apply_const<const coord_type*, coord_type*>;

    std::array<internal_pointer, 3> m_pr_;
};

} // namespace chemist
