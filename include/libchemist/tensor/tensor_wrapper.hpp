#pragma once
#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/detail_/labeled_tensor_wrapper.hpp"
#include "libchemist/tensor/type_traits/type_traits.hpp"
#include <sde/hasher.hpp>
#include <utilities/type_traits/variant/add_const.hpp>
#include <utilities/type_traits/variant/decay.hpp>
#include <utilities/type_traits/variant/has_type.hpp>

namespace libchemist::tensor {

/** @brief Fundamental type for wrapping tensors.
 *
 *  The TensorWrapper class is designed to provide a single type which can hold
 *  any of an array of tensor types. The types that it can hold are determined
 *  by the types in @p VariantType. Algorithms which take TensorWrapper
 *  instances, should do so only for a specific TensorWrapper specialization. In
 *  turn the algorithm signals to the user that the algorithm will work with all
 *  of the tensor types that the TensorWrapper specialization can hold. In turn,
 *  it becomes possible to write non-templated algorithms, which work with a
 *  variety of tensor types.
 *
 *  @tparam VariantType the type of the std::variant holding all possible tensor
 *                      types the wrapper may hold.
 */
template<typename VariantType>
class TensorWrapper {
private:
    /// Type of this instance
    using my_type = TensorWrapper<VariantType>;

    /// Type of the variant with all unqualified types in it
    using clean_variant = utilities::type_traits::variant::decay_t<VariantType>;

    /** @brief Used to enable a function if @p T is in @p clean_variant.
     *
     *  This type allows the TensorWrapper class to selectively enable overloads
     *  of functions by using SFINAE. More specifically the function will
     *  participate in overload resolution if @p T is one of the types in
     *  `clean_variant`.
     *
     *  @tparam T The type of the tensor which must appear in clean_variant. T
     *            is expected to be an unqualfied type.
     */
    template<typename T>
    using eif_has_type =
      utilities::type_traits::variant::enable_if_has_type_t<T, clean_variant>;

public:
    /// Type of the variant this wrapper is templated on
    using variant_type = VariantType;

    using const_variant_type =
      utilities::type_traits::variant::add_const_t<variant_type>;

    /// Type of a wrapper around a labeled tensor
    using labeled_tensor_type = detail_::LabeledTensorWrapper<my_type>;

    /// Type of a wrapper around a read-only labeled tensor
    using const_labeled_tensor_type =
      detail_::LabeledTensorWrapper<const my_type>;

    /// String-like type used to annotate a tensor.
    using annotation_type = typename labeled_tensor_type::annotation_type;

    /** @brief Default CTor
     *
     *  TensorWrapper instances should always be wrapping an existing tensor. To
     *  help ensure that this is the case we have deleted the default ctor.
     */
    TensorWrapper() = delete;

    /** @brief Wrapping CTor
     *
     *  This constructor creates a new TensorWrapper instance which wraps the
     *  provided Tensor. After this call the TensorWrapper will either contain a
     *  copy of the provided tensor or it will have taken ownership of it
     *  depending on whether or not the tensor was moved into the TensorWrapper.
     *
     *  @tparam TensorType The type of the tensor we are wrapping. TensorType
     *                      must be one of the types in @p VariantType.
     *  @tparam <anonymous> A type used to disable this ctor via SFINAE if
     *                      @p TensorType is not present in @p VariantType.
     */
    template<typename TensorType,
             typename = eif_has_type<std::decay_t<TensorType>>>
    explicit TensorWrapper(TensorType&& t) :
      m_tensor_(std::forward<TensorType>(t)) {}

    /** @brief Annotates the modes of the wrapped index with the provided
     *         labels.
     *
     *  The domain-specific language of tensor operations is written in terms of
     *  Einstein notation. This requires us to assign dummy indices to each mode
     *  of the tensor. This function pairs @p annotation with the wrapped tensor
     *  to provide a labeled tensor. The resulting labeled tensor can be used in
     *  tensor expressions.
     *
     *  @param[in] annotation The dummy indices we are annotating the underlying
     *                        tensor with.
     *
     *  @return A labeled read/write tensor.
     */
    auto operator()(const annotation_type& annotation);

    /** @brief Annotates the modes of the wrapped index with the provided
     *         labels.
     *
     *  The domain-specific language of tensor operations is written in terms of
     *  Einstein notation. This requires us to assign dummy indices to each mode
     *  of the tensor. This function pairs @p annotation with the wrapped tensor
     *  to provide a labeled tensor. The resulting labeled tensor can be used in
     *  tensor expressions.
     *
     *  @param[in] annotation The dummy indices we are annotating the underlying
     *                        tensor with.
     *
     *  @return A labeled read-only tensor.
     */
    auto operator()(const annotation_type& annotation) const;

    /** @brief Creates an annotation suitable for the wrapped tensor.
     *
     *  For a rank @f$r@f$ tensor this function will create a string containing
     *  @f$r@f$ indices. The @f$i@f$-th 0-based mode's index will be the result
     *  of concatentating @p letter with @f$i@f$. For example, using the default
     *  value of @p letter this function will generate the annotation
     *  `"i0,i1,i2"` for a rank 3 tensor. If the wrapped tensor is a
     *  tensor-of-tensors, this function will insert a semicolon where
     *  appropriate.
     *
     *  This function is meant to be used to perform generic operations on the
     *  tensors by string replacement. For example:
     *
     *  @code
     *  auto idx0 = t0.make_annotation();
     *  auto idx1 = t1.make_annotation("j");
     *  // To contract mode 1 of t0 with mode 3 of t1 we replace "j3" with "i1"
     *  auto new_idx1 = std::regex_replace(idx1, std::regex("j3"), "i1");
     *  // In practice we would work out the resulting annotation too
     *  auto mult_op = t0(idx0) * t1(idx1);
     *  @endcode
     *
     *  @param[in] letter The label part of the index. This is what will be
     *                    concatenated with the mode number to get the final
     *                    annotation for each mode. Default is `"i"`.
     *
     *  @return A string containing an annotation which is appropriate for the
     *          tensor.
     */
    auto make_annotation(const annotation_type& letter = "i") const;

    /** @brief Returns the number of modes in the wrapped tensor.
     *
     *  For a normal, non-hierarchical tensor the rank is simply the number of
     *  modes in the tensor. For a hierarchical tensor the rank is still the
     *  number of modes in the tensor, but it should be noted that this is the
     *  sum of the number of independent and dependent modes.
     *
     *  @return The number of modes in the tensor.
     */
    auto rank() const;

    /** @brief Used to get the wrapped tensor back.
     *
     *  This function should really only be called by the creator of the tensor
     *  instance as they are the only ones who know what type they wrapped. In
     *  practice it is also possible for a function to loop over the types in
     *  the variant to figure out which type is in the wrapper; however, needing
     *  to do this (versus going through the tensor-generic API of the
     *  TensorWrapper class) suggests that your function may be better off being
     *  specialized for a particular tensor type.
     *
     *  @tparam TensorType The cv-qualified type of the tensor to retrieve.
     *
     *  @return A read/write reference to the wrapped tensor.
     */
    template<typename TensorType>
    TensorType& get() {
        return std::get<TensorType>(m_tensor_);
    }

    /** @brief Used to get the wrapped tensor back.
     *
     *  This function should really only be called by the creator of the tensor
     *  instance as they are the only ones who know what type they wrapped. In
     *  practice it is also possible for a function to loop over the types in
     *  the variant to figure out which type is in the wrapper; however, needing
     *  to do this (versus going through the tensor-generic API of the
     *  TensorWrapper class) suggests that your function may be better off being
     *  specialized for a particular tensor type.
     *
     *  @tparam TensorType The cv-qualified type of the tensor to retrieve.
     *
     *  @return A read-only reference to the wrapped tensor.
     */
    template<typename TensorType>
    const TensorType& get() const {
        return std::get<TensorType>(m_tensor_);
    }

    /** @brief Adds a string representation of the wrapped tensor to the
     *         provided stream.
     *
     *  @param[in,out] os The stream we adding the string representation of the
     *                    tensor to. After the call @p os will contain the
     *                    string representation of the wrapped tensor instance.
     *
     *  @return This function returns @p os to facilitate operator chaining.
     */
    std::ostream& print(std::ostream& os) const;

    /** @brief Adds the hash of the wrapped tensor to the provided Hasher.
     *
     *  @param[in] h The hasher we are adding the wrapped tensor to.
     */
    void hash(sde::Hasher& h) const;

    /** @brief Determines if two TensorWrappers wrap identical tensors.
     *
     *  This comparison determines if the two wrapped tensors are identical
     *  elementwise.
     *
     *  @tparam RHSType the type of the variant used by @p rhs.
     *
     *  @param[in] rhs The wrapped tensor we are comparing to.
     *
     *  @return True if the wrapped tensor compares equal to @p rhs and false
     *          otherwise.
     */
    template<typename RHSType>
    bool operator==(const TensorWrapper<RHSType>& rhs) const;

protected:
    template<typename OtherType>
    friend class TensorWrapper;
    friend labeled_tensor_type;
    friend const_labeled_tensor_type;

    /** @brief Returns the wrapped variant.
     *
     *  This function is used by LabeledTensorWrapper to get the variant. In
     *  general users of the TensorWrapper class shouldn't be working with the
     *  variant, which is why the function is not part of the public API.
     *
     *  @return A modifiable reference to the wrapped variant.
     *
     *  @throw None No throw guarantee.
     */
    variant_type& variant() { return m_tensor_; }

    /** @brief Returns the wrapped variant.
     *
     *  This function is used by LabeledTensorWrapper to get the variant. In
     *  general users of the TensorWrapper class shouldn't be working with the
     *  variant, which is why the function is not part of the public API.
     *
     *  @return A read-only reference to the wrapped variant.
     *
     *  @throw None No throw guarantee.
     */
    const variant_type& variant() const { return m_tensor_; }

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

    /** @brief Returns the outer rank of the tensor.
     *
     *  For a normal non-hierarchical tensor all modes are "outer" modes. For a
     *  ToT the modes to the left of the semi-colon are outer modes.
     *
     *  @return The number of outer modes.
     *
     *  @throw No throw guarantee.
     */
    auto outer_rank_() const noexcept;

    /** @brief Returns the inner rank of the tensor.
     *
     *  For a normal non-hierarchical tensor there are zero inner modes. For a
     *  ToT the modes to the right of the semi-colon are inner modes.
     *
     *  @return The number of inner modes.
     */
    auto inner_rank_() const;

    /// The actual tensor stored in an std::variant
    variant_type m_tensor_;
};

/** @brief Prints the wrapped tensor to the provided output stream.
 *
 *  @relates TensorWrapper
 *
 *  This function is a convenience function for calling TensorWrapper::print so
 *  that the wrapped tensor can be printed in the usual C++ manner.
 *
 *  @param[in,out] os The stream we adding the string representation of the
 *                    tensor to. After the call @p os will contain the
 *                    string representation of the wrapped tensor instance.
 *  @param[in] t The tensor to print to @p os.
 *
 *  @return This function returns @p os to facilitate operator chaining.
 */
template<typename VType>
std::ostream& operator<<(std::ostream& os, const TensorWrapper<VType>& t) {
    return t.print(os);
}

/** @brief Determiens if the wrapped tensor instances are different.
 *
 *  @relates TensorWrapper
 *
 *  @tparam LHSType The type of the variant in the left tensor wrapper.
 *  @tparam RHSType The type of the variant in the right tensor wrapper.
 *
 *  @param[in] lhs The wrapped tensor on the left of the not equal operator.
 *  @param[in] rhs The wrapped tensor on the right of the not equal operator.
 *
 *  @return False if @p lhs is equal to @p rhs and true otherwise.
 */
template<typename LHSType, typename RHSType>
bool operator!=(const TensorWrapper<LHSType>& lhs,
                const TensorWrapper<RHSType>& rhs) {
    return !(lhs == rhs);
}

// ------------------------------- Implementations -----------------------------

#define TENSOR_WRAPPER TensorWrapper<VariantType>

template<typename VariantType>
auto TENSOR_WRAPPER::operator()(const annotation_type& annotation) {
    return labeled_tensor_type(annotation, *this);
}

template<typename VariantType>
auto TENSOR_WRAPPER::operator()(const annotation_type& annotation) const {
    return const_labeled_tensor_type(annotation, *this);
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
void TENSOR_WRAPPER::hash(sde::Hasher& h) const {
    auto l = [&](auto&& arg) { h(arg); };
    std::visit(l, m_tensor_);
}

template<typename VariantType>
template<typename RHSType>
bool TENSOR_WRAPPER::operator==(const TensorWrapper<RHSType>& rhs) const {
    auto l = [&](auto&& lhs) {
        auto m = [&](auto&& rhs) { return lhs == rhs; };
        return std::visit(m, rhs.m_tensor_);
    };
    return std::visit(l, m_tensor_);
}

template<typename VariantType>
bool TENSOR_WRAPPER::is_tot_() const noexcept {
    auto l = [](auto&& arg) {
        using clean_t = std::decay_t<decltype(arg)>;
        return TensorTraits<clean_t>::is_tot;
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
        using clean_t   = std::decay_t<decltype(arg)>;
        using size_type = decltype(std::declval<clean_t>().range().rank());
        constexpr bool is_tot = TensorTraits<clean_t>::is_tot;
        if constexpr(!is_tot)
            return size_type{0};
        else {
            const auto& tile0 = arg.begin()->get();
            return size_type{tile0[0].range().rank()};
        }
    };
    return std::visit(l, m_tensor_);
}

#undef TENSOR_WRAPPER

} // namespace libchemist::tensor
