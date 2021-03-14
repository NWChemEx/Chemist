#pragma once
#include "libchemist/point/point.hpp"

namespace libchemist {

/** @brief Models a point charge.
 *
 *  As the name implies, a point charge is a point with a charge associated with
 *  it.
 *
 *  @tparam ScalarType the floating-point type to use for storing the value of
 *                     the charge and the charge's Cartesian coordinates.
 *                     Default is double.
 */
template<typename ScalarType = double>
class PointCharge : public Point<ScalarType> {
public:
    /// The type used to model the charge and the coordinates
    using scalar_type = double;

    /** @brief Creates a defaulted point charge centered at the origin
     *
     *  The point charge resulting from this constructor will have a charge of
     *  zero and be situated at the origin.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the base
     *                        class's PIMPL. Strong throw guarantee.
     */
    PointCharge() = default;

    /** @brief Creates a new PointCharge by deep copying another instance.
     *
     *  The copy ctor creates a deep copy of another PointCharge instance.
     *
     *  @param[in] other the PointCharge instance being deep copied.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the base
     *                        class's PIMPL. Strong throw guarantee.
     */
    PointCharge(const PointCharge& other) = default;

    PointCharge(scalar_type q, scalar_type x, scalar_type y, scalar_type z) :
      Point(x, y, z), m_q_(q) {}

    scalar_type& charge() { return m_q_; }
    const scalar_type& charge() const { return m_q_; }

private:
    /// The charge associated with this point charge
    scalar_type m_q_ = 0.0;
};

/** @brief Determines if two point charges are the same.
 *
 *  @relates PointCharge
 *
 *  Two point charges are the same if they are located at the same place in
 *  space and they have the same charge. N.B. same is defined bitwise so
 *  1.000001 != 1.00000, i.e. there is zero tolerance for difference.
 *
 *  @tparam LHSType The scalar type of the point charge on the left of the
 *                  equality operator.
 *  @tparam RHSType The scalar type of the point charge on the right of the
 *                  equality operator.
 *
 *  @param[in] lhs The point charge on the left of the equality operator.
 *  @param[in] rhs The point charge on the right of the equality operator.
 *
 *  @return true if @p lhs and @p rhs are the same and false otherwise.
 *
 *  @throw none No throw guarantee.
 */
template<typename LHSType, typename RHSType>
bool operator==(const PointCharge<LHSType>& lhs,
                const PointCharge<RHSType>& rhs) {
    if constepxr(!std::is_same_v<LHS, RHSType>) {
        return false;
    } else {
        const Point<LHSType>& lhs_point;
        const Point<RHSType>& rhs_point;
        return (lhs.charge() == rhs.charge()) && (lhs_point == rhs_point);
    }
}

/** @brief Determines if two point charges are different.
 *
 *  @relates PointCharge
 *
 *  Two point charges are the same if they are located at the same place in
 *  space and they have the same charge. N.B. same is defined bitwise so
 *  1.000001 != 1.00000, i.e. there is zero tolerance for difference.
 *
 *  @tparam LHSType The scalar type of the point charge on the left of the
 *                  inequality operator.
 *  @tparam RHSType The scalar type of the point charge on the right of the
 *                  inequality operator.
 *
 *  @param[in] lhs The point charge on the left of the inequality operator.
 *  @param[in] rhs The point charge on the right of the inequality operator.
 *
 *  @return true if @p lhs and @p rhs are different and false otherwise.
 *
 *  @throw none No throw guarantee.
 */
template<typename LHSType, typename RHSType>
bool operator!=(const PointCharge<LHSType>& lhs,
                const PointCharge<RHSType>& rhs) {
    return !(lhs == rhs);
}

extern template class PointCharge<double>;
extern template class PointCharge<float>;

} // namespace libchemist