/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <iomanip>
#include <iostream>

namespace chemist::topology {

/** @brief Describes a distance.
 *
 *  This class is basically just a strong-type. Conceivably it could be
 *  fleshed out a bit more to include units or something.
 *
 */
class Distance {
public:
    /// Type used to store the distance
    using value_type = double;

    /// Type of a read/write reference to the distance
    using reference_type = value_type&;

    /// Type of a read-only reference to the distance
    using const_reference = const value_type&;

    /** @brief Creates a new Distance instance with the provided value.
     *
     *  @param[in] r The value (in bohr) of the instance.
     *
     *  @throw None No throw guarantee.
     */
    explicit Distance(value_type r = 0.0) : m_distance_(r) {}

    /** @brief Returns the value of the distance in bohr.
     *
     *  @return The value of this distance in a read/write format
     *
     *  @throw None No throw guarantee.
     */
    reference_type value() noexcept { return m_distance_; }

    /** @brief Returns the value of the distance, in bohr.
     *
     *  @return The value of this instance, in a read-only format.
     *
     *  @throw None No throw guarantee.
     */
    const_reference value() const noexcept { return m_distance_; }

private:
    /// The actual value of the distance.
    value_type m_distance_;
};

/** @brief Compares two Distance instances for equality.
 *
 *  @relates Distance
 *
 *  Two Distance instances are equal if the values they wrap compare equal. This
 *  comparison is done at a binary level thus all bits must be the same, i.e.
 *  Distances of 1.230000 and 1.230001 will not compare equal.
 *
 *  @param[in] lhs The Distance instance on the left of the equality operator.
 *  @param[in] rhs The Distance instance on the right of the equality operator.
 *
 *  @return True if @p lhs compares equal to @p rhs and false otherwise.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator==(const Distance& lhs, const Distance& rhs) noexcept {
    return lhs.value() == rhs.value();
}

/** @brief Determines if two Distance instances are different.
 *
 *  @relates Distance
 *
 *  Two Distance instances are equal if the values they wrap compare equal. This
 *  comparison is done at a binary level thus all bits must be the same, i.e.
 *  Distances of 1.230000 and 1.230001 will not compare equal.
 *
 *  @param[in] lhs The Distance instance on the left of the inequality operator.
 *  @param[in] rhs The Distance instance on the right of the inequality
 *                 operator.
 *
 *  @return False if @p lhs compares equal to @p rhs and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator!=(const Distance& lhs, const Distance& rhs) noexcept {
    return !(lhs == rhs);
}

/** @brief Prints a Distance instance to the provided output stream.
 *
 *  This function allows Distance instances to be printed. The string
 *  representation strives to maintain full precision (there will be 16 decimal
 *  points for a double). Also included in the representation are the units.
 *
 *  @param[in,out] os The output stream we are printing the Distance to. After
 *                    this call the internal buffer of @p os will contain the
 *                    string representation of @p d.
 *  @param[in] d The Distance instance we are printing.
 *
 *  @return To support operator chaining this function also returns @p os by
 *          reference (after @p d has been added to it).
 *
 *  @throw None The function is no throw guarantee; however, errors my occur
 *              when adding @p d to @p os. If an error occurs the bad bit of
 *              @p os will be set.
 */
inline std::ostream& operator<<(std::ostream& os, const Distance& d) {
    return os << std::fixed << std::setprecision(16) << d.value() << " (a.u.)";
}

} // namespace chemist::topology
