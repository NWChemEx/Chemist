#pragma once
#include "libchemist/tensor/detail_/labeled_tensor_wrapper.hpp"
#include "libchemist/tensor/detail_/type_traits.hpp"
#include <utilities/type_traits/variant/has_type.hpp>

namespace libchemist::tensor {

template<typename VariantType>
class TensorWrapper {
private:
    template<typename TensorType>
    using eif_has_type =
      utilities::type_traits::variant::enable_if_has_type_t<TensorType,
                                                            VariantType>;

    /// Variant type resulting from labeling the types in VariantType
    using labeled_variant_type = labeled_variant_t<VariantType>;

    using const_labeled_variant_type = const_labeled_variant_t<VariantType>;

    /** @brief Determines if a TA::DistArray type is a tensor-of-tensors
     *
     *  @tparam T Assumed to be a specialization of TA::DistArray.
     */
    template<typename T>
    static constexpr bool is_tot_v =
      TA::detail::is_tensor_of_tensor<typename T::value_type>::value;

public:
    /// Type of the variant this wrapper is templated on
    using variant_type = VariantType;

    /// String-like type used to annotate a tensor.
    using annotation_type = std::string;

    /// Type of a wrapper around a labeled tensor
    using labeled_tensor_type =
      detail_::LabeledTensorWrapper<labeled_variant_type>;

    using const_labeled_tensor_type =
      detail_::LabeledTensorWrapper<const_labeled_variant_type>;

    /** @brief Default CTor
     *
     *  TensorWrapper instances should always be wrapping an existing tensor. To
     *  help ensure that this is the case we have deleted the default ctor.
     */
    TensorWrapper() = delete;

    template<typename TensorType,
             typename = eif_has_type<std::decay_t<TensorType>>>
    explicit TensorWrapper(TensorType&& t) :
      m_tensor_(std::forward<TensorType>(t)) {}

    auto operator()(const annotation_type& annotation);
    auto operator()(const annotation_type& annotation) const;

    auto make_annotation(const annotation_type& letter = "i") const;

    auto rank() const;

    std::ostream& print(std::ostream& os) const;

private:
    /** @brief Determines if we're wrapping a Tensor-of-tensors or not.
     *
     *  Users of TensorWrapper should be using it in a manner that is agnostic
     *  of what it is wrapping. That's why this function is private. Internally,
     *  we have to diverge a little to treat ToTs vs. normal tensors. This
     *  function is used to determine which scenario we presently have.
     *
     *  @return True if we are wrapping a ToT and false otherwise.
     *  @throw None No throw guarantee.
     */
    bool is_tot_() const noexcept;
    auto outer_rank_() const noexcept;
    auto inner_rank_() const;

    variant_type m_tensor_;
};

template<typename VType>
std::ostream& operator<<(std::ostream& os, const TensorWrapper<VType>& t) {
    return t.print(os);
}

// ------------------------------- Implementations -----------------------------

#define TENSOR_WRAPPER TensorWrapper<VariantType>

template<typename VariantType>
auto TENSOR_WRAPPER::operator()(const annotation_type& annotation) {
    auto l = [=](auto&& arg) { return labeled_variant_type(arg(annotation)); };
    return labeled_tensor_type(std::visit(l, m_tensor_));
}

template<typename VariantType>
auto TENSOR_WRAPPER::operator()(const annotation_type& annotation) const {
    auto l = [=](auto&& arg) {
        return const_labeled_variant_type(arg(annotation));
    };
    return const_labeled_tensor_type(std::visit(l, m_tensor_));
}

template<typename VariantType>
auto TENSOR_WRAPPER::make_annotation(const annotation_type& letter) const {
    auto r          = rank();
    auto outer_rank = (is_tot_() ? outer_rank_() : r);
    annotation_type x;
    if(r == 0) return x;
    for(decltype(r) i = 0; i < r - 1; ++i) {
        x += letter + std::to_string(i);
        x += (i + 1 == outer_rank ? ";" : ",");
    }
    x += letter + std::to_string(r - 1);
    return x;
}

template<typename VariantType>
auto TENSOR_WRAPPER::rank() const {
    return outer_rank_() + inner_rank_();
}

template<typename VariantType>
std::ostream& TENSOR_WRAPPER::print(std::ostream& os) const {
    auto l = [&](auto&& arg) { os << arg; };
    std::visit(l, m_tensor_);
    return os;
}

template<typename VariantType>
bool TENSOR_WRAPPER::is_tot_() const noexcept {
    auto l = [](auto&& arg) {
        using clean_t = std::decay_t<decltype(arg)>;
        return is_tot_v<clean_t>;
    };
    return std::visit(l, m_tensor_);
}

template<typename VariantType>
auto TENSOR_WRAPPER::outer_rank_() const noexcept {
    auto l = [](auto&& arg) { return TA::rank(arg); };
    return std::visit(l, m_tensor_);
}

template<typename VariantType>
auto TENSOR_WRAPPER::inner_rank_() const {
    auto l = [](auto&& arg) {
        using clean_t = std::decay_t<decltype(arg)>;
        if constexpr(!is_tot_v<clean_t>)
            return 0;
        else {
            const auto& tile0 = arg.begin()->get();
            return tile0[0].range().rank();
        }
    };
    return std::visit(l, m_tensor_);
}

#undef TENSOR_WRAPPER

} // namespace libchemist::tensor