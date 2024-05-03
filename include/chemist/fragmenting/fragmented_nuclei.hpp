#pragma once
#include <chemist/chemical_system/nucleus/nucleus.hpp>
#include <chemist/fragmenting/capping/capping.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>

namespace chemist::fragmenting {
namespace detail_ {
template<typename NucleiType>
class FragmentedNucleiPIMPL;
}

template<typename NucleiType>
class FragmentedNuclei : public FragmentedBase<FragmentedNuclei<NucleiType>> {
private:
    /// Type of *this
    using my_type = FragmentedNuclei<NucleiType>;

    /// Type *this derives from
    using base_type = FragmentedBase<my_type>;

public:
    /// Type of the object defining types associated with *this
    using typename base_type::traits_type;

    /// Type of the object implementing *this
    using pimpl_type = detail_::FragmentedNucleiPIMPL<NucleiType>;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of object *this is fragmenting
    using typename base_type::supersystem_type;

    /// Type of the fragments
    using value_type = typename traits_type::fragment_type;

    /// Type of a mutable reference to a fragment
    using reference = typename traits_type::fragment_reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename traits_type::const_fragment_reference;

    /// Type of the object holding the caps for the fragments
    using cap_set_type = typename traits_type::cap_set_type;

    /// Type supersystem uses for indexing and offsets
    using size_type = typename supersystem_type::size_type;

    /// Type used to specify a set of nucleus indices
    using nucleus_index_set = std::vector<size_type>;

    /// Type of a map from fragment index to nuclei offset
    using nucleus_map_type = std::vector<nucleus_index_set>;

    // -- CTors, assignment, and dtor

    /** @brief Creates a null object.
     *
     *  Null FragmentedNuclei objects have no supersystem, not even an empty
     *  one. Since they do not contain a supersystem they can not contain
     *  fragments either.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedNuclei() noexcept;

    /** @brief Creates an empty object.
     *
     *  Empty FragmentedNuclei objects have a supersystem, but no fragments.
     *  Fragments can be added by calling emplace_back. This ctor is most
     *  useful when *this will be filled in algorithmically.
     *
     *  @param[in] supersystem The object *this will hold fragments of.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory. Strong
     *                        throw guarantee.
     */
    explicit FragmentedNuclei(supersystem_type supersystem);

    /** @brief Initializes *this with the provided set of fragments.
     *
     *  @param[in] supersystem The object that is being fragmented.
     *  @param[in] frags A container-of-containers object such that
     *                   `frags[i][j]` is the offset (with respect to
     *                   @p supersystem) of the `j`-th nucleus in the `i`-th
     *                   fragment.
     *
     *  @throw std::bad_alloc if there is a problem allocating the initial
     *                        state. Strong throw guarantee.
     */
    FragmentedNuclei(supersystem_type supersystem, nucleus_map_type frags);

    FragmentedNuclei(supersystem_type supersystem, nucleus_map_type frags,
                     cap_set_type caps);

    /// Defaulted no throw dtor
    ~FragmentedNuclei() noexcept;

protected:
    friend utilities::IndexableContainerBase<my_type>;

    /// Implements at() and operator[]
    reference at_(size_type i);

    /// Implements at() const and operator[] const
    const_reference at_(size_type i) const;

    /// Implements size_
    size_type size_() const noexcept;

private:
    /// Factors out the code for checking if *this has a PIMPL
    bool has_pimpl_() const noexcept;

    /// The object actually implementing *this
    pimpl_pointer m_pimpl_;
};

extern template class FragmentedNuclei<Nuclei>;
extern template class FragmentedNuclei<const Nuclei>;

} // namespace chemist::fragmenting
