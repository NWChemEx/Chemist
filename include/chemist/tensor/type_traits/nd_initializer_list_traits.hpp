#pragma once
#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace chemist::tensor {

/** @brief Traits class for an @p N -dimensional initializer list of type @p T
 *
 *  This traits class is primarily envisioned as being used with the
 *  TensorWrapper class in order to deduce the type of an initializer list for
 *  a rank @p N tensor. Unrolling the first few results:
 *
 *  @code
 *  using rank0_il = T;
 *  using rank1_il = std::initializer_list<T>;
 *  using rank2_il = std::initializer_list<std::initializer_list<T>>;
 *  @endcode
 *
 *  Bascially a rank @p N tensor requires @p N nested initializer lists.
 *
 *  This class is the primary template and works out the initializer list when
 *  @p N is not 0. For @p N not equal to 0, the type of an initializer list for
 *  a rank @p N tensor is an initializer_list of rank @p N - 1
 *  initializer_lists. The type of the initializer list for a rank @p N tensor
 *  is accessible via the member type `type`.
 *
 *  @tparam T The scalar type in the tensor. This will be the type the most
 *            nested initializer_list is filled with.
 *  @tparam N An unsigned integer specifying the rank of the tensor the
 *            initializer_list is for.
 */
template<typename T, std::size_t N>
struct NDInitializerListTraits : NDInitializerListTraits<T, N - 1> {
    /// Typedef of the class this class inherits from
    using base_type = NDInitializerListTraits<T, N - 1>;

    /// The type of an initializer_list for a rank @p N tensor with scalars @p T
    using type = std::initializer_list<typename base_type::type>;
};

/** @brief Specialization of NDInitializerListTraits for a rank 0 tensor.
 *
 *  This class stops the recursion of the primary template by specializing the
 *  base case (*i.e.*, when the rank is 0). This class contains a member `type`
 *  which is a typedef of @p T. Equivalently you can think of a single scalar as
 *  being the initializer list for a rank 0 tensor.
 *
 *  @tparam T The type of the scalar in the analogous tensor.
 */
template<typename T>
struct NDInitializerListTraits<T, 0ul> {
    /// Type of an initializer list for a rank 0 tensor with scalars @p T
    using type = T;
};

/** @brief Convenience typedef of the initializer_list for a rank @p N tensor of
 *         scalars of type @p T.
 *
 *  This typedef is a typedef of the `type` member of the `NDInitializer<T, N>`
 *  type for coding convenience.
 *
 *  @tparam T The type of the scalars in the tensor.
 *  @tparam N The rank of the tensor we are generating the initializer_list for.
 */
template<typename T, std::size_t N>
using n_d_initializer_list_t = typename NDInitializerListTraits<T, N>::type;

/** @brief Utility function to figure out the shape of the tensor which will
 *         result from @p il.
 *
 *  @relates NDInitializerListTraits
 *
 *  @tparam T The scalar type in the initializer_list.
 *  @tparam N The rank of the tensor being initialized by @p il.
 *
 *  @param[in] il The initializer list whose shape is wanted.
 *
 *  @return a std::vector of length @p N such that the i-th element (0 based) is
 *          the extent of the i-th mode of the resulting tensor.
 */
template<typename T, std::size_t N>
auto il2extents(const n_d_initializer_list_t<T, N>& il) {
    using size_type    = typename std::initializer_list<T>::size_type;
    using extents_type = std::vector<size_type>;

    if constexpr(N == 0) {
        return extents_type{};
    } else {
        // Handle empty ils
        if(il.begin() == il.end()) return extents_type(N, 0);

        const auto& i0 = *(il.begin()); // 0-th element

        auto sub_extents = il2extents<T, N - 1>(i0);
        for(const auto& x : il)
            if(sub_extents != il2extents<T, N - 1>(x))
                throw std::runtime_error(
                  "Initializer list must be rectangular");

        sub_extents.insert(sub_extents.begin(), il.size());
        return sub_extents;
    }
}

} // namespace chemist::tensor
