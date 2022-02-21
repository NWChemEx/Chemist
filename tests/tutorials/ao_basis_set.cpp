// TUTORIAL
//
// This tutorial will show you how to use the ``AOBasisSet`` class by means of
// example. The vast majority of code in NWX will be given an ``AOBasisSet``
// instance and will not need to create one; nonetheless since we want this
// tutorial to be self contained (and because someone, somewhere, must make
// the ``AOBasisSet`` instance you are using) we start by showing how to
// create an ``AOBasisSet`` instance.
//
// All of the functionality of the ``AOBasisSet`` class is contained in the
// ``chemist/basis_set/basis_set.hpp`` header file. After including this
// header file the next several lines fill in an ``AOBasisSet`` instance with
// the STO-3G parameters for a water molecule. If you are not interested in
// how to make an ``AOBasisSet`` instance skip to the next code block.
#include <chemist/basis_set/basis_set.hpp>

// TUTORIAL_START_SKIP
#include <catch2/catch.hpp>
namespace tutorial {
TEST_CASE("AOBasisSet Tutorial") {
    // TUTORIAL_STOP_SKIP

    chemist::AOBasisSet<double> bs;

    // Atomic basis set for the oxygen atom
    chemist::AtomicBasisSet<double> O("STO-3G", 8, 0.0, -0.1432223429807816,
                                      0.0);
    O.add_shell(chemist::ShellType::pure, 0,
                std::vector<double>{0.15432897, 0.53532814, 0.44463454},
                std::vector<double>{99.1061690, 18.0523120, 4.8856602});
    O.add_shell(chemist::ShellType::pure, 0,
                std::vector<double>{-0.09996723, 0.39951283, 0.70011547},
                std::vector<double>{3.7804559, 0.8784966, 0.2857144});
    O.add_shell(chemist::ShellType::pure, 1,
                std::vector<double>{0.15591627, 0.60768372, 0.39195739},
                std::vector<double>{3.7804559, 0.8784966, 0.2857144});

    // Atomic basis set for the first hydrogen atom
    chemist::AtomicBasisSet<double> H1("STO-3G", 1, 1.6380335020342418,
                                       1.1365568803584036, 0.0);
    H1.add_shell(chemist::ShellType::pure, 0,
                 std::vector<double>{0.15432897, 0.53532814, 0.44463454},
                 std::vector<double>{3.42525091, 0.62391373, 0.16885540});

    // Atomic basis set for the second hydrogen atom
    chemist::AtomicBasisSet<double> H2("STO-3G", 1, -1.6380335020342418,
                                       1.1365568803584036, 0.0);
    H2.add_shell(chemist::ShellType::pure, 0,
                 std::vector<double>{0.15432897, 0.53532814, 0.44463454},
                 std::vector<double>{3.42525091, 0.62391373, 0.16885540});

    bs.add_center(O);
    bs.add_center(H1);
    bs.add_center(H2);

    // TUTORIAL
    //
    // A couple of things to note:
    //
    // 1. Classes associated with the ``AOBasisSet`` are templated on the type
    //    used to store the basis set parameters (the contraction coefficients,
    //    primitive exponents, and Cartesian coordinates of the center).
    // 2. The ``Center``'s ctor takes the Cartesian coordinates of the center,
    //    in Bohr.
    // 3. Inputs to ``add_shell`` are:
    //    - Is the shell pure or Cartesian?
    //      - An enum is used to avoid some technical corner cases with``bool``
    //    - The total angular momentum of the shell (i.e. 0 for an "s"-shell, 1
    //      for a "p"-shell, etc.)
    //    - The contraction coefficients for the Gaussian primitives (in a.u.)
    //    - The exponents for the Gaussian primitives (in a.u.)
    //
    // Now that we have an ``AOBasisSet`` instance we can do basic inspections:

    assert(bs.size() == 3); // Gives the number of centers
    assert(bs[0] == O);     // Center 0 is the AO basis set for the oxygen atom
    assert(bs[1] == H1); // 1 is the AO basis set for one of the hydrogen atoms
    assert(bs[2] == H2); // and 2 is the AO basis set for the other hydrogen
    assert(bs.n_shells() == 5);             // # of shells
    assert(bs.n_aos() == 7);                // # of AO basis functions
    assert(bs.n_unique_primitives() == 15); // # of unique Gaussian primitives
    // Get the contraction coefficient of the first unique primitive
    assert(bs.unique_primitive(0).coefficient() == 0.15432897);

    // TUTORIAL
    //
    // The ``AOBasisSet`` class is designed to be used in an object-oriented
    // manner. What this means is that the overall ``AOBasisSet`` is thought of
    // as being a set of ``Center`` instances. Each ``Center`` instance is a set
    // of ``Shell`` instances. Each ``Shell`` instance is a "set" of
    // ``ContractedGaussian`` instances. And finally each ``ContractedGaussian``
    // instance is a set of ``Primitive`` instances.
    //
    // This means that we can loop over the entirety of the basis set like:
    std::size_t counter = 0;
    for(chemist::AtomicBasisSet<double> center_i : bs) {
        for(chemist::Shell<double> shell_j : center_i) {
            for(chemist::ContractedGaussian<double> ao_k : shell_j) {
                for(chemist::Primitive<double> prim_l : ao_k) {
                    // Do something with the primitive
                    ++counter;
                }
            }
        }
    }
    assert(counter == 21); // Note: not all primitives were unique (vide infra)

    // TUTORIAL
    //
    // This object-oriented approach leads to a more natural encapsulation of
    // the data and an (arguably) easier to read code where inquiries for data
    // are localized to objects which actually own the data. Nonetheless there's
    // a couple of points to make about the above:
    //
    // 1. It's common for the same ``ContractedGaussian`` to be used for each
    //    AO in the shell. In such a case, the third loop (the one over AOs in
    //    ``shell_i``) actually returns the same AO multiple times.
    // 2. As a result of point 1, the loop over primitives will not be over
    //    unique primitives, but will return the same primitives for each AO in
    //    a shell.
    // 3. The design of the classes makes it such that, if need be, we can later
    //    come back and relax these assumptions to allow for different CGTOs in
    //    a shell (and in the process likely introduce a proper set of AO
    //    classes which would combine one, or more, ``ContractedGaussian``
    //    instances with a prefactor that depends on the purity of the AO).
    // 4. Because the removal of redundancy in the basis set is so common, many
    //    of the ``AOBasisSet`` member functions come in two flavors (or can be
    //    made to when the need arises). The "normal" flavor considers all
    //    elements, regardless of uniqueness, whereas the ``unique_`` prefixed
    //    flavor only considers the non-redundant elements (see the next code
    //    block for example).
    //
    // That said, the above nested loop structure is a bit of an eyesore. For
    // the most part, anytime we use the ``AOBasisSet`` we typically want to use
    // it at a specific coarseness. For example we may want to loop over shells,
    // AOs, or unique primitives, . The AOBasisSet class makes this easy. For
    // example to loop over unique primitives in the basis:
    counter = 0;
    for(chemist::Primitive<double> prim_i : bs.unique_primitives()) {
        // Do something with the i-th unique primitive
        ++counter;
    }
    assert(counter == 15); // Only looped over the unique Primitives

    // TUTORIAL
    //
    // The class hierarchy makes it possible to do more advanced flattenings as
    // well such as looping over unique_primitives in each non-unique AO:
    counter = 0;
    for(chemist::ContractedGaussian<double> ao_i : bs.aos()) {
        for(chemist::Primitive<double> prim_j : ao_i) {
            // Use the j-th unique primitive from the (non-unique) i-th AO
            ++counter;
        }
    }
    assert(counter == 21); // "Double counted" some of the primitives

    // TUTORIAL
    //
    // Or we may want to loop over the AOs, regardless of whether they have the
    // same ``ContractedGaussian`` instance or not.
    counter = 0;
    for(chemist::ContractedGaussian<double> ao_i : bs.aos()) {
        // Do something with the (not necessarily unique) i-th AO
        ++counter;
    }
    assert(counter == 7); // Note: loops over the 3 non-unique "p"-orbitals

    // TUTORIAL
    //
    // At this point it is worth taking a brief aside. Up until this point we
    // have manually specified the type for our foreach loop. For example we
    // wrote
    // ``for(chemist::ContractedGaussian<double> ao_i : bs.aos())``
    // and not the much shorter (and arguably preferred) syntax:
    // ``for(auto&& ao_i : bs.aos())``. This was done for the intent of
    // clarity. The shorter syntax works; however, the type is probably not what
    // you expect:

    for(auto&& ao_i : bs.aos()) {
        // The actual type of ao_i (without references)
        using ao_i_type = std::remove_reference_t<decltype(ao_i)>;
        // We acted like it was this type,
        using cgto_type = chemist::ContractedGaussian<double>;
        // but as you can see it's not
        static_assert(std::is_same_v<ao_i_type, cgto_type> == false);
        // It's actually this type
        using actual_type = chemist::ContractedGaussianView<double>;
        static_assert(std::is_same_v<ao_i_type, actual_type>);
    }

    // TUTORIAL
    //
    // Some explanation is in order. Associated with each of: ``Primitive``,
    // ``ContractedGaussian``, and ``Shell`` is a corresponding "view" variant
    // (the same class name plus ``View``, like ``ContractedGaussianView``
    // above). The different types signify the memory ownership of the object.
    // More specifically the "non-view" variants own their own state,
    // whereas the "view" variants alias the state of the parent container.
    //
    // Hence all of our foreach loops so far have copied state from ``bs`` as is
    // easily shown:
    for(chemist::Primitive<double> prim_i : bs.unique_primitives()) {
        assert(prim_i.coefficient() != 0.0); // Wasn't 0 to start with
        prim_i.coefficient() = 0.0;
        assert(prim_i.coefficient() == 0.0); // We really did change it
    }

    for(chemist::PrimitiveView<double>&& prim_i : bs.unique_primitives()) {
        assert(prim_i.coefficient() != 0.0); // Isn't 0
    }

    // TUTORIAL
    //
    // We can also prove that that the views really are aliases by modifying
    // ``bs``:
    for(chemist::PrimitiveView<double>&& prim_i : bs.unique_primitives()) {
        assert(prim_i.coefficient() != 0.0); // Still isn't 0
        prim_i.coefficient() = 0.0;          // Set it to 0
    }
    for(chemist::PrimitiveView<double>&& prim_i : bs.unique_primitives()) {
        assert(prim_i.coefficient() == 0.0); // It's 0 now!!!
    }

    // TUTORIAL
    //
    // It is worth noting that the views combined with the syntax of a foreach
    // loop do lead to a gotcha where, despite the conversion being defined, we
    // can not loop over read-only references to non-views. Namely do **not** do
    // the following:
    //
    // .. code-block: c++
    //
    //    for(const chemist::Shell<double>& shell_i : bs.shells()) {
    //       // Intent is to loop over read-only shells
    //    }
    //
    // This code will trip an assert in debug mode (and likely segfault in
    // release). The problem is this gets compiled to something like:
    //
    // .. code-block:: c++
    //
    //    {
    //
    //    auto && __range = bs.shells() ;
    //    auto __begin = __range.begin() ;
    //    auto __end = __range.end() ;
    //    for ( ; __begin != __end; ++__begin) {
    //
    //        const chemist::Shell<double>& shell_i = *__begin;
    //        loop_statement
    //
    //    }
    //
    // Inside the body of the loop the iterator will return a view, which is
    // then cast to a read-only reference, and the view is freed, leaving
    // ``shell_i`` as invalid reference. The equivalent view code will work just
    // fine:
    for(const chemist::ShellView<double>&& shell_i : bs.shells()) {
        // Do something with the read-only shell
    }

    // TUTORIAL_START_SKIP
} // ADD_TEST
} // namespace tutorial
//// TUTORIAL_STOP_SKIP
