#pragma once
#include "libchemist/potentials/electrostatic.hpp"
#include <cstddef>
#include <pluginplay/hasher.hpp>
#include <typeindex>

namespace libchemist {

/** @brief An abstract base class for Operator types
 *
 *  This class provides the basic API specifications for operator types to
 *  satisfy `Any` type requirements and to allow for their identification and
 *  manipulation in a type-erased manner.
 *
 *  All Operator types are to be derived from this class.
 */
class OperatorBase {
public:
    /// Hash function
    inline void hash(pluginplay::Hasher& h) const { hash_impl(h); }

    /// Polymorphic comparison of this Operator instance with another
    inline bool is_equal(const OperatorBase& other) const noexcept;

    /** @brief Non-polymorphic equality comparison of Operator instances
     *
     *  @param[in] other Operator instance to compare
     *  @return    true (stateless base class)
     *
     *  @throw None No throw guarantee.
     */
    inline bool operator==(const OperatorBase& other) const { return true; }

    /** @brief Non-polymorphic inequality comparison of Operator instances
     *
     *  @param[in] other Operator instance to compare
     *  @return    false (stateless base class)
     *
     *  @throw None No throw guarantee.
     */
    inline bool operator!=(const OperatorBase& other) const { return false; }

    /// Polymorphic defaulted no-throw dtor
    virtual ~OperatorBase() noexcept = default;

protected:
    /// Defaulted default noexcept ctor
    OperatorBase() noexcept = default;
    /// Defaulted copy ctor
    OperatorBase(const OperatorBase&) noexcept = default;
    /// Defaulted move ctor
    OperatorBase(OperatorBase&&) noexcept = default;

    /// Derived implementation of Hash function.
    virtual void hash_impl(pluginplay::Hasher& h) const = 0;
    /// Derived implementation of comparison function.
    virtual bool is_equal_impl(const OperatorBase&) const noexcept = 0;
};

inline bool OperatorBase::is_equal(const OperatorBase& other) const noexcept {
    return is_equal_impl(other) and other.is_equal_impl(*this);
}

} // namespace libchemist
