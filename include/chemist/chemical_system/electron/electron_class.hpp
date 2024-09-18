#pragma once

namespace chemist {

/** @brief Represents a single electron.
 *
 *  The Electron class is essentially a strong type used to create other types.
 *  Each object has no state and some minimal features.
 */
class Electron {
public:
    /** @brief Determines if *this is value equal to another object.
     *
     *  At present all Electron objects are value equal. This is because
     *  electrons are indistinguishable particles.
     *
     *  @param[in] rhs The object to compare to (at present not actually used).
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Electron&) const noexcept { return true; }

    /** @brief Is *this different from @p rhs?
     *
     *  Two Electron objects are different if they are not equal.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return False if *this and @p rhs are value equal and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const Electron& rhs) const noexcept {
        return !(*this == rhs);
    }
};

} // namespace chemist