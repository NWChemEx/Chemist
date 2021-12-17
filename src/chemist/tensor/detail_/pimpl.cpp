#include "chemist/ta_helpers/slice.hpp"
#include "chemist/ta_helpers/ta_helpers.hpp"
#include "chemist/tensor/detail_/pimpl.hpp"

namespace chemist::tensor::detail_ {
namespace {

// TODO: This is basically chemist::tensor::to_vector copy/pasted, need to
//       work this into main to_vector routine
auto to_vector_from_pimpl(const TensorWrapperPIMPL<field::Scalar>& t) {
    auto t_ta = std::get<TA::TSpArrayD>(t.variant());
    t_ta.make_replicated();
    std::vector<double> rv(t.size(), 0.0);

    for(const auto& tile_i : t_ta) {
        const auto& i_range = tile_i.get().range();
        for(auto idx : i_range) rv[i_range.ordinal(idx)] = tile_i.get()[idx];
    }
    return rv;
}

template<typename VariantType>
auto make_extents(VariantType&& v) {
    using extents_type = TensorWrapperPIMPL<field::Scalar>::extents_type;
    using size_type    = typename extents_type::size_type;

    auto l = [=](auto&& t) {
        if(!t.is_initialized()) return extents_type{};
        const auto& tr = t.trange();
        extents_type rv(tr.rank());
        const auto& erange = tr.elements_range().extent();
        for(size_type i = 0; i < rv.size(); ++i) rv[i] = erange[i];
        return rv;
    };
    return std::visit(l, std::forward<VariantType>(v));
}

} // namespace

// Macro to avoid retyping the full type of the PIMPL
#define PIMPL_TYPE TensorWrapperPIMPL<FieldType>

template<typename FieldType>
PIMPL_TYPE::TensorWrapperPIMPL(variant_type v, allocator_pointer p) :
  m_tensor_(std::move(v)),
  m_allocator_(std::move(p)),
  m_shape_(std::make_unique<shape_type>(make_extents(m_tensor_))) {
    reallocate_(*m_allocator_);
}

template<typename FieldType>
PIMPL_TYPE::TensorWrapperPIMPL(variant_type v, shape_pointer s,
                               allocator_pointer p) :
  m_tensor_(std::move(v)),
  m_allocator_(std::move(p)),
  m_shape_(std::make_unique<shape_type>(make_extents(m_tensor_))) {
    reallocate_(*m_allocator_);
    reshape(std::move(s));
}

template<typename FieldType>
typename PIMPL_TYPE::pimpl_pointer PIMPL_TYPE::clone() const {
    allocator_pointer new_alloc(m_allocator_ ? m_allocator_->clone() : nullptr);
    shape_pointer new_shape(m_shape_ ? m_shape_->clone() : nullptr);
    return std::make_unique<my_type>(m_tensor_, std::move(new_shape),
                                     std::move(new_alloc));
}

template<typename FieldType>
typename PIMPL_TYPE::const_allocator_reference PIMPL_TYPE::allocator() const {
    if(m_allocator_) return *m_allocator_;
    throw std::runtime_error("Tensor has no allocator!!!!");
}

template<typename FieldType>
typename PIMPL_TYPE::const_shape_reference PIMPL_TYPE::shape() const {
    if(m_shape_) return *m_shape_;
    throw std::runtime_error("Tensor has no shape!!!!");
}

template<typename FieldType>
typename PIMPL_TYPE::labeled_variant_type PIMPL_TYPE::annotate(
  const annotation_type& annotation) {
    using new_variant = labeled_variant_t<variant_type>;
    auto l            = [&](auto&& t) { return new_variant(t(annotation)); };
    return std::visit(l, m_tensor_);
}

template<typename FieldType>
typename PIMPL_TYPE::const_labeled_type PIMPL_TYPE::annotate(
  const annotation_type& annotation) const {
    using new_variant = const_labeled_type;
    auto l            = [&](auto&& t) { return new_variant(t(annotation)); };
    return std::visit(l, m_tensor_);
}

template<typename FieldType>
typename PIMPL_TYPE::extents_type PIMPL_TYPE::extents() const {
    return m_shape_ ? m_shape_->extents() : extents_type{};
}

template<typename FieldType>
typename PIMPL_TYPE::annotation_type PIMPL_TYPE::make_annotation(
  const annotation_type& letter) const {
    auto r                = rank();
    constexpr bool is_tot = std::is_same_v<FieldType, field::Tensor>;
    auto outer_rank       = (is_tot ? outer_rank_() : r);
    annotation_type x;
    if(r == 0) return x;
    for(decltype(r) i = 0; i < r - 1; ++i) {
        x += letter + std::to_string(i);
        x += (i + 1 == outer_rank ? ";" : ",");
    }
    x += letter + std::to_string(r - 1);
    return x;
}

template<typename FieldType>
typename PIMPL_TYPE::rank_type PIMPL_TYPE::rank() const {
    return outer_rank_() + inner_rank_();
}

template<typename FieldType>
void PIMPL_TYPE::reallocate(allocator_pointer p) {
    reallocate_(*p);
    m_allocator_ = std::move(p);
}

template<typename FieldType>
void PIMPL_TYPE::reshape(shape_pointer pshape) {
    reshape_(*pshape);
    m_shape_ = std::move(pshape);
}

template<typename FieldType>
typename PIMPL_TYPE::size_type PIMPL_TYPE::size() const {
    auto dims = extents();
    if(dims.empty()) return 0;
    std::multiplies<size_type> fxn;
    return std::accumulate(dims.begin(), dims.end(), size_type{1}, fxn);
}

template<typename FieldType>
typename PIMPL_TYPE::pimpl_pointer PIMPL_TYPE::slice(
  const il_type& lo, const il_type& hi, allocator_pointer p) const {
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

    return std::make_unique<my_type>(std::visit(l, m_tensor_), std::move(p));
}

template<typename FieldType>
std::ostream& PIMPL_TYPE::print(std::ostream& os) const {
    auto l = [&](auto&& arg) { os << arg; };
    std::visit(l, m_tensor_);
    return os;
}

template<typename FieldType>
void PIMPL_TYPE::hash(pluginplay::Hasher& h) const {
    h(m_shape_, m_allocator_);
    auto l = [&](auto&& arg) { h(arg); };
    std::visit(l, m_tensor_);
}

template<typename FieldType>
bool PIMPL_TYPE::operator==(const TensorWrapperPIMPL& rhs) const {
    // Compare shapes
    if(m_shape_ && rhs.m_shape_) {
        if(!m_shape_->is_equal(*rhs.m_shape_)) return false;
    } else if(!m_shape_ != !rhs.m_shape_)
        return false;

    // Compare allocators
    if(m_allocator_ && rhs.m_allocator_) {
        if(!m_allocator_->is_equal(*rhs.m_allocator_)) return false;
    } else if(!m_allocator_ != !rhs.m_allocator_)
        return false;

    auto l = [&](auto&& lhs) {
        auto m = [&](auto&& rhs) { return lhs == rhs; };
        return std::visit(m, rhs.m_tensor_);
    };
    return std::visit(l, m_tensor_);
}

//------------------------------------------------------------------------------
//                     Private Member Functions
//------------------------------------------------------------------------------

template<typename FieldType>
void PIMPL_TYPE::reshape_(const shape_type& other) {
    auto shape = other.extents();

    // Short-circuit if shapes are polymorphically equivalent
    if(m_shape_->is_equal(other)) return;

    // If the extents aren't the same we're shuffling elements around
    if(shape != extents()) shuffle_(shape);

    // TODO: This is basically a hack to compare the sparsities
    auto ta_tensor = other.make_tensor(*m_allocator_);

    auto l = [&](auto&& old_tensor) {
        auto m = [&](auto&& new_tensor) {
            if(old_tensor.shape() == new_tensor.shape()) return;
            const auto& new_shape = new_tensor.shape();
            auto dummy_idx        = make_annotation("j");
            old_tensor(dummy_idx) = old_tensor(dummy_idx).set_shape(new_shape);
        };
        std::visit(m, ta_tensor);
    };
    std::visit(l, m_tensor_);
}

template<typename FieldType>
void PIMPL_TYPE::reallocate_(const_allocator_reference p) {
    auto l = [&](auto&& arg) {
        // We have nothing to do if it's not initialized yet
        if(!arg.is_initialized()) return;

        // Only retile if the tiled ranges are different
        const auto tr = p.make_tiled_range(extents());
        if(arg.trange() != tr) {
            if constexpr(std::is_same_v<FieldType, field::Scalar>) {
                arg = TA::retile(arg, tr);
            } else {
                throw std::runtime_error("reallocate for ToT NYI!!!");
            }
        }
    };
    std::visit(l, m_tensor_);
}

template<typename FieldType>
void PIMPL_TYPE::shuffle_(const extents_type& shape) {
    const auto times_op = std::multiplies<size_t>();
    auto new_volume = std::accumulate(shape.begin(), shape.end(), 1, times_op);

    if(new_volume != size()) {
        std::string msg =
          "Volume of the new shape: " + std::to_string(new_volume) +
          " is not the same as " +
          "the volume of the old shape: " + std::to_string(size());
        throw std::runtime_error(msg);
    }

    auto tr = m_allocator_->make_tiled_range(shape);

    // TODO: Use a distribution aware algorithm
    auto l = [=](auto&& arg) {
        using clean_t = std::decay_t<decltype(arg)>;
        clean_t rv;
        if constexpr(TensorTraits<clean_t>::is_tot) {
            std::runtime_error("Can't reshape a ToT");
        } else {
            auto data = to_vector_from_pimpl(*this);
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
    m_tensor_ = std::visit(l, m_tensor_);
}

template<typename FieldType>
typename PIMPL_TYPE::rank_type PIMPL_TYPE::inner_rank_() const noexcept {
    if constexpr(std::is_same_v<FieldType, field::Scalar>) {
        return rank_type{0};
    } else {
        auto l = [](auto&& arg) {
            if(!arg.is_initialized()) return rank_type{0};
            const auto& tile0 = arg.begin()->get();
            return rank_type{tile0[0].range().rank()};
        };
        return std::visit(l, m_tensor_);
    }
}

template<typename FieldType>
typename PIMPL_TYPE::rank_type PIMPL_TYPE::outer_rank_() const noexcept {
    auto l = [](auto&& arg) {
        return arg.is_initialized() ? TA::rank(arg) : 0;
    };
    return std::visit(l, m_tensor_);
}

#undef PIMPL_TYPE

template class TensorWrapperPIMPL<field::Scalar>;
template class TensorWrapperPIMPL<field::Tensor>;

} // namespace chemist::tensor::detail_
