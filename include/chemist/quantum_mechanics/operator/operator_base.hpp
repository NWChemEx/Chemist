#pragma once

namespace chemist::qm_operator {
class OperatorVisitor;

class OperatorBase {
public:
    using visitor_type = OperatorVisitor;

    using visitor_reference = visitor_type&;

    virtual ~OperatorBase() noexcept = default;

    void visit(visitor_reference visitor) const { return visit_(visitor); }

private:
    virtual void visit_(visitor_reference visitor) const = 0;
};

} // namespace chemist::qm_operator