#pragma once
#include <memory>

namespace chemist::qm_operator {
class OperatorVisitor;

class OperatorBase {
public:
    using visitor_type = OperatorVisitor;

    using visitor_reference = visitor_type&;

    using base_pointer = std::unique_ptr<OperatorBase>;

    using base_reference = OperatorBase&;

    using const_base_reference = const OperatorBase&;

    virtual ~OperatorBase() noexcept = default;

    base_pointer clone() const { return clone_(); }

    void visit(visitor_reference visitor) { return visit_(visitor); }

    void visit(visitor_reference visitor) const { return visit_(visitor); }

    bool are_equal(const_base_reference other) const noexcept {
        return are_equal_(other);
    }

protected:
    OperatorBase() noexcept           = default;
    OperatorBase(const OperatorBase&) = default;

    /// Deleted to avoid slicing
    ///@{
    OperatorBase(OperatorBase&&)                 = delete;
    OperatorBase& operator=(const OperatorBase&) = delete;
    OperatorBase& operator=(OperatorBase&&)      = delete;
    ///@}

    /// Derived class should override to be consistent with clone()
    virtual base_pointer clone_() const = 0;

    /// Derived class should override to be consist with visit()/visit() const
    ///@{
    virtual void visit_(visitor_reference visitor)       = 0;
    virtual void visit_(visitor_reference visitor) const = 0;
    ///@}

    virtual bool are_equal(const_base_reference other) const noexcept = 0;
};

} // namespace chemist::qm_operator