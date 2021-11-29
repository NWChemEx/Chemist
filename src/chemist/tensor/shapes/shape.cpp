#include "chemist/tensor/shapes/shape.hpp"

namespace chemist::tensor::shape {

template<typename FieldType>
class ShapePIMPL {
private:
    using parent_type = Shape<FieldType>;

public:
    using extents_type = typename parent_type::extents_type;

    explicit ShapePIMPL(extents_type x = {}) : m_extents_(std::move(x)) {}

    virtual ~ShapePIMPL() noexcept = default;

private:
    extents_type m_extents_;
};

template class ShapePIMPL<field::Scalar>;
template class ShapePIMPL<field::Tensor>;

} // namespace chemist::tensor::shape
