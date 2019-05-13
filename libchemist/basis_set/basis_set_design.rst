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



Gaussian Basis Set
------------------

Our current implementation is heavily tailored to the traditional view of
Gaussian basis sets and defines the following entities:

- ``AOPrimitive`` holds the coefficient and exponent
- ``AOBasisFunction`` has nothing, just present to make the loops line up
- ``AOShell`` has collection of prims, pure-ness, and angular momentum
- ``AOCenter`` has a collection of shells, and a center

Thinking of ``AOPrimitive`` as the most fundamental, and ``AOCenter`` as the
most general, this structure factors each property into the most derived class
such that all classes below it have the same value for that property. This is
certainly not general (for example there's no physical reason to force all
primitives in a shell to have the same exponents and coefficients), but rather
maps to how most integral libraries define their APIs. The actual AOBasisSet
object is templated on the type of shells it returns, thus any AO shell
satisfying the API will work

