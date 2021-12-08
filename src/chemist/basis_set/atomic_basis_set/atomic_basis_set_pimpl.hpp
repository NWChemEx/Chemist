#pragma once
#include <utility>

#include "../shell/shell_pimpl.hpp"

namespace chemist::detail_ {

/** @brief Implements a AtomicBasisSet
 *
 *  As a technical note, the C++ standard specializes std::vector<bool> so that
 *  it does not return references. We therefore can not use it and allow the
 *  user to do something like `center[i].pure() = false`. To get around this, we
 *  internally store the purity as an integer.
 *
 */
template<typename T>
class AtomicBasisSetPIMPL {
public:
    /// Unsigned integral type used for indexes and offsets
    using size_type = std::size_t;
    /// Type used to store the purity of a shell
    using pure_type = ShellType;
    /// Type used to store the total angular momentum of a shell
    using am_type = size_type;
    /// Type used to hold the purities for a set of shells
    using pure_set = std::vector<pure_type>;
    /// Type used to hold the total angular momentum of a set of shells
    using am_set = std::vector<am_type>;
    /// Type used to hold the parameters for a CTGO
    using param_set = std::vector<T>;
    /// Type used to hold a set of param_set instances
    using shell_set = std::vector<param_set>;

    /** @brief Constructs an empty AtomicBasisSetPIMPL instance.
     *
     *  This ctor can be used to create an empty AtomicBasisSetPIMPL. The
     * resulting instance contains no Shells. Shells can be added by calling
     * add_shell.
     *
     */
    AtomicBasisSetPIMPL();

    /** @brief Creates a new AtomicBasisSetPIMPL with the specified name and
     *         atomic number.
     *
     *  @param[in] name The name of the basis set.
     *  @param[in] atomic_number The atomic number associated with this set.
     *
     */
    explicit AtomicBasisSetPIMPL(const std::string& name,
                                 const size_type& atomic_number);

    /** @brief Makes a deep copy of @p rhs.
     *
     *
     *  @param[in] rhs The instance to copy.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the state
     *                        of @p rhs. Strong throw guarantee.
     */
    AtomicBasisSetPIMPL(const AtomicBasisSetPIMPL<T>& rhs);

    /** @brief Creates a new AtomicBasisSetPIMPL with the specified state.
     *
     *  This ctor can be used to create a new AtomicBasisSetPIMPL with the
     *  provided state. If a parameter is a pointer than the resulting instance
     *  will alias that piece of state, whereas if a parameter is passed by
     *  value the resulting instance will own that parameter.
     *
     *  @tparam U The type of the shells' purities. Expected to be
     *            std::vector<ShellType> or std::vector<ShellType>*.
     *  @tparam V The type of the shells' angular momentum. Expected to be
     *            std::vector<size_type> or std::vector<size_type>*
     *  @tparam W The type of the shells' contraction coefficients. Expected to
     *            be std::vector<std::vector<T>> or std::vector<std::vector<T>>*
     *  @tparam X The type of the shells' primitive exponents. Expected to be
     *            std::vector<std::vector<T>> or std::vector<std::vector<T>>*.
     *
     *  @param[in] name The name of the basis set.
     *  @param[in] atomic_number The atomic number associated with this set.
     *  @param[in] purities A number-of-shells-long vector of shell purities,
     *                      such that the i-th element is the purity of shell i.
     *  @param[in] ls A number-of-shells-long vector of total angular momenta,
     *                such that the i-th element is the total angular momentum
     *                of shell i.
     *  @param[in] cs A number-of-shells long vector such that the i-th element
     *                is a vector containing the contraction coefficients for
     *                shell i.
     *  @param[in] es A number-of-shells long vector such that the i-th element
     *                is a vector containing the exponents for the primitives
     *                comprising the i-th shell.
     *
     *  @throw None no throw guarantee.
     *
     *  Complexity: Constant, but copying of the input values may occur prior to
     *              the call depending on how the values are passed.
     */
    template<typename U, typename V, typename W, typename X>
    AtomicBasisSetPIMPL(const std::string& name, const size_type& atomic_number,
                        U&& purities, V&& ls, W&& cs, X&& es) noexcept;

    /** @brief Creates and adds a new Shell to this AtomicBasisSetPIMPL
     * instance.
     *
     *  This function adds a shell to the center with the provided state.
     *
     *  @param[in] purity Whether the shell is pure or Cartesian?
     *  @param[in] l The total angular momentum of the new shell.
     *  @param[in] cs The contraction coefficients for the shell.
     *  @param[in] es The exponents for the primitives in the shell.
     *
     *  @throw std::bad_alloc if the underlying vectors need to resize and there
     *                        is insufficient memory for that to occur. Weak
     *                        throw guarantee.
     */
    void add_shell(pure_type purity, am_type l, param_set cs, param_set es);

    /** @brief Returns the number of shells on this center.
     *
     *  This function is used to determine the number of Shells currently
     *  centered on the center.
     *
     *  @return The number of shells currently centered on this
     *          AtomicBasisSetPIMPL.
     *
     *  @throw none No throw guarantee.
     *
     *  Complexity: Constant
     */
    size_type size() const noexcept { return m_purities_.value().size(); }

    /** @brief Returns the ShellPIMPL for the @p i-th shell on the center.
     *
     *  This function is code factorization for creating the PIMPL for the
     *  @p i-th shell on the center. The resulting PIMPL is read-/write-able
     *  regardless of the const-ness of this PIMPL with the assumption that the
     *  PIMPL is going into a ShellView which will enforce const-correctness.
     *
     *  @param[in] i The index of the requested shell. Must be in the range
     *               [0, size()).
     *
     *  @return The PIMPL for the requested shell.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, size()). Strong
     *                           throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        new PIMPL. Strong throw guarantee.
     *
     *  Complexity: Constant
     */
    std::unique_ptr<ShellPIMPL<T>> at(size_type i) const;

    /** @brief Returns the name of the basis set. */
    std::string basis_set_name() const { return set_name_; }

    /** @brief Returns the atomic number of the basis set. */
    size_type atomic_number() const { return atomic_number_; }

private:
    /// The name associated with this basis set
    std::string set_name_;
    /// The atomic number associated with this basis set
    std::size_t atomic_number_;
    /// The purities of the shells making up this center
    utilities::OwnOrBorrow<pure_set> m_purities_;
    /// The total angular momenta of the shells making up this center
    utilities::OwnOrBorrow<am_set> m_ls_;
    /// The contraction coefficients for each shell making up this center
    utilities::OwnOrBorrow<shell_set> m_coefs_;
    /// The exponents for each shell making up this center
    utilities::OwnOrBorrow<shell_set> m_exps_;
}; // class AtomicBasisSet

// ---------------------------- Implementations --------------------------------

template<typename T>
AtomicBasisSetPIMPL<T>::AtomicBasisSetPIMPL() : AtomicBasisSetPIMPL("", 0){};

template<typename T>
AtomicBasisSetPIMPL<T>::AtomicBasisSetPIMPL(const std::string& name,
                                            const size_type& atomic_number) :
  set_name_(name), atomic_number_(atomic_number) {}

template<typename T>
AtomicBasisSetPIMPL<T>::AtomicBasisSetPIMPL(const AtomicBasisSetPIMPL<T>& rhs) :
  AtomicBasisSetPIMPL(rhs.set_name_, rhs.atomic_number_,
                      rhs.m_purities_.value(), rhs.m_ls_.value(),
                      rhs.m_coefs_.value(), rhs.m_exps_.value()) {}

template<typename T>
template<typename U, typename V, typename W, typename X>
AtomicBasisSetPIMPL<T>::AtomicBasisSetPIMPL(const std::string& name,
                                            const size_type& atomic_number,
                                            U&& purities, V&& ls, W&& cs,
                                            X&& es) noexcept :
  set_name_(name),
  atomic_number_(atomic_number),
  m_purities_(std::forward<U>(purities)),
  m_ls_(std::forward<V>(ls)),
  m_coefs_(std::forward<W>(cs)),
  m_exps_(std::forward<X>(es)) {}

template<typename T>
void AtomicBasisSetPIMPL<T>::add_shell(pure_type purity, am_type l,
                                       param_set cs, param_set es) {
    m_purities_.value().push_back(purity);
    m_ls_.value().push_back(l);
    m_coefs_.value().emplace_back(std::move(cs));
    m_exps_.value().emplace_back(std::move(es));
}

template<typename T>
std::unique_ptr<ShellPIMPL<T>> AtomicBasisSetPIMPL<T>::at(size_type i) const {
    if(i >= size())
        throw std::out_of_range("Requested index: " + std::to_string(i) +
                                " is not in the range [0, size()).");
    return std::make_unique<ShellPIMPL<T>>(
      const_cast<pure_type*>(&(m_purities_.value()[i])),
      const_cast<am_type*>(&(m_ls_.value()[i])),
      const_cast<param_set*>(&(m_coefs_.value()[i])),
      const_cast<param_set*>(&(m_exps_.value()[i])));
}

} // namespace chemist::detail_
