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
#include <chemist/basis_set/basis_set.hpp>

namespace chemist {

/// PIMPL forward declaration
class IntegralFactoryPIMPL;

class IntegralFactory {
public:
    /// Type of this instance
    using pimpl_t = IntegralFactoryPIMPL;

    /// Type of the pimpl
    using pimpl_ptr_t = std::unique_ptr<pimpl_t>;

    /// Type of the shell indices
    using indices_t = std::vector<std::size_t>;

    /// Type returned by compute
    using buffer_t = std::vector<const double*>;

    /** @brief Creates a new IntegralFactory with default constructed members.
     *
     *  @throws std::bad_alloc if there is an error while allocating the PIMPL.
     *                         Strong throw guarantee.
     */
    IntegralFactory();

    /** @brief Creates a new IntegralFactory with the provided implementation.
     *
     */
    explicit IntegralFactory(pimpl_ptr_t p) noexcept;

    /** @brief Makes a new IntegralFactory by deep copying another one.
     *
     *  The copy ctor makes a member-wise deep copy of another IntegralFactory
     *  instance. Note that if the other IntegralFactory is in a null state
     *  (i.e. has no PIMPL) the resulting instance will also be in a null state.
     *
     *  @param[in] other The instance to deep copy.
     *
     *  @throw std::bad_alloc if an error arises while allocating the new state.
     *                        Strong throw guarantee.
     */
    IntegralFactory(const IntegralFactory& other);

    /** @brief Takes ownership of another IntegralFactory's state.
     *
     *  The move ctor will transfer the state of @p other into the newly created
     *  IntegralFactory instance. After the move @p other will be in a null
     *  state (i.e. have no PIMPL). N.B. if @p other was in a null state to
     *  begin with the newly created IntegralFactory instance will also be in a
     *  null state.
     *
     *  @param[in,out] other The instance whose state is being taken. After the
     *                       construction @p other will be in a null state.
     *
     *  @throws none No throw guarantee.
     */
    IntegralFactory(IntegralFactory&& other);

    /// Default destructor, voids all references to member data
    ~IntegralFactory() noexcept;

    /** @brief Overwrites this system's state with a deep copy of @p rhs's
     *         state.
     *
     *  @param[in] rhs The IntegralFactory whose state will be deep copied.
     *
     *  @return The current IntegralFactory after setting its state to a deep
     *          copy of @p rhs's state.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the state
     *                        strong throw guarantee.
     */
    IntegralFactory& operator=(const IntegralFactory& rhs);

    /** @brief Overwrites this system's state with @p rhs's state.
     *
     *  @param[in,out] rhs The system whose state is being taken. After this
     *                     operation @p rhs will be in a null state.
     *
     *  @return The current IntegralFactory after it takes ownership of @p rhs's
     *          state.
     *
     *  @throw None No throw gurantee.
     */
    IntegralFactory& operator=(IntegralFactory&& rhs);

    /** @brief Determines if two IntegralFactory instances are the same.
     *
     *  @param[in] rhs The IntegralFactory on the right of the equality
     * operator.
     *
     *  @return True if this and @p rhs are the same and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const IntegralFactory& rhs) const noexcept;

    /** @brief Computes the integral values for a given set of shell indices
     *
     *  @return A vector of buffer points where the values of the integral can
     *          be found.
     *
     *  @throw std::runtime_error if the PIMPL isn't set.
     */
    const buffer_t& compute(const indices_t& indices);

private:
    /** @brief Ensures that the PIMPL exists, throwing if it doesn't.
     *
     *  @throw std::runtime_error if the PIMPL isn't set.
     */
    void assert_pimpl_() const;

    /// The object actually implementing this class
    pimpl_ptr_t m_pimpl_;
};

/** @brief Determines if two IntegralFactory instances are different.
 *
 *  @relates IntegralFactory
 *
 *  Two IntegralFactory instances are equal if all of their members compare
 *  equal.
 *
 *  @param[in] lhs The IntegralFactory on the left of the inequality operator.
 *  @param[in] rhs The IntegralFactory on the right of the inequality operator.
 *
 *  @return True if @p lhs and @p rhs are different and false otherwise.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator!=(const IntegralFactory& lhs,
                       const IntegralFactory& rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace chemist