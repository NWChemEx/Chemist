#pragma once

namespace libchemist {

class Identity : public detail_::OperatorImpl<Identity> {
private:
    using base_type = detail_::OperatorImpl<Identity>;

public:
    using base_type::detail_::OperatorImpl;
};

} // namespace libchemist
