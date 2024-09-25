#pragma once
#include <memory>

namespace chemist::qm_operator {
class OperatorVisitor;

/** @brief Class defining the common API for all operators.
 *
 *  OperatorBase is charged with providing a common API for all operators and
 *  in doing so without exposing templates (to facilitate use from Python). The
 *  OperatorBase class also is charged with code factorization that can be done
 *  without needing to know the template types. See detail_::OperatorImpl for
 *  code factorization that needs to know the template types.
 *
 *  See the design documentation for more details.
 */
class OperatorBase {
public:
    /// Base type user should derive visitor objects from.
    using visitor_type = OperatorVisitor;

    /// Type of a mutable reference to an object of type visitor_type
    using visitor_reference = visitor_type&;

    /// Type of a unique_ptr to an object of OperatorBase type
    using base_pointer = std::unique_ptr<OperatorBase>;

    /// Type of a mutable reference to an object of type OperatorBase
    using base_reference = OperatorBase&;

    /// Type of a read-only reference to an object of type OperatorBase
    using const_base_reference = const OperatorBase&;

    /// Defaulted no-throw no-op polymorphic dtor
    virtual ~OperatorBase() noexcept = default;

    /** @brief Makes a deep polymorphic copy of *this.
     *
     *  The clone method is used to create a deep copy of *this via the base
     *  class.
     *
     *  @return A deep copy of *this.
     *
     *  @throw std::bad_alloc if there is a problem performing the deep copy.
     *                        Strong throw guarantee.
     */
    base_pointer clone() const { return clone_(); }

    /** @brief Allows a user to interact with the derived class generically.
     *
     *  Users can derive visitors from the OperatorVisitor class in order to
     *  interact with the derived class generically. This method will take the
     *  user-provided visitor and call the `run` method with *this passed as an
     *  instance of the derived class. *this will be passed in a mutable manner.
     *
     *  @param[in] visitor The user-provided object to forward to the derived
     *                     class. @p visitor will receive *this in a mutable
     *                     state.
     *
     *  @throw ??? This method will throw if calling the visitor throws and
     *             provides the same throw guarantee as the visitor.
     */
    void visit(visitor_reference visitor) { return visit_(visitor); }

    /** @brief Allows a user to interact with the derived class generically.
     *
     *  This overload is used for downcasting *this and providing it to
     *  @p visitor in a read-only manner. See the documentation for the non-
     *  const version of visit for more details.
     *
     *  @param[in] visitor The OperatorVisitor the derived class will call.
     *
     *  @throw ??? Throws if @p visitor throws. Same throw guarantee.
     */
    void visit(visitor_reference visitor) const { return visit_(visitor); }

    /** @brief Determines if *this and @p other are polymorphically value equal.
     *
     *  Two OperatorBase objects, x and y, are polymorphically value equal if
     *  the most derived class of x is the same as that of y and if x and y
     *  are value equal when compared as instances of the derived class.
     *
     *  This method dispatches to are_equal_ and developers of derived classes
     *  are responsible for overriding are_equal_ appropriately.
     *
     *  @param[in] other The object to compare to *this.
     *
     *  @return True if *this and @p other are polymorphically value equal and
     *          false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal(const_base_reference other) const noexcept {
        return are_equal_(other) && other.are_equal_(*this);
    }

    /** @brief Determines if *this is polymorphically different from @p other.
     *
     *  The operator class hierarchy defines "polymorphically different" as
     *  being not "polymorphically value equal". See the documentation for
     *  are_equal for the definition of polymorphically value equal.
     *
     *  @param[in] other The operator to compare against.
     *
     *  @return False if *this and @p other are polymorphically value equal and
     *          true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_different(const_base_reference other) const noexcept {
        return !are_equal(other);
    }

protected:
    /// No-op, nothrow default ctor, protected as only derived classes need it
    OperatorBase() noexcept = default;

    /// No-op, nothrow copy ctor, protected so it is only used by derived class
    OperatorBase(const OperatorBase&) = default;

    /// No-op, nothrow move ctor protected so it is only used by derived class
    OperatorBase(OperatorBase&&) noexcept = default;

    /// No-op, no throw copy assignment to be used by derived class only!
    OperatorBase& operator=(const OperatorBase&) = default;

    /// No-op, no throw move assignment to be used by derived class only!
    OperatorBase& operator=(OperatorBase&&) = default;

    /// Derived class should override to be consistent with clone()
    virtual base_pointer clone_() const = 0;

    /// Derived class should override to be consist with visit()/visit() const
    ///@{
    virtual void visit_(visitor_reference visitor)       = 0;
    virtual void visit_(visitor_reference visitor) const = 0;
    ///@}

    /// Derived class should override to be consistent with are_equal
    virtual bool are_equal_(const_base_reference other) const noexcept = 0;
};

} // namespace chemist::qm_operator