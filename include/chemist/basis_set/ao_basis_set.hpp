#pragma once
#include "chemist/basis_set/atomic_basis_set.hpp"
#include "chemist/basis_set/detail_/flattened_view.hpp"
#include "chemist/detail_/hashing.hpp"
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename T>
class AOBasisSetPIMPL;
} // namespace detail_

template<typename T>
class AOBasisSet : public utilities::IndexableContainerBase<AOBasisSet<T>> {
private:
    /// Type of this instance
    using my_type = AOBasisSet<T>;
    /// Type of the base class implementing Container API
    using base_type = utilities::IndexableContainerBase<my_type>;
    /// Type of the PIMPL implementing this class
    using pimpl_type = detail_::AOBasisSetPIMPL<T>;

public:
    /// Type of the centers in this basis set
    using value_type = AtomicBasisSet<T>;
    /// Type of a read-/write-able reference to a AtomicBasisSet
    using reference = AtomicBasisSet<T>&;
    /// Type of a read-only reference to a AtomicBasisSet
    using const_reference = const AtomicBasisSet<T>&;
    /// Unsigned integral type used for indexing/offsets
    using size_type = typename base_type::size_type;

    /** @brief Creates a new AOBasisSet instance containing no centers.
     *
     *  This ctor can be used to create a new AOBasisSet instance which contains
     *  no centers. Centers can be added by calling `add_center`.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     *
     *  Complexity: Constant
     */
    AOBasisSet();

    /** @brief Creates a new AOBasisSet by deep copying another instance.
     *
     *  This ctor can be used to create a new AOBasisSet instance which contains
     *  a deep copy of @p rhs.
     *
     *  @param[in] rhs The AOBasisSet instance to deep copy.
     *
     *  @throw std::bad_alloc if there is insufficient memory to deep copy
     *                        @p rhs. Strong throw guarantee.
     *
     *  Complexity: Linear in the size of @p rhs.
     */
    AOBasisSet(const AOBasisSet<T>& rhs);

    /** @brief Creates a new AOBasisSet instance which takes ownership of
     *         another instance's state.
     *
     *  This ctor will create a new AOBasisSet instance by taking ownership of
     *  @p rhs's state.
     *
     *  @param[in,out] rhs The instance whose state will be transferred to the
     *                     resulting instance. After the operation @p rhs will
     *                     not contain a PIMPL and will thus not be usable until
     *                     another PIMPL-containing AOBasisSet instance is
     *                     assigned to it.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    AOBasisSet(AOBasisSet<T>&& rhs) noexcept;

    /** @brief Makes the current instance contain a deep copy of another
     *         AOBasisSet.
     *
     *  This function can be used to make the current AOBasisSet instance a deep
     *  copy of @p rhs. The state this instance had prior to the call will be
     *  released.
     *
     *  @param[in] rhs The AOBasisSet instance to deep copy.
     *
     *  @return The current instance containing a deep copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is insufficient memory to deep copy
     *                        @p rhs. Strong throw guarantee.
     *
     *  Complexity: Linear in the size of @p rhs.
     */
    AOBasisSet& operator=(const AOBasisSet<T>& rhs);

    /** @brief Makes the current instance take ownership of another AOBasisSet
     *         instance's state.
     *
     *  This function will transfer ownerhsip of @p rhs's state to the current
     *  instance. After the transfer the state originally held by this instance
     *  will be released.
     *
     *  @param[in] rhs The instance whose state is being taken. After this
     *                 function, @p rhs will not contain a PIMPL and will need
     *                 to be assigned to in order to be used again.
     *
     *  @return The current instance after taking ownership of @p rhs's state.
     *
     *  Complexity: Constant.
     */
    AOBasisSet& operator=(AOBasisSet<T>&& rhs) noexcept;

    /// Defaulted no-throw dtor
    ~AOBasisSet() noexcept;

    /** @brief Adds an additional AtomicBasisSet instance to this basis set.
     *
     *  @param[in] center The center to add to this basis set.
     *
     *  @throw std::bad_alloc if the underlying buffers need to reallocate and
     *                        there is insufficient memory to do so. Strong
     *                        throw guarantee.
     */
    void add_center(value_type center);

    /** @brief Determines the largest total angular momentum of the shells in
     *         the basis set.
     *
     *  Many integral libraries preallocate a scratch buffer given the maximum
     *  total angular momentum of any particular shell. This function is a
     *  convenience function for determining that value for the current basis
     *  set.
     *
     *  @return The maximum total angular momentum of the shell with the largest
     *          total angular momentum.
     *
     *  @throw std::runtime_error if this basis set does not contain any shells.
     *         Strong throw guarantee.
     *
     *  @note Since a total angular momentum of 0 is a valid result, we instead
     *        opt to throw if this function is called on an empty basis set.
     */
    size_type max_l() const;

    // ------------------------- Shells ----------------------------------------
    /// Type used to model a Shell
    using shell_type = typename value_type::value_type;
    /// Type used to model a read-/write-able reference to a Shell
    using shell_reference = typename value_type::reference;
    /// Type used to model a read-only reference to a Shell
    using const_shell_reference = typename value_type::const_reference;
    /// Type of a read-/write-able view of the basis set as a set of Shells
    using flattened_shells =
      detail_::FlattenedView<shell_reference, const_shell_reference>;
    /// Type of a read-only view of the basis set as a set of Shells
    using const_flattened_shells =
      detail_::FlattenedView<const_shell_reference, const_shell_reference>;

    /** @brief Returns the total number of shells in this basis set.
     *
     *  This function will sum up the total number of shells on each center
     *  comprising the basis set.
     *
     *  @return The total number of shells in the basis set.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Linear in the number of centers.
     */
    size_type n_shells() const noexcept;

    /** @brief Returns the @p i-th shell in the basis set.
     *
     *  Shells are numbered by flattening out the centers comprising the basis
     *  set. The @f$n_0@f$ shells belonging to the 0-th center comprise shells
     *  0 to @f$n_0 -1@f$, the @f$n_1@f$ shells belonging to the 1-st center
     *  comprise shells @f$n_0@f$ through @f$n_0 + n_1 - 1@f$, etc.
     *
     *  @param[in] i The index of the requested shell. Must be in the range
     *               [0, n_shells()).
     *  @return A read-/write-able reference to the requested shell.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, n_shells()).
     *                           Strong throw guarantee.
     */
    shell_reference shell(size_type i);

    /** @brief Returns the @p i-th shell in the basis set.
     *
     *  Shells are numbered by flattening out the centers comprising the basis
     *  set. The @f$n_0@f$ shells belonging to the 0-th center comprise shells
     *  0 to @f$n_0 -1@f$, the @f$n_1@f$ shells belonging to the 1-st center
     *  comprise shells @f$n_0@f$ through @f$n_0 + n_1 - 1@f$, etc.
     *
     *  @param[in] i The index of the requested shell. Must be in the range
     *               [0, n_shells()).
     *
     *  @return A read-only reference to the requested shell.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, n_shells()).
     *                           Strong throw guarantee.
     */
    const_shell_reference shell(size_type i) const;

    /** @brief Returns a proxy container to facilitate looping over shells in a
     *         foreach loop.
     *
     *  This member function is used to loop over the shells in this basis set
     *  in a foreach loop like: `for(auto&& shell_i : bs.shells())`, where `bs`
     * is this instance and `shell_i` will be a read-/write-able reference to a
     * shell in this basis set.
     *
     *  @return A container which simulates being filled with all of the shells
     *          in this basis set to facilitate usage in a foreach loop.
     *
     *  @throw None No throw guarantee.
     */
    flattened_shells shells() noexcept;

    /** @brief Returns a proxy container to facilitate looping over shells in a
     *         foreach loop.
     *
     *  This member function is used to loop over the shells in this basis set
     *  in a foreach loop like: `for(auto&& shell_i : bs.shells())`, where `bs`
     * is this instance and `shell_i` will be a read-only reference to a shell
     * in this basis set.
     *
     *  @return A container which simulates being filled with all of the shells
     *          in this basis set to facilitate usage in a foreach loop.
     *
     *  @throw None No throw guarantee.
     */
    const_flattened_shells shells() const noexcept;

    /** @brief Returns a vector of shell offsets per center
     *
     *  This function returns a vector whose elements are such that the i-th
     *  element is the first shell on the i-th center in the set and the final
     *  element is the total number of shells.
     *
     *  @return A vector of shell offsets
     */
    std::vector<size_type> shell_offsets() const;

    // ---------------------------- AOs ----------------------------------------
    /// Type used to model an AO
    using ao_type = typename value_type::ao_type;
    /// Type of a read-/write-able reference to an AO
    using ao_reference = typename value_type::ao_reference;
    /// Type of a read-only reference to an AO
    using const_ao_reference = typename value_type::const_ao_reference;
    /// Type of a read-/write-able view of the basis set as a set of AOs
    using flattened_aos =
      detail_::FlattenedView<ao_reference, const_ao_reference>;
    /// Type of a read-only view of the basis set as a set of AOs
    using const_flattened_aos =
      detail_::FlattenedView<const_ao_reference, const_ao_reference>;

    /** @brief Returns the total number of AOs in this basis set.
     *
     *  This function will sum up the total number of AOs on each center in this
     *  basis set.
     *
     *  @return  The total number of AOs in this basis set.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Linear in the total number of shells.
     */
    size_type n_aos() const noexcept;

    /** @brief Returns the @p i-th AO in this basis set.
     *
     *  The AOs in this basis set are numbered such that the first @f$n_{0}@f$
     *  in this basis set's 0-th shell are numbered 0 to @f$n_{0} -1@f$. The
     *  @f$n_{1}@f$ AOs in the basis set's 1-st shell are numbered @f$n_0@f$ to
     *  @f$n_0 + n_1 -1@f$, etc.
     *
     *  @param[in] i The index of the requested AO. Must be in the range [0,
     *               n_aos()).
     *  @return A read-/write-able reference to the requested AO.
     *
     *  @throw std::out_of_range if the requested AO is not in the range [0,
     *                           n_aos()). Strong throw guarantee.
     */
    ao_reference ao(size_type i);

    /** @brief Returns the @p i-th AO in this basis set.
     *
     *  The AOs in this basis set are numbered such that the first @f$n_{0}@f$
     *  in this basis set's 0-th shell are numbered 0 to @f$n_{0} -1@f$. The
     *  @f$n_{1}@f$ AOs in the basis set's 1-st shell are numbered @f$n_0@f$ to
     *  @f$n_0 + n_1 -1@f$, etc.
     *
     *  @param[in] i The index of the requested AO. Must be in the range [0,
     *               n_aos()).
     *
     *  @return A read-only reference to the requested AO.
     *
     *  @throw std::out_of_range if the requested AO is not in the range [0,
     *                           n_aos()). Strong throw guarantee.
     */
    const_ao_reference ao(size_type i) const;

    /** @brief Returns a proxy container to facilitate looping over AOs in a
     *         foreach loop.
     *
     *  This member function is used to loop over the AOs in this basis set
     *  in a foreach loop like: `for(auto&& ao_i : bs.aos())`, where `bs` is
     *  this instance and `ao_i` will be a read-/write-able reference to an AO
     *  in this basis set.
     *
     *  @return A container which simulates being filled with all of the AOs
     *          in this basis set to facilitate usage in a foreach loop.
     *
     *  @throw None No throw guarantee.
     */
    flattened_aos aos() noexcept;

    /** @brief Returns a proxy container to facilitate looping over AOs in a
     *         foreach loop.
     *
     *  This member function is used to loop over the AOs in this basis set
     *  in a foreach loop like: `for(auto&& ao_i : bs.aos())`, where `bs` is
     *  this instance and `ao_i` will be a read-only reference to an AO
     *  in this basis set.
     *
     *  @return A container which simulates being filled with all of the AOs
     *          in this basis set to facilitate usage in a foreach loop.
     *
     *  @throw None No throw guarantee.
     */
    const_flattened_aos aos() const noexcept;

    /** @brief Returns a vector of AO offsets per center
     *
     *  This function returns a vector whose elements are such that the i-th
     *  element is the first AO on the i-th center in the set and the final
     *  element is the total number of AOs.
     *
     *  @return A vector of shell offsets
     */
    std::vector<size_type> ao_offsets() const;

    // ---------------------------- Primitives ---------------------------------
    /// Type used to model a primitive Gaussian
    using primitive_type = typename value_type::primitive_type;
    /// Type used to model a read-/write-able reference to a primitive
    using primitive_reference = typename value_type::primitive_reference;
    /// Type used to model a read-only reference to a primitive
    using const_primitive_reference =
      typename value_type::const_primitive_reference;
    /// Type of a read-/write-able view of the basis set as a set of primitives
    using flattened_primitives =
      detail_::FlattenedView<primitive_reference, const_primitive_reference>;
    /// Type of a read-only view of the basis set as a set of primitives
    using const_flattened_primitives =
      detail_::FlattenedView<const_primitive_reference,
                             const_primitive_reference>;

    /** @brief Returns the total number of unique primitives in this basis set.
     *
     *  This function will compute the total number of unique primitives in this
     *  basis set.
     *
     *  @return The total number of unique primitives in this basis set.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Linear in the number of unique AOs
     */
    size_type n_unique_primitives() const noexcept;

    /** @brief Returns the @p i-th unique primitive in this basis set.
     *
     *  The unique primitives in this basis set are numbered such that the first
     *  @f$n_{0}@f$  in this basis set's 0-th unique AO are numbered 0 to
     *  @f$n_{0} -1@f$. The @f$n_{1}@f$ unique primitives in the basis set's
     *  1-st unique AO are numbered @f$n_0@f$ to @f$n_0 + n_1 -1@f$, etc.
     *
     *  @param[in] i The index of the requested unique primitive. Must be in the
     *               range [0, n_unique_primitives()).
     *
     *  @return A read-/write-able reference to the requested unique primitive.
     *
     *  @throw std::out_of_range if the requested unique primitive is not in the
     *                           range [0, n_unique_primitives()). Strong throw
     *                           guarantee.
     */
    primitive_reference unique_primitive(size_type i);

    /** @brief Returns the @p i-th unique primitive in this basis set.
     *
     *  The unique primitives in this basis set are numbered such that the first
     *  @f$n_{0}@f$  in this basis set's 0-th unique AO are numbered 0 to
     *  @f$n_{0} -1@f$. The @f$n_{1}@f$ unique primitives in the basis set's
     *  1-st unique AO are numbered @f$n_0@f$ to @f$n_0 + n_1 -1@f$, etc.
     *
     *  @param[in] i The index of the requested unique primitive. Must be in the
     *               range [0, n_unique_primitives()).
     *
     *  @return A read-only reference to the requested unique primitive.
     *
     *  @throw std::out_of_range if the requested unique primitive is not in the
     *                           range [0, n_unique_primitives()). Strong throw
     *                           guarantee.
     */
    const_primitive_reference unique_primitive(size_type i) const;

    /** @brief Returns a proxy container to facilitate looping over unique
     *         primitives in a foreach loop.
     *
     *  This member function is used to loop over the unique_primitives in this
     *  basis set in a foreach loop like:
     *  `for(auto&& prim_i : bs.unique_primitives())`, where `bs` is this
     *  instance and `prim_i` will be a read-/write-able reference to a unique
     *  primitive in this basis set.
     *
     *  @return A container which simulates being filled with all of the unique
     *          primitives in this basis set to facilitate usage in a foreach
     *          loop.
     *
     *  @throw None No throw guarantee.
     */
    flattened_primitives unique_primitives() noexcept;

    /** @brief Returns a proxy container to facilitate looping over unique
     *         primitives in a foreach loop.
     *
     *  This member function is used to loop over the unique_primitives in this
     *  basis set in a foreach loop like:
     *  `for(auto&& prim_i : bs.unique_primitives())`, where `bs` is this
     *  instance and `prim_i` will be a read-only reference to a unique
     *  primitive in this basis set.
     *
     *  @return A container which simulates being filled with all of the unique
     *          primitives in this basis set to facilitate usage in a foreach
     *          loop.
     *
     *  @throw None No throw guarantee.
     */
    const_flattened_primitives unique_primitives() const noexcept;

    /** @brief Serialize AOBasisSet instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const {
        ar& this->size();
        for(const auto& c : *this) { ar& c; }
    }

    /** @brief Deserialize for AOBasisSet instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar) {
        size_type nc;
        ar& nc;
        chemist::AtomicBasisSet<T> c;
        for(int ci = 0; ci < nc; ++ci) {
            ar& c;
            this->add_center(std::move(c));
        }
    }

    /** @brief Hash AOBasisSet instance
     *
     * @param h Hasher object
     */
    void hash(detail_::Hasher& h) const;

    /** @brief Makes this AOBasisSet the union of this set and @p rhs.
     *
     *  Strictly speaking, this function will concatenate the basis functions in
     *  the present basis set with those in @p rhs. It's only a union if the
     *  basis sets are disjoint.
     *
     *  @param[in] rhs The basis we are taking the union with.
     *
     *  @return The current basis set, after adding @p rhs to it.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        new basis functions. Weak throw guarantee.
     */
    AOBasisSet<T>& operator+=(const AOBasisSet<T>& rhs);

    /** @brief Computes the AOBasisSet which is the union of this and @p rhs.
     *
     *  Strictly speaking, this function will concatenate the basis functions in
     *  the present basis set with those in @p rhs. It's only a union if the
     *  basis sets are disjoint.
     *
     *  @param[in] rhs The basis we are taking the union with.
     *
     *  @return The basis set resulting from the union of this with @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory for the
     *                        new basis functions. Strong throw guarantee.
     */
    AOBasisSet<T> operator+(const AOBasisSet<T>& rhs) const;

private:
    /// Allows the base class to implement container API
    friend base_type;
    /// Implements `size()` function
    size_type size_() const noexcept;
    /// Implements `operator[](size_type i)`
    reference at_(size_type i);
    /// Implements `operator[](size_type i)const`
    const_reference at_(size_type i) const;
    /// The instance actually implementing the API
    std::unique_ptr<detail_::AOBasisSetPIMPL<T>> m_pimpl_;
}; // class AOBasisSet

template<typename T>
AOBasisSet<T>& AOBasisSet<T>::operator+=(const AOBasisSet<T>& rhs) {
    for(const auto& ci : rhs) add_center(ci);
    return *this;
}

template<typename T>
AOBasisSet<T> AOBasisSet<T>::operator+(const AOBasisSet<T>& rhs) const {
    AOBasisSet<T> rv(*this);
    return rv += rhs;
}

extern template class AOBasisSet<double>;
extern template class AOBasisSet<float>;

} // namespace chemist
