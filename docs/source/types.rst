***********************
Types of Orbital Spaces
***********************

Libchemist supports a lot of different types of orbital spaces. This page walks
through those spaces and compares and contrasts them.

.. note::

   Many of the "classes" on this page are actually typedefs of more fundamental
   classes. This point is an implementation detail. Readers are encouraged to
   think of them as true classes as it makes things conceptually easier than
   disecting very heavily templated types (C++ concepts where are you?)

Why Not a Single Class?
=======================

At first it may be tempting to try to unify the various orbital space types into
a single type just to simplify things. However, the slight differences among the
classes actually have important algorithmic considerations. For example,
``DerivedSpace`` is for orbitals that are linear combinations of another orbital
space. ``CanonicalSpace`` is a subclass of ``DerivedSpace`` for orbitals which 
not only are linear combinations of other orbitals, but also diagonalize the 
Fock matrix. While all ``CanonicalSpace`` instances are also ``DerivedSpace``
instances, not all ``DerivedSpace`` instances are ``CanonicalSpace`` instances.
Algorithmically, if you need to use the orbital energies you need to take a
``CanonicalSpace`` instance, but if you are only going to make use of the
transformation coefficients taking a ``DerivedSpace`` instance is fine. By
having different types for these scenarios we get compile-time errors when we
try to use a ``DerivedSpace`` when we need a ``CanonicalSpace``. If we went with
a single orbital space class, the alternative would be to add a member function 
``has_orbital_energies()`` (or the like) and throw if the result is false. In
general we always prefer compile-time errors to runtime errors because the 
latter waste time (you have to run the program to learn you made a mistake).

Dense Versions
==============

This section looks at the "dense" orbital spaces. Strictly speaking dense 
orbital spaces can have sparsity, but that sparsity is restricted to being
block sparse. Dense orbital spaces use ``type::tensor`` for all tensorial
quantities. This should be contrasted with the sparse orbital spaces which have
at least one ``type::tensor_of_tensors`` tensorial quantity. Sparse orbital
spaces are signicantly more involved and are covered in the next section.

BaseSpace
---------

This is the base class common to all orbital spaces. It is basically an abstract
base class which defines some methods common to all orbital spaces.

AOSpace
-------

``AOSpace`` instances tend to be the most fundamental spaces. In addition to
being ``BaseSpace`` instances they also contain the parameters for the atomic
orbitals.

DerivedSpace
------------

``DerivedSpace`` instances are linear combinations of orbitals in another space.
The ``DerivedSpace`` class is actually templated on the type of this other 
space; however, in most cases the other space is an ``AOSpace`` instance 
(*i.e.*, the orbitals in the derived space are linear combinations of AOs). This
design makes it possible to chain multiple spaces together, for example by 
having a ``DerivedSpace`` which is a linear combination of another 
``DerivedSpace``, which is itself a linear combination of an ``AOSpace``. If
your algorithm's design depends on the intermediate ``DerivedSpace`` being
present then you'll get a compile time error if the user folded the two 
transformations together since that results in a different type.

CanonicalSpace
--------------

``CanonicalSpace`` instaces are subclasses of ``DerivedSpace`` instances (we
ignore the possibility that the ``AOSpace`` can be a canonical space, which is
only possible for hydrogenic atoms). In addition to being linear combinations of
another orbital space, the ``CanonicalSpace`` also diagonalizes the Fock matrix
and thus we can associate orbital energies with each orbital.

Sparse Versions
===============

Sparse orbital spaces have at least one tensorial quantity which is of type
``type::tensor_of_tensors``. They may derive from either ``BaseSpace`` or
``DependentBaseSpace``. This section assumes you are familiar with the sparse 
map concept in the context of sparsity (TODO: link to concept).

DependentBaseSpace
------------------

This is the equivalent of ``BaseSpace`` for any orbital space which also has a
sparse map from its orbitals to the independent space. The decision to have
``DependentBaseSpace`` be its own base class and not inherit from ``BaseSpace``
comes from the fact that classes that inherit from ``DependentBaseSpace``
actually behave like a set of orbital sets (*i.e.*, it contains many sets of
orbitals) and not a single set. Thus questions like "how many orbitals?" become
fuzzy since you have to also specify which of the orbital sets you are 
referring to.

SparseAOSpace
-------------

Same as the ``AOSpace`` class except that it inherits from 
``DependentBaseSpace`` instead of ``BaseSpace``. The result is that this class
describes domains of AOs associated with an independent space.

SparseIndependentSpace
----------------------

The ``SparseIndependentSpace`` inherits from ``BaseSpace`` (*i.e.* it does not
include a sparse map from itself to the independent space since it is the
independent space and such a map would be a trivial mapping) and is an orbital
space which is a linear combination of another space. Compared to the dense
``DerivedSpace``, ``SparseIndependentSpace`` differs in that it is written as a
linear combination of a ``SparseAOSpace`` and not an ``AOSpace``. The result is
that the transformation coefficients are of type ``type::tensor_of_tensor`` and
not of type ``type::tensor``. The mapping from an independent orbital to its
AO domain is stored in the ``SparseAOSpace`` member. This space is typically
used to model spaces spanned by localized molecular orbitals (LMOs).

SparseDerivedSpace
------------------

The ``SparseDerivedSpace`` is similar to the ``SparseIndependentSpace`` except
that it inherits from ``DependentBaseSpace``. This means that 
``SparseDerivedSpace`` has a sparse map associated with it which maps from the
independent orbitals to domains of the orbitals described by the 
``SparseDerivedSpace``. For example one use of this class is for project-atomic
orbitals (PAOs), which are a set of localized virtual oribtals. For the PAOs
the aforementioned sparse map maps from the independent space (usually LMOs or
tuples thereof) to domains of PAOs. The ``SparseDerivedSpace`` class assumes
that the transformation coefficients go from a ``SparseAOSpace`` as will be the
case for most (all?) linear-scaling theories. 

.. note:: 
   
   Should the need arise to have the from space be dense, a new class will be 
   needed (strictly speaking it would just be a typedef that uses ``AOSpace`` 
   instead of ``SparseAOSpace``).

SparseIndependentCanonicalSpace
-------------------------------

The ``SparseIndependentCanonicalSpace`` derives from ``SparseIndependentSpace``
and additionally associates "energies" with the independent orbitals.




