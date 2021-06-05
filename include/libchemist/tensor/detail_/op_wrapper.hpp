#pragma once
#include "libchemist/tensor/detail_/type_traits.hpp"
#include "libchemist/tensor/types.hpp"
#include <utility>

namespace libchemist::tensor::detail_ {

template<typename VariantType>
class OpWrapper {
private:
    template<typename TileType, typename PolicyType>
    using labeled_ta_tensor_t =
      labeled_tensor_t<ta_tensor_t<TileType, PolicyType>>;

public:
    using variant_type = VariantType;
    OpWrapper(variant_type v) : m_tensor_(std::move(v)) {}

    const auto& tensor() const { return m_tensor_; }
    auto& tensor() { return m_tensor_; }

    template<typename TileType, typename PolicyType>
    void assign_to_tensor(labeled_ta_tensor_t<TileType, PolicyType>& t) const;

private:
    variant_type m_tensor_;
};

template<typename VariantType>
template<typename TileType, typename PolicyType>
void OpWrapper<VariantType>::assign_to_tensor(
  labeled_ta_tensor_t<TileType, PolicyType>& t) const {
    auto l = [&](auto&& op) { t = op; };
    std::visit(l, m_tensor_);
}

} // namespace libchemist::tensor::detail_