#pragma once
#include "chemist/detail_/hashing.hpp"

namespace chemist {

/** @brief Class representing a single Electron
 *
 */
struct Electron {
    /** @brief Hashes an Electron instance.
     *
     *  @param[in,out] <anonymous> The hasher instance to use for hashing. This
     *                             function is a no-op and the hasher will
     *                             remain unchanged after the call.
     */
    void hash(detail_::Hasher&) const noexcept {}
};

/** @brief Compares two Electron instances.
 *
 *  @relates Electron
 *
 *  Electron are indistinguishable particles and thus always compare
 *  equal.
 *
 *  @param[in] <anonymous> The Electron to compare. Since Electron instances
 *                         have no state the provided instance is not
 *                         considered.
 *
 * @return True.
 *
 * @throw None No throw guarantee.
 */
inline bool operator==(const Electron&, const Electron&) { return true; }

/** @brief Determines if two Electron instances are different.
 *
 *  @relates Electron
 *
 *  Electron are indistinguishable particles so they can't be different.
 *
 *  @param[in] <anonymous> The Electron instances to compare. The instances
 *                         provided will not be considered since it has no
 *                         state.
 *
 *  @return False.
 *
 *  @throw None No throw guaranee.
 */
inline bool operator!=(const Electron&, const Electron&) { return false; }

} // namespace chemist
