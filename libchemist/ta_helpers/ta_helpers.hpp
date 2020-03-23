#pragma once
#include "libchemist/ta_helpers/detail_/reducer.hpp"
#include "libchemist/ta_helpers/get_block_idx.hpp"""
#include "libchemist/sparse_map/sparse_map.hpp"
#include <tiledarray.h>

namespace libchemist {

auto inline add_tiled_dimension(const TA::TiledRange& trange,
                                const TA::TiledRange1& tr1,
                                std::size_t mode = 0) {
    std::vector<TA::TiledRange1> tr1s;
    const auto new_rank = trange.rank() + 1;
    tr1s.reserve(new_rank);
    for(std::size_t i = 0; i < new_rank; ++i) {
        if(i < mode) tr1s.push_back(trange.dim(i));
        else if(i == mode)tr1s.push_back(tr1);
        else tr1s.push_back(trange.dim(i - 1));
    }
    return TA::TiledRange{tr1s.begin(), tr1s.end()};
};

//------------------------------------------------------------------------------
// Tensor Creation
//------------------------------------------------------------------------------

/** @brief Creates a new tensor by applying a function elementwise to an
 *         existing tensor.
 *
 *  This function is a convenience function for creating a new tensor whose
 *  initial elements can be determined solely based on the element values of
 *  another tensor.
 *
 * @tparam TileType The type of the tiles in @p input. Expected to satisfy TA's
 *                  concept of "Tile".
 * @tparam PolicyType The type of @p input's policy. Expected to be either
 *                    DensePolicy or SparsePolicy.
 * @tparam Op should be a type commensurate with that of a function having
 *            signature `element_type(element_type)` where `element_type` is the
 *            type of the elements in each tile.
 *
 * @param[in] input The tensor supplying the input values @p op will be called
 *                  with.
 * @param[in] op The function to apply to each element of @p input to generate
 *               the resulting tensor.
 * @return A new tensor having the same shape and distribution as @p input. The
 *         @f$i@f$-th element of the new tensor is the result of `op(input[i])`,
 *         where `input[i]` is the @f$i@f$-th element of @p input.
 */
template<typename TileType, typename PolicyType, typename Op>
auto apply_elementwise(const TA::DistArray<TileType, PolicyType>& input,
    Op&& op) {

  auto m = [op{std::forward<Op>(op)}](TileType& result_tile,
                                      const TileType& input_tile) {
    result_tile = input_tile.unary(op);
  };

  return TA::DistArray<TileType, PolicyType>(input, std::move(m));
}

template<typename TileType, typename PolicyType>


//------------------------------------------------------------------------------
// Element/Tile Retrieval
//------------------------------------------------------------------------------

/** @brief Retrieves the tile of a tensor containing a particular index.
 *
 *  One often needs a particular element of a tensor. This function wraps the
 *  process of retrieving the tile of the tensor which contains that element.
 *
 *  @note If you know the index of the tile you want is `i` just use
 *        `t.find(i)`.
 *
 *  @note This function returns a future to the tile. To get the actual tile
 *        call the `get()` member of the future.
 *
 *  @tparam IndexType Type of the element's index. Assumed to be a random access
 *                    container whose elements are of integral type.
 *  @tparam TensorType Type of the tensor we are accessing. Assumed to be some
 *                     possibly cv-qualified variant of TiledArray::DistArray.
 *  @param[in] elem_idx The index of the element we want. If @p t is a rank
 *                      @f$r@f$ tensor, @p elem_idx should contain (at least)
 *                      @f$r@f$ elements. Indices are interpreted as being
 *                      global element indices.
 *  @param[in] t The tensor we want element @p elem_idx of.
 *
 *  @return A future to the tile which contains @p elem_idx.
 */
template<typename IndexType, typename TensorType>
auto get_tile(IndexType&& elem_idx, TensorType&& t) {
  auto&& trange   = t.trange();
  const auto tile_idx =
      trange.element_to_tile(std::forward<IndexType>(elem_idx));
  return t.find(tile_idx);
}

//------------------------------------------------------------------------------
// Reductions
//------------------------------------------------------------------------------

/** @brief Wraps the process of elementwise reducing two tensors.
 *
 *  Every elementwise reduction of two-tensors can be viewed as a generalization
 *  of the inner product (in practice the actual reduction need not actually
 *  satisfy the mathematical concept of an inner product). That is to say there
 *  are two operations: the "add" operation and the "times" operation. Given two
 *  @f$N@f$ element tensors, @p lhs and @p rhs, such that `lhs[i]` and `rhs[i]`
 *  respectively are the @f$i@f$-th elments of @p lhs and @p rhs, then:
 *
 *  - the "times" operation is used to combine @p lhs and @p rhs into a single
 *    @f$N@f$-element tensor, @f$R@f$, whose @f$i@f$-th value is given by
 *    `R[i] = times(lhs[i], rhs[i]);`, and
 *  - the "add" operation is then used to combine the elements of @f$R@f$ into
 *    a single value.
 *
 *  @todo Does this only work with Tensors? Can it be made to work with futures?
 *
 *  @tparam TileType
 *  @tparam PolicyType
 *  @tparam AddOp Should be a type for an object which can be called as if it
 *                were a function with signature `T(T, T)`, where `T` is the
 *                result of calling @p times_op.
 *  @tparam TimesOp Should be a type of an object which can be called as if it
 *                  were a function with signature
 *                  `T(element_type, element_type)`, where `T` is the input and
 *                  result types of @p AddOp
 * @tparam ResultType Will be deduced to be the result of @p AddOp
 *
 * @param[in] lhs The tensor to go into the left side of @p times_op
 * @param[in] rhs The tensor to go into the right side of @p times_op
 * @param[in] add_op The operation to use as the "add" op
 * @param[in] times_op The operation to use as the "times" op
 * @param[in] init The seed value of the "addition"
 * @return The result of reducing @p lhs and @p rhs using @p add_op and
 *         @p times_op.
 */
template<typename TileType, typename PolicyType, typename AddOp,
         typename TimesOp, typename ResultType>
auto reduce_elementwise(const TA::DistArray<TileType, PolicyType>& lhs,
                        const TA::DistArray<TileType, PolicyType>& rhs,
                        AddOp&& add_op, TimesOp&& times_op,
                        ResultType&& init) {
  using tensor_type = TA::DistArray<TileType, PolicyType>;
  using add_type    = std::decay_t<AddOp>;
  using times_type  = std::decay_t<TimesOp>;

  detail_::Reducer<tensor_type, add_type, times_type> r(
      std::forward<AddOp>(add_op), std::forward<TimesOp>(times_op), init);

  const auto idx = TA::detail::dummy_annotation(lhs.range().rank());

  return lhs(idx).reduce(rhs(idx), std::move(r));
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

/** @brief Determines if corresponding elements of two tensors are "close"
 *
 *  This function will compare two tensors, @f$A@f$ and @f$B@f$, elementwise and
 *  determine if all their values are close to one another. "Close" is defined
 *  by two parameters. The absolute tolerance, @p atol, defines the effective
 *  zero for all comparisons. If for a given index @f$I@f$,
 *  @f$\left\mid A[I] - B[I]\right\mid@ftwo differs by less than @p atol, the
 *  difference is considered indistinguishable from zero and the elements the
 *  same. Particularly for very large elements achieving a difference on the
 *  order of @p atol is not always reasonable; in these cases, one cares more
 *  about the percent error. In this case
 *  @f$\frac{\left\mid A[I] - B[I]\right\mid}{\left\midB[I]\right\mid}@f$ must
 *  be less than a relative tolerance, @p rtol.
 *
 *  This function combines these criteria and returns true if @f$A@f$ and
 *  @f$B@f$ satisfy:
 *
 *  @f{
 *     \left\mid A - B\right\mid \le atol + rtol\left\mid B\right\mid
 *  @f}
 *
 *  Note that this is **NOT** symmetric in @f$A@f$ and @f$B@f$, but is
 *  commensurate with @f$B@f$ being the reference (thereby dictating what is an
 *  acceptable relative tolerance).
 *
 *  @tparam T The type of the first tensor. Assumed to be (possibly
 *            cv-qualified) TiledArray::DistArray
 *  @tparam U The type of the second tensor. Assumed to be the same (up to
 *            cv-qualifiers and value semantics as @p T).
 *  @param[in] actual The tensor you computed and are comparing against a
 *                    reference value.
 *  @param[in] ref The tensor which @p actual is being compared to. Should
 *                 be "the correct value".
 *  @param[in] rtol The maximum percent error (as a decimal) allowed for any
 *                  particular value. Assumed to be a positive decimal. Defaults
 *                  to 1.0E-5, *i.e.*, 0.0001%.
 *  @param[in] atol The effective value of zero for comparisons. Assumed to be a
 *                  positive decimal less than 1.0. Defaults to 1.0E-8.
 *  @return True if @p actual is "close" to @p ref and false otherwise.
 */
template<typename T, typename U,
    typename V = typename std::decay_t<T>::element_type>
bool allclose(T&& actual, U&& ref, V&& rtol = 1.0E-5, V&& atol = 1.0E-8) {
  using tensor_type = std::decay_t<T>;
  static_assert(std::is_same_v<tensor_type, std::decay_t<U>>,
                "different tensor types is currently unsupported");

  // Get a dummy string label (something like "i0, i1, i2, ...")
  const auto idx = TA::detail::dummy_annotation(actual.range().rank());

  // Compute A - B, call result AmB
  tensor_type AmB;
  AmB(idx)      = actual(idx) - ref(idx);

  auto times_op = [=](V lhs, V rhs) {
    return std::fabs(lhs) <= atol + rtol * std::fabs(rhs);
  };
  std::logical_and<bool> add_op;
  return reduce_elementwise(AmB, ref, add_op, times_op, true).get();
}

} // namespace libchemist
