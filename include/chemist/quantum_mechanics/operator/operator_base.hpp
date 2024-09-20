#pragma once
#include <memory>

namespace chemist::qm_operator {
class OperatorVisitor;

class OperatorBase {
public:
    using visitor_type = OperatorVisitor;

    using visitor_reference = visitor_type&;

    using base_pointer = std::unique_ptr<OperatorBase>;

    virtual ~OperatorBase() noexcept = default;

    base_pointer clone() const { return clone_(); }

    void visit(visitor_reference visitor) const { return visit_(visitor); }

protected:
    OperatorBase() noexcept           = default;
    OperatorBase(const OperatorBase&) = default;

    /// Deleted to avoid slicing
    ///@{
    OperatorBase(OperatorBase&&)                 = delete;
    OperatorBase& operator=(const OperatorBase&) = delete;
    OperatorBase& operator=(OperatorBase&&)      = delete;
    ///@}

    virtual void visit_(visitor_reference visitor) const = 0;

    virtual base_pointer clone_() const = 0;
};

} // namespace chemist::qm_operator