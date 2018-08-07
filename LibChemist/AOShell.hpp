#pragma once
#include <array>
#include <memory>
#include <type_traits> //For disjunction/is_same

namespace LibChemist {
namespace detail_ {
class AOShellPIMPL;
} // namespace detail_

/**
 * @brief Structure mainly used for input to CTor of AOShell
 *
 * The AOPrimitive class could be PIMPL-ified, if need be, so that the AOShell
 * class returns AOPrimitives.
 */
struct AOPrimitive {
    /// exponent on primitive
    double alpha;
    /// contraction coefficient on primitive
    double c;
};

/**
 * @brief Holds the details of a shell of atomic orbitals (AOs).
 *
 * This class is part of a hierarchy that strives to be more along the lines of
 * the traditional view of an atomic basis set.  Specifically it treats all of
 * the AOs, and primitives, within a shell as one basis function.
 *
 * For efficiency reasons, the AOShell class guarantees that the alphas and
 * contraction coefficients are contiguous in memory for all primitives within
 * the shell.  Thanks to the PIMPL nature of the AOShell class, other classes
 * (notably the AOBasis class) are free to extend this guarantee if they so
 * choose.  Documentation for the AOShell class assumes the default PIMPL is
 * used.  All other PIMPL instances are free to offer additional guarantees, but
 * they must also satisfy the guarantees listed here.
 */
class AOShell {
public:
    /// A type tag for signaling that the shell is Cartesian
    struct Cartesian {};
    /// A type tag for signaling that the shell is Pure
    struct Spherical {};

    /// The type of a number used for counting
    using size_type = std::size_t;

    /// The type used to store a parameter of the shell
    using value_type = double;

    /// The type of a reference to a stored value
    using reference = value_type&;

    /// The type of a read-only reference to a stored value
    using const_reference = const value_type&;

    /// The type of the object holding the center
    using coord_type = std::array<value_type, 3>;

    /**
     * @brief Creates an pure s-shell with no primitives.
     *
     * This CTor creates a more-or-less placeholder AOShell instance.  That
     * instance will be a pure, s-like AO with no primitives.  Given that there
     * is no way to add additional primitives, except *via* the state CTor,
     * this shell can not be made functional.
     *
     * @throw std::bad_alloc if there is insufficient memory to create the PIMPL
     *        instance.
     */
    AOShell();

    /**
     * @defgroup Copy/Move CTors and Assignment Operators
     *
     * @brief CTors and assignment operators for transferring the state of an
     *        AOShell instance.
     *
     * Behind the scenes the memory semantics of this class are quite
     * complicated.  While it is possible to make the move ctor/assignment
     * operator and the copy ctor behave regularly, the fact that two shells
     * may have different numbers of primitives makes assignment hard as we may
     * need to reallocate to store the extra shells.  While not too hard for an
     * isolated AOShell, this becomes tricky if that AOShell is in the middle
     * of a contiguous chunk of data.  For this reason we have deleted the
     * assignment operator.
     *
     * @param[in] rhs The instance to copy/move.  For move operations
     *            @p rhs is in a valid, but otherwise undefined state.
     *
     * @throw std::bad_alloc Thrown by the copy ctor if there is insufficient
     *        memory to perform the copy.  Strong throw guarantee.
     */
    ///@{
    AOShell(const AOShell& rhs);
    AOShell(AOShell&& rhs) noexcept;
    AOShell& operator=(const AOShell& rhs) = delete;
    AOShell& operator                      =(AOShell&& rhs) noexcept;
    ///@}

    /// Default delete
    ~AOShell() noexcept;

    /**
     * @defgroup State Ctors
     *
     * @brief The constructors in this section can be used to initialize an
     * AOShell with a particular state.
     *
     * The signatures for the ctors in this section may look scary, but really
     * it's just a bit of C++ magic to make the API more friendly.
     * Specifically, these AOShell class ctors will parse a list of arguments
     * provided to them.  The following list summarizes the available capture
     * groups and what they are interpreted as:
     *
     * - `[std::array<double, 3>]` the coordinates for the center of this shell
     * - `[size_type]` the angular momentum of this shell
     * - `[AOPrimitive]` the details of one of the primitives in this shell
     * - `[Spherical || Cartesian]` signals that the shell is pure/Cartesian
     *   - These flags are mutually exclusive
     *
     * With the exception of the `AOPrimitive` capture group, each capture group
     * can appear only once in the constructor list.  A compiler error will be
     * raised if this is not the case.  The arguments to these constructors can
     * appear in any order.
     *
     * @param[in] carts The Cartesian coordinate for the point the shell is
     *            centered on.
     * @param[in] l The angular momentum of this shell.
     * @param[in] prim The exponent and coefficient for a primitive
     * @param[in] args The arguments that have not been parsed yet.
     * @param[in] pimpl Allows creating an AOShell with a particular pimpl
     * @tparam Args The types of the remaining arguments.
     * @throw std::bad_alloc if there is insufficient memory to add the value to
     *        the instance.  Strong throw guarantee.
     *
     */
    ///@{
    template<typename... Args>
    explicit AOShell(const coord_type& carts, Args&&... args) :
      AOShell(std::forward<Args>(args)...) {
        constexpr bool is_carts =
          std::disjunction_v<std::is_same<std::decay_t<Args>, coord_type>...>;
        static_assert(!is_carts,
                      "Please only pass a single set of coordinates");
        for(size_type i = 0; i < 3; ++i) center()[i] = carts[i];
    }

    template<typename... Args>
    explicit AOShell(size_type l_in, Args&&... args) :
      AOShell(std::forward<Args>(args)...) {
        constexpr bool is_am =
          std::disjunction_v<std::is_same<Args, size_type>...>;
        static_assert(!is_am, "Please only provide one set of coordinates.");
        l() = l_in;
    }

    template<typename... Args>
    explicit AOShell(const AOPrimitive& prim, Args&&... args) :
      AOShell(std::forward<Args>(args)..., ColoredPrim{prim}) {}

    template<typename... Args>
    explicit AOShell(Spherical, Args&&... args) :
      AOShell(std::forward<Args>(args)...) {
        constexpr bool is_type =
          std::disjunction_v<std::is_same<Args, Spherical>...,
                             std::is_same<Args, Cartesian>...>;
        static_assert(!is_type,
                      "Please only pass either Spherical or Cartesian");
        pure() = true;
    }

    template<typename... Args>
    explicit AOShell(Cartesian, Args&&... args) :
      AOShell(std::forward<Args>(args)...) {
        constexpr bool is_type =
          std::disjunction_v<std::is_same<Args, Spherical>...,
                             std::is_same<Args, Cartesian>...>;
        static_assert(!is_type,
                      "Please only pass either Spherical or Cartesian");
        pure() = false;
    }

    explicit AOShell(std::unique_ptr<detail_::AOShellPIMPL>&& pimpl) noexcept;
    ///@}

    /**
     * @brief Returns the number of AOs within this shell.
     *
     * For an angular momentum of @f$\ell@f$, there are @f$2\ell +1@f$ AOs
     * in a pure shell and @f$\left( {3 \choose \ell}\right)@f$ AOs in a
     * Cartesian shell.
     *
     * @return The number of AOs within this shell.
     * @throw std::overflow_error if the angular momentum of this shell is too
     *         high (@f$\ell \approx 64@f$).  Strong throw guarantee.
     */
    size_type size() const;

    /**
     * @brief Returns the number of primitives used to form the shell
     *
     * @return The number of primitives.
     * @throw None. No throw guarantee.
     */
    size_type nprims() const noexcept;

    /**
     * @defgroup Getters and Setters
     *
     * @brief The public API for getting and setting the state of an AOShell.
     *
     * The member functions in this section allow getting/setting data if called
     * on a non-const instance of this class and only getting if called on a
     * cont instance.  *N.B.* that once an AOShell instance is made one can not
     * add any additional primitives.
     *
     * @param[in] prim_i The primitive for which information is being requested.
     *            No bounds checks are made.
     *
     * @throw None.  All functions are no throw guarantee.
     */
    ///@{
    bool& pure() noexcept;
    const bool& pure() const noexcept {
        return const_cast<AOShell&>(*this).pure();
    }

    size_type& l() noexcept;
    const size_type& l() const noexcept {
        return const_cast<AOShell&>(*this).l();
    }

    coord_type& center() noexcept;
    const coord_type& center() const noexcept {
        return const_cast<AOShell&>(*this).center();
    }

    reference coef(size_type prim_i) noexcept;
    const_reference coef(size_type prim_i) const noexcept {
        return const_cast<AOShell&>(*this).coef(prim_i);
    }

    reference alpha(size_type prim_i) noexcept;
    const_reference alpha(size_type prim_i) const noexcept {
        return const_cast<AOShell&>(*this).alpha(prim_i);
    }
    ///@}
private:
    /// Struct used to color primitives we've seen while parsing arguments
    struct ColoredPrim {
        AOPrimitive prim;
    };

    /// End-point for recursion
    template<typename... Args>
    AOShell(const ColoredPrim& prim, Args&&... args) : AOShell() {
        for(auto primi : {prim, args...})
            add_prim_(primi.prim.alpha, primi.prim.c);
    }

    /**
     * @brief Used to add a primitive to the shell
     *
     * @param[in] alpha The exponent of the primitive.
     * @param[in] c The contraction coefficient of the primitive
     *
     * @throw std::bad_alloc if their is insufficient memory to add the
     *                       primitive.  Weak throw guarantee.
     */
    void add_prim_(double alpha, double c);

    /// The class that actually implements the details of this API.
    std::unique_ptr<detail_::AOShellPIMPL> pimpl_;
};

/**
 * @defgroup AOShell State Comparison Operators
 * @relates AOShell
 * @brief Operators for asserting whether two AOShell instances are identical
 *
 * Two AOShell instances are identical if they are centered on the same point in
 * space, have the same angular momentum, are both pure (or both Cartesian), and
 * have the same primitives with the same contraction coefficients.  Equality of
 * floating point values is done bitwise with zero tolerance for variation,
 * *i.e.*, every bit of the floating point values must be identical, even if
 * they only differ in the thirteenth significant figure they are not considered
 * equal.
 *
 * @param[in] lhs The AOShell instance on the left side of the operator
 * @param[in] rhs The AOShell instance on the right side of the operator
 * @return Whether the two AOShell instances compare true under the requested
 *         operation.
 * @throw None no throw guarantee.
 */
///@{
bool operator==(const AOShell& lhs, const AOShell& rhs) noexcept;
inline bool operator!=(const AOShell& lhs, const AOShell& rhs) noexcept {
    return !(lhs == rhs);
}
///@}

/**
 * @brief Allows an AOShell to be printed
 *
 * @param os The ostream to forward the shell to.
 * @param shell The shell to print.
 * @return @p os containing the textual representation of @p os
 * @throw std::ios_base::failure if anything goes wrong.  Weak throw guarantee.
 */
std::ostream& operator<<(std::ostream& os, const LibChemist::AOShell& shell);

} // namespace LibChemist
