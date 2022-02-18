#pragma once
#include "chemist/point_charge/point_charge.hpp"
#include <vector>

namespace chemist::potentials {

/** @brief Class that models an electrostatic potential.
 *
 *  At the moment I'm not exactly sure what I want from this class beyond it
 *  needs to be able to hold point charges. Realistically the nuclei also create
 *  an Electrostatic field so it may make sense to use the class for them too.
 * Then there's the possibility of going beyond point charges to dipoles, but
 * also possibly to full electronic densities...So consider the API of this
 * class subject to change without notice.
 *
 */
class Electrostatic {
public:
    /// Type used to store scalar components of the Electrostatic potential
    using scalar_type = double;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /// Type used to store point charges
    using charge_type = PointCharge<scalar_type>;

    /// Read/write reference to point charge
    using charge_reference = charge_type&;

    /// Read-only reference to a  point charge
    using const_charge_reference = const charge_type&;

    /** @brief Adds a point charge to the electrostatic potential.
     *
     *  This function is used to add a point charge to the electrostatic
     *  potential. No attempts are made to ensure that the the charge is unique
     *  or doesn't overlap with an already existing charge.
     *
     *  @param[in] q The charge to add to the potential.
     *
     *  @throw std::bad_alloc if there is insufficient memory to hold the new
     *                        charge. Strong throw guarantee.
     */
    void add_charge(charge_type q) { m_charges_.emplace_back(std::move(q)); }

    /** @brief Returns the i-th point charge in the potential in a modifiable
     *         state.
     *
     *   @param[in] i The index of the charge to retrieve.
     *
     *   @return The requested point charge in a modifiable state.
     *
     *   @throw std::out_of_range if @p i is not in the range [0, ncharges()).
     *                            Strong throw guarantee.
     */
    charge_reference charge(size_type i);

    /** @brief Returns the i-th point charge in a read-only state
     *
     *   @param[in] i The index of the charge to retrieve.
     *
     *   @return The requested point charge in a read-only state.
     *
     *   @throw std::out_of_range if @p i is not in the range [0, ncharges()).
     *                            Strong throw guarantee.
     */
    const_charge_reference charge(size_type i) const;

    /** @brief Returns the number of point charges in the electrostatic
     *         potential.
     *
     *  @return The number of point charges in the electrostatic potential.
     *
     *  @throw None no throw guarantee.
     */
    size_type ncharges() const noexcept { return m_charges_.size(); }

    /** @brief Determines if two electrostatic potentials are equal.
     *
     *  Two electrostatic potentials are equal if they contain the same set of
     *  point charges (and at the moment they must be in the same order). Note
     *  that same means that 1.00000 != 1.00001, i.e. floating-point comparisons
     *  are bitwise with no tolerance.
     *
     *  @return True if this Electrostatic potential is equal to @p rhs and
     *          false otherwise.
     *
     *  @throw None no throw guarantee.
     */
    bool operator==(const Electrostatic& rhs) const noexcept;

    /** @brief Serializes the electrostatic potential.
     *
     *  @param[in,out] ar The archive instance being used for serialization.
     *                    After this call @p ar will contain this instance's
     *                    serialized state.
     */
    template<typename Archive>
    void save(Archive& ar) const;

    /** @brief Deserializes the electrostatic potential
     *
     *  @param[in,out] ar The archive instance which contains the serialized
     *                    state for this instance. After this call @p ar will
     *                    no longer contain this instance's serialized state.
     */
    template<typename Archive>
    void load(Archive& ar);

    /** @brief Computes a hash of the electrostatic potential.
     *
     *  @param[in,out] h The object used to hash the state. After this call @p h
     *                   will have been modified to include a hash of this
     *                   object's state.
     */
    void hash(chemist::detail_::Hasher& h) const { h(m_charges_); }

private:
    /** @brief Checks that a charge index is in bounds
     *
     *   This function primarily serves to provide a more user-friendly error
     *   message than the normal std::vector::at error.
     *
     *   @param[in] i The index of the charge we are bounds checking.
     *
     *   @throw std::out_of_range if @p i is not in the range [0, ncharges()).
     *                            Strong throw guarantee.
     */
    void charge_bound_(size_type i) const;

    /// The point charges contributing to the electrostatic potential
    std::vector<charge_type> m_charges_;
};

/** @brief Determines if two electrostatic potentials are different.
 *
 *  Two electrostatic potentials are equal if they contain the same set of
 *  point charges (and at the moment they must be in the same order). Note
 *  that same means that 1.00000 != 1.00001, i.e. floating-point comparisons
 *  are bitwise with no tolerance.
 *
 *  @return False if this Electrostatic potential is equal to @p rhs and
 *          true otherwise.
 *
 *  @throw None no throw guarantee.
 */
inline bool operator!=(const Electrostatic& lhs, const Electrostatic& rhs) {
    return !(lhs == rhs);
}

// ------------------- Implementations -----------------------------------------

inline typename Electrostatic::charge_reference Electrostatic::charge(
  size_type i) {
    charge_bound_(i);
    return m_charges_[i];
}

inline typename Electrostatic::const_charge_reference Electrostatic::charge(
  size_type i) const {
    charge_bound_(i);
    return m_charges_[i];
}

inline bool Electrostatic::operator==(const Electrostatic& rhs) const noexcept {
    return std::tie(m_charges_) == std::tie(rhs.m_charges_);
}

template<typename Archive>
void Electrostatic::save(Archive& ar) const {
    ar& m_charges_;
}

template<typename Archive>
void Electrostatic::load(Archive& ar) {
    ar& m_charges_;
}

inline void Electrostatic::charge_bound_(size_type i) const {
    if(i < ncharges()) return;
    std::string msg = "charge index " + std::to_string(i) +
                      " is not in range [0, " + std::to_string(ncharges()) +
                      ")";
    throw std::out_of_range(msg);
}

} // namespace chemist::potentials
