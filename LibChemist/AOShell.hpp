#pragma once
#include <memory>
#include <array>
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
    struct Cartesian{};
    /// A type tag for signaling that the shell is Pure
    struct Spherical{};

    /// The type of a number used for counting
    using size_type = std::size_t;

    /// The type of the object holding the center
    using coord_type = std::array<double, 3>;

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

    ///@{
    AOShell(const AOShell& rhs);
    AOShell(AOShell&& rhs) noexcept;
    AOShell& operator=(const AOShell& rhs);
    AOShell& operator=(AOShell&& rhs) noexcept;
    ///@}

    ~AOShell();

    /**
     * @defgroup State Ctors
     *
     * @brief The constructors in this section can be used to initialize an
     * AOShell with a particular state.
     *
     * The signatures for the ctors in this section may look scary, but really
     * it's just a bit of C++ magic to make the API more friendly.
     * Specifically, the AOShell class ctor will parse a list of arguments
     * provided to it.  The following list summarizes the available capture
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
     * raised if this is not the case.
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
    template<typename...Args>
    explicit AOShell(const coord_type& carts, Args&&...args) :
      AOShell(std::forward<Args>(args)...){
        for(size_type i = 0; i<3; ++i) center()[i] = carts[i];
      }

    template<typename...Args>
    explicit AOShell(size_type l_in, Args&&...args) :
      AOShell(std::forward<Args>(args)...){
        static_assert(!std::disjunction_v<std::is_same<Args, size_type>...>,
          "Please only provide one value of angular momentum");
        l() = l_in;
    }

    template<typename...Args>
    explicit AOShell(const AOPrimitive& prim, Args&&...args):
      AOShell(std::forward<Args>(args)...){
        add_prim_(prim.alpha, prim.c);
    }

    template<typename...Args>
    explicit AOShell(Spherical, Args&&...args) :
      AOShell(std::forward<Args>(args)...) {
        static_assert(!std::disjunction_v<std::is_same<Args, Cartesian>...>,
          "Please only pass either Spherical or Cartesian");
        pure() = true;
    }

    template<typename...Args>
    explicit AOShell(Cartesian, Args&&...args) :
      AOShell(std::forward<Args>(args)...) {
        static_assert(!std::disjunction_v<std::is_same<Args, Spherical>...>,
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

    double& coef(size_type prim_i) noexcept;
    const double& coef(size_type prim_i)const noexcept {
        return const_cast<AOShell&>(*this).coef(prim_i);
    }

    double& alpha(size_type prim_i) noexcept;
    const double& alpha(size_type prim_i)const noexcept {
        return const_cast<AOShell&>(*this).alpha(prim_i);
    }
    ///@}

    bool operator==(const AOShell& rhs)const noexcept;
    bool operator!=(const AOShell& rhs)const noexcept {
        return !((*this) == rhs);
    }
private:
    /**
     * @brief Used to add a primitive to the shell
     *
     * Note that because of the recursive nature of variadic templates this
     * function will ultimately be called in the reverse order, *i.e.* the first
     * call to this function is actually the last primitive the user provided.
     * The PIMPL implementation should account for this and ensure that the
     * order is the same as that provided by the user.
     *
     * @param[in] alpha The exponent of the primitive.
     * @param[in] c The contraction coefficient of the primitive
     *
     * @throw std::bad_alloc if their is insufficient memory to add the
     *                       primitive.  Weak throw guarantee.
     */
    void add_prim_(double alpha, double c);

    ///The class that actually implements the details of this API.
    std::unique_ptr<detail_::AOShellPIMPL> pimpl_;

};

} // namespace LibChemist
