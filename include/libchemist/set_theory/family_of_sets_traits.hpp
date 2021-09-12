#pragma once
#include <memory>
#include <tuple>

namespace libchemist::set_theory {
template<typename SetType>
class Subset;

template<typename SetType>
struct FamilyOfSetsTraits {
private:
    /// The inner type of the initializer list, maps to a subset
    template<typename SizeType>
    using offset_il = std::initializer_list<SizeType>;

public:
    using value_type = Subset<SetType>;

    using reference_type = value_type&;

    using const_reference = const value_type&;

    template<typename SizeType>
    using il_type = std::initializer_list<offset_il<SizeType>>;

    using ptr_type = std::shared_ptr<const SetType>;

    static ptr_type make_pointer(SetType set) {
        return std::make_shared<SetType>(std::move(set));
    }
};

template<typename... SetType>
struct FamilyOfSetsTraits<std::tuple<SetType...>> {
private:
    static constexpr std::size_t N = sizeof...(SetType);

    template<typename SizeType>
    using offset_il = std::initializer_list<SizeType>;

public:
    using parent_set_type = std::tuple<SetType...>;

    using value_type = std::tuple<Subset<SetType>...>;

    using reference_type = value_type&;

    using const_reference = const value_type&;

    template<typename SizeType>
    using il_element_type = std::initializer_list<offset_il<SizeType>>;

    template<typename SizeType>
    using il_type = std::initializer_list<il_element_type<SizeType>>;

    using ptr_type = std::tuple<std::shared_ptr<const SetType>...>;

    /** @brief Wraps the logic for making the shared_ptr to the parent set.
     *
     */
    static auto make_pointer(parent_set_type pset) {
        return make_pointer_(std::move(pset), std::make_index_sequence<N>());
    }

    template<typename SizeType>
    static value_type new_subset(ptr_type parent,
                                 il_element_type<SizeType> il) {
        return new_subset_(parent, il, std::make_index_sequence<N>());
    }

private:
    template<typename SizeType, std::size_t... Is>
    static auto new_subset_(ptr_type parent, il_element_type<SizeType> il,
                            std::index_sequence<Is...>);

    /** @brief Implements make_pointer by expanding the provided index_sequence
     *
     *  @tparam Is template non-type parameters to be used as indices for
     *             manipulating the tuple. @p Is should contain the values
     *             [0, N).
     */
    template<std::size_t... Is>
    static auto make_pointer_(parent_set_type pset, std::index_sequence<Is...>);
};

template<typename... SetTypes>
template<typename SizeType, std::size_t... Is>
auto FamilyOfSetsTraits<std::tuple<SetTypes...>>::new_subset_(
  ptr_type ptr, il_element_type<SizeType> il, std::index_sequence<Is...>) {
    return value_type{Subset<std::tuple_element_t<Is, parent_set_type>>(
      std::get<Is>(ptr), *(il.begin() + Is))...};
}

template<typename... SetTypes>
template<std::size_t... Is>
auto FamilyOfSetsTraits<std::tuple<SetTypes...>>::make_pointer_(
  parent_set_type pset, std::index_sequence<Is...>) {
    return ptr_type{std::make_shared<std::tuple_element_t<Is, parent_set_type>>(
      std::move(std::get<Is>(pset)))...};
}

} // namespace libchemist::set_theory