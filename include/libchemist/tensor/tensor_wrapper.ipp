/** @brief tensor_wrapper.ipp
 *
 *  Inline implementations for the TensorWrapper class. This file should not be
 *  explicitly included in any other file aside from tensor_wrapper.hpp.
 */

#pragma once
#include "libchemist/ta_helpers/slice.hpp"
#include "libchemist/tensor/conversions.hpp"

namespace libchemist::tensor {
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
typename TENSOR_WRAPPER::size_type TENSOR_WRAPPER::size() const {
    auto dims = extents();
    if(dims.empty()) return 0;
    std::multiplies<size_type> fxn;
    return std::accumulate(dims.begin(), dims.end(), size_type{1}, fxn);
}

template<typename VariantType>
auto TENSOR_WRAPPER::extents() const {
    auto l = [&](auto&& arg) {
        extents_type rv;
        if(!arg.is_initialized()) return rv;
        const auto& tr = arg.trange();
        for(size_type i = 0; i < tr.rank(); ++i)
            rv.push_back(tr.dim(i).extent());
        return rv;
    };
    return std::visit(l, m_tensor_);
}

template<typename VariantType>
TENSOR_WRAPPER TENSOR_WRAPPER::slice(
  const std::initializer_list<size_t>& lo,
  const std::initializer_list<size_t>& hi) const {
    sparse_map::ElementIndex low(lo), high(hi);
    auto l = [=](auto&& arg) {
        using clean_t         = std::decay_t<decltype(arg)>;
        constexpr bool is_tot = TensorTraits<clean_t>::is_tot;
        clean_t rv;
        if constexpr(is_tot) {
            throw std::runtime_error("Can't slice a ToT.");
        } else {
            rv = ta_helpers::slice(arg, low, high);
        }
        return rv;
    };
    return TENSOR_WRAPPER(std::visit(l, m_tensor_));
}

template<typename VariantType>
TENSOR_WRAPPER TENSOR_WRAPPER::reshape(
  const std::initializer_list<size_t>& shape) const {
    const auto times_op = std::multiplies<size_t>();
    auto new_volume = std::accumulate(shape.begin(), shape.end(), 1, times_op);
    if(new_volume != size()) {
        std::string msg =
          "Volume of the new shape: " + std::to_string(new_volume) +
          " is not the same as " +
          "the volume of the old shape: " + std::to_string(size());
        throw std::runtime_error(msg);
    }

    // TODO: Use allocator and do not hard-code to a single tile.
    std::vector<TA::TiledRange1> tr1s;
    for(auto x : shape) tr1s.emplace_back(TA::TiledRange1{0, x});
    TA::TiledRange tr(tr1s.begin(), tr1s.end());

    // TODO: Use a distribution aware algorithm
    auto l = [=](auto&& arg) {
        using clean_t = std::decay_t<decltype(arg)>;
        clean_t rv;
        if constexpr(TensorTraits<clean_t>::is_tot) {
            std::runtime_error("Can't reshape a ToT");
        } else {
            auto data = to_vector(*this);
            rv        = TA::make_array<clean_t>(
              arg.world(), tr, [=](auto& tile, const auto& range) {
                  tile = std::decay_t<decltype(tile)>(range);
                  for(const auto& new_idx : range) {
                      tile[new_idx] = data[range.ordinal(new_idx)];
                  }
                  return tile.norm();
              });
        }
        return rv;
    };
    return TENSOR_WRAPPER(std::visit(l, m_tensor_));
}

template<typename VariantType>
std::ostream& TENSOR_WRAPPER::print(std::ostream& os) const {
    auto l = [&](auto&& arg) { os << arg; };
    std::visit(l, m_tensor_);
    return os;
}

template<typename VariantType>
void TENSOR_WRAPPER::hash(pluginplay::Hasher& h) const {
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
    auto l = [](auto&& arg) {
        return arg.is_initialized() ? TA::rank(arg) : 0;
    };
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
            if(!arg.is_initialized()) return size_type{0};
            const auto& tile0 = arg.begin()->get();
            return size_type{tile0[0].range().rank()};
        }
    };
    return std::visit(l, m_tensor_);
}

#undef TENSOR_WRAPPER
} // namespace libchemist::tensor
