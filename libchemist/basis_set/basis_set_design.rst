Basis Set Design
================

TL;DR We can write a super simple ``AOBasisSet`` class if we're allowed to
duplicate the center's coordinates, angular momentum, pure-ness, exponents,
and contraction coefficients per basis function. However, such a design does not
incorporate easily into the traditional view of Gaussian basis sets, hence you
get this templated mess to take into account the myriad of different ways of
thinking about the basis set.

While conceptually an atomic-orbital basis set is a simple thing --- a set of
point-centered atomic-orbitals, each of which is defined by a few parameters ---
the number of restrictions that developers place on the basis set makes it
considerably more complicated. The next section outlines the considerations that
must go into the design of the final object(s). For simplicity we assume that we
are talking about a Gaussian-based basis set, but it's straightforward to
generalize this section to other point-centered basis functions.

Considerations
--------------

The primary considerations that a basis set object must satisfy are:

- Filling is shell based
- Centers and parameters must be stored contiguously
- Must be viewable by centers, shells, basis functions, and even primitives

The first consideration means that users want to fill the basis set by providing
shells. Each shell has a Cartesian coordinate where it is centered, an angular
momentum, whether the angular part is Cartesian or spherical, and a collection
of parameters defining the primitive Gaussians in it. Hence building the
basis set needs to look like:

.. code-block:: c++

    AOBasisSet bs{AOShell{x1, y1, z1, l1, pure1, {params1_1, params1_2, ...}},
                  AOShell{x2, y2, z2, l2, pure2, {params2_1, params2_2, ...}},
                  ...};

The second consideration means we can't simply store the data inside the basis
set as ``std::vector<AOShell>``, but rather need to store it like:

.. code-block:: c++

    class AOBasisSet {
        ...
    private:
        std::vector<double> m_coods_; //n centers long
        std::vector<int> m_l_; //n shells long
        std::vector<bool> m_pure_; // n shells long
        std::vector<double> m_exponent_;// n primitives long
        std::vector<double> m_coefficent_; // n primitives long
    }

This means we need to copy the data out of the input shells and into the arrays.
This more-or-less stores the data by shell, with some compression on the centers
that takes into account that there are usually multiple shells on a center. The
data could arguably be compressed more by storing the data per atom type, but
we're not going to worry about that for now.

The third consideration has us wanting to view the basis set as a set of
centers, or a set of shells, or...The problem is that with the second
consideration we can't just store the basis set as a set of basis functions (if
we could then a shell or a center is nothing more than a pair of iterators to
the first and last basis functions in the shell or center).

Solution
--------

In almost every electronic structure program I am aware of, the first two
considerations are satisfied, but an object-oriented solution to the third
consideration is not accomplished. Instead one typically tabulates jump tables,
which allow you to map from a given center to a given shell, or basis function,
or primitive. This leads to index manipulations that get quite ugly and are not
compatible with most STL algorithms.

The canonical object-oriented solution to this problem is views. Basically you
create proxy objects on-the-fly that allow you to interact with the basis set
as if it really had given you say a list of centers, or a list of basis
functions. Superficially views just wrap pointers. The tricky part about views
is getting the const-ness correct and allowing the user to interact with the
data correctly. The latter is by far the harder problem and is easier to explain
by example. Say the class ``BasisFxnView`` creates basis function proxy objects.
Then looping over a basis set by basis function, and changing every odd numbered
basis function's X-coordinate to 2.3 looks like:

.. code-block:: c++

    AOBasisSet bs; //Assume we got this from somewhere
    std::size_t i = 0;
    for(auto bf : BasisFxnView(bs))
        if(i % 2) bf.center()[0] = 2.3;

Let us ignore the question of why someone would want to do such a thing and
instead focus on the problem with this code. Here the user presumably expected
all of the even numbered basis functions' X-coordinates to remain untouched. The
problem is since we don't store the X-coordinate of each basis function
individually, but rather alias them for an entire center, the code above changes
the X-coordinate for every center that has an odd numbered basis function (so
probably all of them...). There's probably at least two ways to handle this
scenario: error if the user tries to write to an aliased piece of data or modify
the basis set we are viewing to accommodate the user's change. For a general
manipulation it is impossible to modify the state of the basis set to
accommodate the user's change while respecting the internal storage arrangement.
For this reason we pursue the error approach.

Design
------

Our current implementation is heavily tailored to the traditional view of
Gaussian basis sets and defines the following entities:

- ``Gaussian`` holds the coefficient, and exponent
- ``GaussianAO`` has a series of ``Gaussian``s
- ``AOShell`` has a basis function, the pure-ness, and the angular momentum
- ``AOCenter`` has a set of shells and a center

The ``Gaussian`` class is the most fundamental and ``AOCenter`` is the least
fundamental. Each layer is conceptually a set of the layer below it. For
example the ``GaussianAO`` is viewed as a series of ``Gaussian``s. Each
layer may introduce additional data; for example the ```` adds . This is data that can be viewed as being
factored out of the layer below it. For example, the ``AOShell`` introduces the
pure-ness of the shell (spherical or Cartesian), but in reality each
``AOBasisFunction`` also has the same purity. Users are allowed to change a value at the level it resides at or
higher; for example changing the angular momentum through an ``AOShell`` or an
``AOCenter`` is allowed, but changing it through an ``AOBasisFunction`` is not.

While this sounds like a lot of error checking we can actually get the compiler
to take care of all of it for us. The trick to doing this is proper template and
const utilization. For simplicity consider a ``Gaussian`` and a
``GaussianAlias`` class which only has two data members ``coef`` and ``exp``,
which respectively are the contraction coefficient and exponent of the primitive.
A naive implementation would look like:

.. code-block:: c++

    struct Gaussian {
        double coef;
        double exp;
    };

    struct GaussianAlias {
        double* coef;
        double* exp;
    };

The problem with these implementations is that we can either write to both
members' values or neither (given a ``const Gaussian`` or
``const GaussianAlias`` no values are writable). While that's fine for the
contraction coefficient and exponent since they always appear together. It's
not okay for something like the angular momentum, which will get factored out
to a much higher level. If we template on the type of each member we end up
with:

.. code-block:: c++

    template<typename CoefType, typename ExpType>
    struct Gaussian {
        CoefType coef;
        ExpType exp;
    };

    template<typename CoefType, typename ExpType>
    struct GaussianAlias {
        CoefType* coef;
        ExpType* exp;
    };

Now declaring ``Gaussian<double, const double>`` would allow the coefficient to
be written to, but not the exponent (a ``const Gaussian<double, const double>``
instance would still be immutable). As a slight technical aside we can reduce
the boiler plate further by pulling the pointer into the template type
parameter, *e.g.*, ``GaussianAlias<double, double>`` is just a typedef of
``Gaussian<double*, double*>`` (we have to do a bit of template meta-programming
to make sure we dereference when appropriate, but otherwise this leads to no
additional difficulties). To summarize so far ``Gaussian`` is a glorified struct
that is templated on the type of every member and it contains a field for the
center's coordinates, the angular momentum, the purity, the exponent, and the
contraction coefficient and thus conceptually it looks like:

.. code-block:: c++

    template<typename CenterType, typename AmType, typename PureFlag,
             typename ExpType, typename CoefType>
    struct Gaussian{
        CenterType center
        ExpType    exp;
        CoefType   coef;
    };

Having this many template type parameters is necessary in order to allow any
field to be read-only, or an alias, but it makes the type of the signature of
the ``Gaussian`` class messy. A cleaner solution is decouple the type choices
from the ``Gaussian`` class using a traits class. We define ``BasisSetTraits``:

.. code-block:: c++

    template<typename CenterType, typename AmType, typename PureFlag,
             typename ExpType, typename CoefType>
    struct BasisSetTraits{
        using center_type = CenterType;
        using am_type     = AmType;
        ...
    };

and our ``Gaussian`` class becomes:

.. code-block:: c++

    template<typename Traits>
    struct Gausian{
        typename Traits::center_type center;
        ...
    };

While this may look like all we did was shift the template parameters to a
different class. As we'll see in a second, all of the other classes in this
hierarchy also need those parameters, so we've in essence factored these
parameters out of all of those classes as well.

The next step is to declare the ``AOBasisFunction`` class. For a traditional
integral library this class factors out of each ``Gaussian`` everything
besides the exponent and contraction coefficient. Thus our ``AOBasisFunction``
is conceptually:

.. code-block:: c++

    template<typename Traits, typename PrimType>
    struct AOBasisFunction{
        using prim_type = PrimType;
        typename Traits::center_type center;
        typename Traits::am_type     l;
        typename Traits::pure_type   pure;
        std::vector<prim_type> prims;
    };

As you can see if we hadn't defined the traits class we would have ended up with
something like:

.. code-block:: c++

    template<typename CenterType, typename AmType, typename PureFlag,
             typename ExpType, typename CoefType, typename PrimType>
    struct AOBasisFunction{
        ...
    };

We've taken the type of the ``Gaussian`` as a template parameter for simplicity,
the reality is that our implementation mandates how the ``Gaussian`` class
takes each parameter (in order to work as coded, the center has to be a
read-only alias to the one stored in the ``AOBasisFunction`` class, as does the
angular momentum and the purity.

Our ``AOBasisFunction`` class assumes we're using a traditional integral
library, but what if we didn't want to factor ``l`` o

Which is just a specialization. Hence with the current setup we are able to
arbitrarily factor properties between the primitive and the basis function (we
also can store the exponents and coefficients in the ``AOBasisFunction``
instance and have the primitives hold pointers). Having to specialize the class
is a bit messy and a cleaner solution is to use CRTP. Namely we change above to:

.. code-block:: c++

   template<typename ParentType>
   struct AOBasisFunctionBase {
       //Get the
       //Get the i-th primitive
       auto& operator[](std::size_t i){ return ParentType::at(i); }
   }

Next is the ``AOShell`` conceptually the ``AOShell`` factors out of each basis
function the coordinates, the angular momentum, the purity, and the set of
primitives, leaving the angular momentum component. Since we are not storing
the angular momentum component our ``AOShell`` class is nothing more than:

.. code-block:: c++

    template<typename CenterType, typename AmType, typename PureFlag,
             typename ExpType, typename CoefType,
             typename BasisFxnType = AOBasisFunction<CenterType, AmType, PureFlag,
                                         ExpType, CoefType,
                                         Gaussian<const CenterType*, const AmType*,
                                         const PureFlag*, ExpType, CoefType>>
    struct AOShell{
        BasisFxnType fxn;
    };

Finally we have the ``AOCenter`` class which looks like:

.. code-block:: c++

    template<typename CenterType, typename AmType, typename PureFlag,
             typename ExpType, typename CoefType, typename ShellType
    struct AOCenter{
        CenterType enter;
        std::vector
    };

Where for sanity I didn't bother showing the default ``AOShell`` type. It's
worth noting that we can introduce general contractions into this
hierarchy simply by introducing a class ``GeneralContraction`` that factors out
of ``AOShell`` the exponents common to each shell and using the
``GeneralContraction`` class as the shell type for ``AOCenter``.

The template type of the final ``AOCenter`` is super nasty, so how do we clean
it up? The eaiest way is to use a "traits" class:

.. code-block:: c++


    template<typename T,
             typename CenterType = std::array<T, 3>,
             typename AmType = std::size_t,
             typename PureType = bool,
             typename ExpType = T,
             typename CoefType = T>
    struct GaussianTraits {
        using center_type = CenterType;
        using am_type     = AmType;
        using pure_type   = PureType;
        using exp_type    = ExpType;
        using coef_type   = CoefType;
    }

and template the ``Gaussian`` class like:

.. code-block:: c++

    template<typename T, typename traits = GaussianTraits<T>>
    struct Gaussian{
    };

and then you declare similar traits classes for all the other traits. By
factoring the traits class into three traits classes we
