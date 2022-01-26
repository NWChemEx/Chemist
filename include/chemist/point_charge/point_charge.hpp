#pragma once
#include "chemist/point/point.hpp"

namespace chemist {

/** @brief Models a point charge.
 *
 *  As the name implies, a point charge is a point with a charge associated with
 *  it. In quantum chemistry point charges are typically used to model nuclei
 *  and distant electrostatic interactions.
 *
 *  @tparam ScalarType the floating-point type to use for storing the value of
 *                     the charge and the charge's Cartesian coordinates.
 *                     Default is double.
 */
template<typename ScalarType = double>
class PointCharge : public Point<ScalarType> {
private:
    using base_type = Point<ScalarType>;

public:
    /// The type used to model the charge and the coordinates
    using scalar_type = ScalarType;

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

    /** @brief Creates a PointCharge by taking ownership of another instance's
     *         state.
     *
     *  @param[in,out] other The PointCharge whose state is being taken. After
     *                       this operation @p other is in a valid, but
     *                       otherwise undefined state.
     *
     *  @throw none No throw guarantee.
     */
    PointCharge(PointCharge&& other) noexcept = default;

    /** @brief Creates a new PointCharge instance with the specified state.
     *
     *  @param[in] q The charge of the PointCharge (in a.u.)
     *  @param[in] x The x coordinate of the PointCharge (in a.u.)
     *  @param[in] y The y coordinate of the PointCharge (in a.u.)
     *  @param[in] z The z coordinate of the PointCharge (in a.u.)
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        base class's PIMPL. Strong throw guarantee.
     */
    PointCharge(scalar_type q, scalar_type x, scalar_type y, scalar_type z);

    /** @brief Overwrites the current state with a deep copy of another point
     *         charge's state.
     *
     *  @param[in] rhs The instance whose state is being deep copied.
     *
     *  @return This instance with its state set to a deep copy of @p rhs's
     *          state.
     *
     *  @throw none No throw guarantee.
     */
    PointCharge& operator=(const PointCharge& rhs) noexcept = default;

    /** @brief Overwrites the current state by taking ownwership of another
     *         point charge's state.
     *
     *  @param[in,out] rhs The instance whose state is being transferred into
     *                     this instance. After this operation @p rhs is in a
     *                     valid, but otherwise undefined state.
     *
     *  @return This instance now owning @p rhs's state.
     *
     *  @throw none No throw guarantee.
     */
    PointCharge& operator=(PointCharge&& rhs) noexcept = default;

    /** @brief Returns the charge of the point charge in a read/write format.
     *
     *  This function can be used to both inspect and modify the charge of the
     *  point charge.
     *
     *  @return A read/write reference to this PointCharge's charge.
     *
     *  @throw None No throw gurantee.
     */
    scalar_type& charge() noexcept { return m_q_; }

    /** @brief Returns the charge in a read-only state.
     *
     *  @return A read-only reference to the PointCharge's charge.
     *
     *  @throw None No throw guarantee.
     */
    const scalar_type& charge() const noexcept { return m_q_; }

    /** @brief Serializes the point charge.
     *
     *  @param[in,out] ar The archive instance being used for serialization.
     *                    After this call @p ar will contain this instance's
     *                    serialized state.
     */
    template<typename Archive>
    void save(Archive& ar) const;

    /** @brief Deserializes the point charge.
     *
     *  @param[in,out] ar The archive instance which contains the serialized
     *                    state for this instance. After this call @p ar will
     *                    no longer contain this instance's serialized state.
     */
    template<typename Archive>
    void load(Archive& ar);

    /** @brief Computes a hash of the PointCharge.
     *
     *  @param[in,out] h The object used to hash the state. After this call @p h
     *                   will have been modified to include a hash of this
     *                   object's state.
     */
    void hash(detail_::Hasher& h) const;

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
    if constexpr(!std::is_same_v<LHSType, RHSType>) {
        return false;
    } else {
        const Point<LHSType>& lhs_point = lhs;
        const Point<RHSType>& rhs_point = rhs;
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

// ----------------------- Implementations -------------------------------------

template<typename T>
PointCharge<T>::PointCharge(scalar_type q, scalar_type x, scalar_type y,
                            scalar_type z) :
  Point<T>(x, y, z), m_q_(q) {}

template<typename T>
template<typename Archive>
void PointCharge<T>::save(Archive& ar) const {
    base_type::save(ar);
    ar& m_q_;
}

template<typename T>
template<typename Archive>
void PointCharge<T>::load(Archive& ar) {
    base_type::load(ar);
    ar& m_q_;
}

template<typename T>
void PointCharge<T>::hash(detail_::Hasher& h) const {
    base_type::hash(h);
    h(m_q_);
}

extern template class PointCharge<double>;
extern template class PointCharge<float>;

} // namespace chemist
