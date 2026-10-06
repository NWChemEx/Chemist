.. Copyright 2026 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

.. _designing_the_ao_class_hierarchy:

################################
Designing the AO Class Hierarchy
################################

This section contains notes on the classes which represent an AO basis set.

*************************
What are we representing?
*************************

:ref:`ao_basis_set_background` describes an AO as a layered object: primitive
Gaussians :math:`\chi_p` combine into a contracted Gaussian :math:`G`, which is
combined with a Cartesian polynomial to give a Cartesian AO. Optionally, a
spherical AO can be formed by taking a linear combination of Cartesian AOs.
AOs (Cartesian or spherical) sharing a contracted Gaussian and a total angular
momentum form a shell, shells sharing a center are an atomic basis set, and the
collection of all atomic basis sets is the molecular basis set.

The classes in this component mirror that structure one-for-one, so that each
concept in the background page has a class and each relationship between
concepts is a relationship between classes.

**************************
Why make the AOs explicit?
**************************

Historically the AOs in a shell have been stored implicitly. A shell records a
total angular momentum, a purity flag, and the parameters for the contracted
Gaussian.

This leads to several problems:

First, it leaves the count of AOs and the identity of AOs in two different
places. A shell can say how many AOs it has, but not what the third one *is*.
Every consumer which cares about individual AOs --- and real-space evaluation,
property calculations, and anything reporting per-orbital quantities all care
--- ends up reimplementing the mapping from offset to angular momentum, and
each implementation is an opportunity to order them differently.

Second, it makes normalization inexpressible. As
:ref:`designing_basis_set_normalization` shows, one of the three factors
depends on the individual Cartesian powers, so it differs between the AOs of a
single shell. With no object per AO there is nowhere to put that factor, which
is precisely why the prevailing convention drops it.


.. _ao_hierarchy_considerations:

*********************************
AO Class Hierarchy Considerations
*********************************

Topics in this section were considered in designing the class hierarchy and
ultimately were addressed by the design.

.. _aoh_explicit_aos:

Explicit AOs
   An AO should be an object, not an offset.

   - It should be possible to obtain the :math:`n`-th AO of a shell, of an
     atomic basis set, or of a molecular basis set, and to ask that object
     about itself.
   - An AO should be able to report its own angular momentum: the Cartesian
     powers :math:`(i,j,k)` if it is Cartesian, the component :math:`m_\ell`
     if it is spherical.

.. _aoh_two_kinds_of_ao:

Two kinds of AO, one interface
   Following from :ref:`aoh_explicit_aos`, Cartesian and spherical AOs are not
   the same kind of object, but most code does not care which it has.

   - A Cartesian AO is a contracted Gaussian paired with a monomial. A
     spherical AO is a linear combination of the Cartesian AOs of its shell.
     They are built differently and carry different state.
   - Nevertheless both are AOs: both have a center, a total angular momentum, a
     normalization constant, and a value at a point. Code wanting only those
     should not have to know which kind it holds.
   - So the two need a common base, with the differences confined to the
     derived types.

.. _aoh_composition:

An AO is built by composition
   Following from :ref:`aoh_two_kinds_of_ao`, each kind of AO is built by
   pairing something shared with an index that selects one function out of it.

   - A Cartesian AO pairs a contracted Gaussian with the powers
     :math:`(i,j,k)`.
   - A spherical AO pairs the Cartesian shell of its angular momentum with the
     component :math:`m_\ell`.
   - Both pairings are constrained to agree on :math:`\ell`.

.. _aoh_shared_radial:

The shared part is shared, not copied
   Following from :ref:`aoh_composition`, the first half of each pairing is
   common to a whole shell of AOs.

   - Materializing the AOs must not copy it once per AO. A shell of :math:`f`
     functions would otherwise hold ten copies of the same exponents, and a
     spherical shell would hold seven copies of the same Cartesian shell.
   - More importantly, the copies could drift. Editing a shell's exponents must
     change every AO built on it, because physically they are one function.

.. _aoh_transform_coupling:

The spherical transform assumes a convention
   Following from :ref:`aoh_composition`, a spherical AO is defined by
   coefficients multiplying Cartesian AOs, and those coefficients are not
   convention-free.

   - The standard transformation coefficients :cite:`schlegel1995transformation`
     have :math:`N^{AO}_{ijk}` built into them, so they assume the Cartesian AOs
     they multiply are normalized only up to :math:`N^{G}`.
   - Handing them Cartesian AOs which are individually normalized applies
     :math:`N^{AO}_{ijk}` twice, which is silent and wrong.
   - Whatever owns the transform must therefore also know, and be able to
     guarantee, the convention of the Cartesian AOs it is transforming.

.. _aoh_angular_momentum_storage:

Angular momentum is stored once
   :ref:`angular_momentum_in_primitive` establishes that a primitive needs
   :math:`\ell` in order to normalize itself. Every primitive in a contraction
   shares that :math:`\ell`.

   - Storing :math:`\ell` in each primitive would duplicate it, with the usual
     risk of the copies disagreeing.
   - It should be stored once per contraction, with primitives reaching it
     rather than owning it. A primitive which is not part of a contraction has
     nothing to reach, and must own its own.

.. _aoh_center_ownership:

The center is stored once
   All the AOs on a center are centered on the same point, and that point
   moves when the atom does.

   - The center should be stored once per center, not once per primitive.
   - Lower levels of the hierarchy should see it rather than own it, so that
     moving the center moves everything built on it.

.. _aoh_ordering:

AO ordering is part of the design
   Tensors in an AO basis inherit their index ordering from the basis set, so
   two tensors are only compatible if their bases agree on the order of the
   AOs.

   - The order in which a shell enumerates its AOs must be defined, documented,
     and stable.
   - It must match what the integral libraries expect, since a reordering
     applied to only one of the two would be silent.
   - There is no single right answer. Codes disagree, and libint alone can be
     built against several mutually incompatible choices, so the design must
     accommodate more than one rather than picking one and hard-coding it.
   - Because a mismatch is silent, the ordering is worth making visible to the
     type system rather than leaving it as a runtime property to be checked, or
     not checked, by each consumer.

.. _aoh_purity_dispatch:

Purity and ordering are independent
   Following from :ref:`aoh_two_kinds_of_ao` and :ref:`aoh_ordering`, a shell
   has both a purity and an ordering, and the two are separate questions.

   - A great deal of code needs to know whether a shell is Cartesian or
     spherical without caring what order its AOs are in. A spherical AO needs a
     Cartesian shell, but sums over all of its AOs. Code applying the
     Cartesian-to-spherical transformation, or choosing an integral kernel by
     purity, needs the purity but not the order.
   - Such code should be able to require a purity without also having to name
     an ordering, and, having required one, should get the API that purity
     implies: the powers :math:`(i,j,k)` of a Cartesian shell, or the
     :math:`m_\ell` of a spherical one.
   - Code which does not know the purity until runtime, e.g., code reading a
     basis set whose purity is chosen by the user, must still be able to hold
     a shell, and to recover its purity in one checked step.

.. _aoh_normalization_placement:

Each level owns its normalization factor
   :ref:`designing_basis_set_normalization` factors the normalization constant
   into one term per layer of the hierarchy.

   - Each class should own the factor its layer introduces, so that the
     factorization and the class structure agree.
   - The convention in force is a property of a shell, since a shell is the
     smallest object owning a complete set of AOs sharing a radial part.

.. _aoh_value_view:

Value and view semantics
   Following from :ref:`aoh_shared_radial`, most of what a caller handles will
   be an alias into something larger rather than an independent object.

   - Every class in the hierarchy needs both an owning form and an aliasing
     form, with the same API.
   - The shared API should be written once rather than once per form, and
     should not cost a virtual call, since which form a caller holds is always
     known at compile time.

   This is the same pattern, and the same reasoning, as
   :ref:`designing_the_point_component`.

.. _aoh_flattening:

Flattening
   Much of the infrastructure which consumes basis sets is function-based and
   operates on flat arrays of primitives.

   - Each level of the hierarchy should be able to present the primitives
     beneath it as a flat sequence, regardless of the nesting.

Out of Scope
============

Topics in this section were considered, but do not play a role in the current
design.

General contractions
   The design assumes segmented contractions: each contracted Gaussian owns its
   primitives, and shells do not share them. General contractions would need a
   class parallel to ``AOShellBase`` which is keyed into the sharing.

Mixed purity within a shell
   A shell is either Cartesian or spherical. Shells which mix the two are not
   represented.

Effective core potentials
   ECPs are basis-set-specific state which would most naturally attach to
   ``AtomicBasisSet``, but are not represented.

Non-Gaussian AOs
   The hierarchy is specific to Gaussians. Slater or numerical orbitals would
   need their own radial types, though the radial/angular split and the AO
   layer above it would carry over.

Vector space behavior
   These classes will also participate in the ``quantum_mechanics`` component,
   so that matrix elements can be requested over them. That is the subject of a
   separate page and is not designed here.

*****************************
AO Class Hierarchy Design
*****************************

.. _fig_ao_hierarchy_design:

.. figure:: assets/ao_hierarchy.svg
   :align: center

   Classes comprising the AO basis set component, and how a shell shares one
   contracted Gaussian across its AOs.

:numref:`fig_ao_hierarchy_design` shows the classes and their relationships.

The radial classes
==================

``Primitive`` holds a contraction coefficient :math:`d_p`, an exponent
:math:`\zeta_p`, a center, and the angular momentum :math:`\ell` it is to be
paired with. It owns the factor :math:`N^{\chi}(\zeta_p; \ell)`.

``ContractedGaussian`` is a container of ``Primitive`` objects sharing a center
and an :math:`\ell`. It owns the factor :math:`N^{G}(d, \zeta; \ell)`, which
per :ref:`designing_basis_set_normalization` depends on the whole contraction
at once and so could not belong to any one primitive.

Per :ref:`aoh_angular_momentum_storage`, :math:`\ell` is stored by the
contracted Gaussian, and the ``Primitive`` objects a contracted Gaussian hands
out are views which reach it. A standalone ``Primitive`` value owns its own
:math:`\ell`. The same applies to the center, per
:ref:`aoh_center_ownership`: it is owned at the ``AtomicBasisSet`` level and
seen by everything below.

The AO classes
==============

Per :ref:`aoh_two_kinds_of_ao`, ``AO`` is an abstract base with two concrete
derived classes. The base provides what every AO has regardless of kind:

- the total angular momentum :math:`\ell`,
- the center,
- the contracted Gaussian it is ultimately built on,
- its normalization constant,
- its value at a point,

together with the usual ``clone`` and comparison, matching the idiom already
used by ``VectorSpace`` and ``OperatorBase`` elsewhere in Chemist. Code which
only needs those never learns which kind it holds.

``CartesianAO`` pairs a ``ContractedGaussian`` with the powers
:math:`(i,j,k)`, which is the pairing of :ref:`aoh_composition`. It reports
those powers, and it owns the factor :math:`N^{AO}_{ijk}`, which per
:ref:`designing_basis_set_normalization` is the only factor distinguishing the
members of a shell.

``SphericalAO`` pairs a Cartesian shell, ``AOShell<CartesianAO>``, with a
component :math:`m_\ell`. It reports :math:`m_\ell`, and its value is the linear
combination

.. math::

    \mu_{\ell m}(r; d,\zeta) = \sum_{i+j+k=\ell}
        c^{(ijk)}_{\ell m}\, \mu_{ijk}(r; d, \zeta)

from :ref:`ao_basis_set_background`, taken over the Cartesian AOs of that
shell. The coefficients :math:`c^{(ijk)}_{\ell m}` are a fixed function of
:math:`(\ell, m_\ell, i, j, k)` and are computed rather than stored, so a
``SphericalAO`` adds only :math:`m_\ell` to the shell it references.

The two derived classes are therefore the same shape: something shared, plus an
index selecting one function out of it. What differs is what is shared --- a
contracted Gaussian in one case, a Cartesian shell in the other.

Per :ref:`aoh_transform_coupling`, the coefficients carry
:math:`N^{AO}_{ijk}`, so they assume Cartesian AOs normalized only up to
:math:`N^{G}`. However, ``CartesianAO`` applies :math:`N^{AO}_{ijk}` itself,
``SphericalAO`` reconciles this by dividing :math:`N^{AO}_{ijk}` out of each
coefficient before applying it to a normalized Cartesian AO. Its
normalized value is

.. math::

    \mu_{\ell m}(r; d,\zeta) = \sum_{i+j+k=\ell}
        \frac{c^{(ijk)}_{\ell m}}{N^{AO}_{ijk}}\, \mu_{ijk}(r; d, \zeta),

with :math:`\mu_{ijk}` the fully normalized Cartesian AO. Its unnormalized
value applies :math:`c^{(ijk)}_{\ell m}` as-is, since the unnormalized
Cartesian AO does not apply :math:`N^{AO}_{ijk}` either. Its
``normalization_constant`` is :math:`N^{G}`: the coefficients normalize the
solid harmonic, but they are part of the angular function rather than a
constant in front of it. Should a second Cartesian convention ever be
introduced, this is where it would have to be checked.

The sum runs over every AO in the shell, so its value does not depend on the
order the shell enumerates them in. Per :ref:`aoh_purity_dispatch`,
``SphericalAO`` therefore holds its shell as an ``AOShell<CartesianAO>``: the
purity is part of that type, but the ordering is not, so it accepts a Cartesian
shell of any ordering, and a spherical shell does not compile.

Note this also means a ``SphericalAO`` reaches its contracted Gaussian through
its Cartesian shell, all of whose AOs share one. The base class accessor is
therefore well defined for both kinds.

The container classes
=====================

A shell is a container of ``AO`` sharing a total angular momentum and a purity.
The purity determines both what it contains and what it is built on:

- A Cartesian shell holds one ``ContractedGaussian`` and contains the
  :math:`(\ell+1)(\ell+2)/2` ``CartesianAO`` objects built from it.
- A spherical shell holds one Cartesian shell and contains the
  :math:`2\ell+1` ``SphericalAO`` objects built from it.

The second case nests: a spherical shell owns the Cartesian shell its AOs
transform, and that Cartesian shell owns the contracted Gaussian. So there is
still exactly one contracted Gaussian per shell however deep it sits.

Per :ref:`aoh_shared_radial`, neither level copies what it shares. Indexing a
Cartesian shell returns a ``CartesianAOView`` whose contracted Gaussian is a
``ContractedGaussianView`` aliasing the shell's one copy; indexing a spherical
shell returns a ``SphericalAOView`` aliasing the one Cartesian shell. Editing
the exponents once is visible through every AO above them.

A shell also carries the normalization convention in force, per
:ref:`aoh_normalization_placement`, and can report the product
:math:`N^{\chi} N^{G}` which integral libraries expect. For a spherical shell
this is also what :ref:`aoh_transform_coupling` requires of the Cartesian shell
underneath it.

The shell classes form three layers, each fixing one more thing in the type,
and each layer comes as an owning class and a view (see
:ref:`aoh_value_view`):

- ``AOShellBase`` and ``AOShellBaseView`` fix nothing: neither the purity nor
  the ordering is part of the type. They share an ``AOShellBaseCommon``,
  exactly as ``AO`` and ``AOView`` share ``AOCommon``. The purity is reported
  at runtime by ``is_pure`` (with ``is_cartesian`` as its negation), and
  ``size`` follows from it: :math:`2\ell+1` for a pure shell and
  :math:`(\ell+1)(\ell+2)/2` for a Cartesian one.
- ``AOShell<T>`` and ``AOShellView<T>``, with ``T`` either ``CartesianAO`` or
  ``SphericalAO``, derive from those and fix the purity. This is the layer
  described in :ref:`ao_hierarchy_purity_as_a_type`.
- The orderings, e.g. ``CCAShell<T>`` and its view, derive from those and fix
  the ordering. This is the layer described in
  :ref:`ao_hierarchy_ordering_as_a_type`.

A shell does not store the views of its AOs. Indexing builds the requested
view on demand and returns an owning pointer to it. Indexing an
``AOShellBase``, which does not know the kind of AO it holds, returns a pointer
to a polymorphic ``AOView``. Indexing an ``AOShell<T>``, or anything deriving
from one, returns a pointer to the concrete view, since the caller then knows
which kind it holds.
Either way the view aliases the shell's contracted Gaussian, so it must not
outlive the shell, and writes made through the shell are visible through it.
With nothing stored, there is nothing for copying, assigning, swapping, or
deserializing a shell to invalidate, and a copy can never hand out views of
its source. The one thing a view fixes when it is built is its angular index,
so a view obtained before :math:`\ell` changes no longer describes an AO of
the shell; index the shell again instead.

The purity is a template parameter of the concrete shell: ``CCAShell`` is
``CCAShell<CartesianAO>`` or ``CCAShell<SphericalAO>``. The two have the same
state, namely one ``ContractedGaussian``. A pure shell does not store the
Cartesian shell its AOs transform. Instead it presents one as a
``CCAShellView<const CCAShell<CartesianAO>>`` over that same contracted
Gaussian. This satisfies the nesting described above with exactly one
contracted Gaussian, and it means a spherical shell and a Cartesian shell built
from the same parameters can share every line of code except the ordering. The
``SphericalAOView`` objects a pure shell hands out alias that view.

``normalization_constant`` reports :math:`N^{G}` for either purity, with
:math:`N^{\chi}` applied per primitive, following ``CartesianAO``. There is no
convention enumerator yet, since there is only one convention.

In addition to the value/view members described below, ``as_view`` returns a
polymorphic view of a shell, of the same purity when called on an
``AOShell<T>``. This is what lets a ``SphericalAOView`` alias a Cartesian shell
without knowing its ordering. ``SphericalAO`` and ``SphericalAOView`` only
accept a Cartesian shell, so that they are given one is checked by the
compiler rather than on construction.

.. _ao_hierarchy_purity_as_a_type:

Purity as a type
================

Per :ref:`aoh_purity_dispatch`, ``AOShell<T>`` is the layer of the shell
hierarchy whose purity, but not ordering, is part of the type:
``AOShell<CartesianAO>`` holds Cartesian AOs and ``AOShell<SphericalAO>`` holds
spherical ones. A function which needs a Cartesian shell, but does not care
about the order of its AOs, takes an ``AOShell<CartesianAO>``, and every
ordering of Cartesian shell satisfies it.

Knowing the purity statically is what lets a shell answer questions which only
make sense for one purity. ``AOShell<CartesianAO>`` reports the powers
:math:`(i,j,k)` at an offset, through ``cartesian_powers``, and
``AOShell<SphericalAO>`` reports :math:`m_\ell`, through ``magnetic_index``.
Each has only the method for its purity, so asking a spherical shell for its
Cartesian powers does not compile. Which angular index sits at which offset is
the ordering, so the derived classes supply it; everything else at this layer
follows from the purity alone and is implemented once, for every ordering:

- ``is_pure`` and ``is_cartesian`` are compile-time constants,
- indexing returns a pointer to the concrete AO view, ``CartesianAOView`` or
  ``SphericalAOView``, rather than to a polymorphic ``AOView``,
- ``get_cartesian_shell`` returns a view of the Cartesian shell underneath,
  which for a Cartesian shell is the shell itself, and
- ``clone``, ``as_view``, and ``as_shell`` return pointers to the same layer,
  so that the purity is not lost in a round trip.

``AOShell<T>`` and ``AOShellView<T>`` share this API through an
``AOShellCommon``, in the same way as the other value/view pairs.

``AOShellBase`` remains for code which only learns the purity at runtime, the
main example being ``AtomicBasisSet`` (see below). Going from an
``AOShellBase`` to an ``AOShell<T>`` is the one place a purity is checked at
runtime, and is done by ``as_cartesian_shell`` or ``as_spherical_shell``
(overloaded for ``AOShellBaseView``, giving an ``AOShellView<T>``), which
return the same shell with its purity in its type, or throw if the shell does
not have that purity.

.. _ao_hierarchy_ordering_as_a_type:

Ordering as a type
==================

Per :ref:`aoh_ordering`, ``AOShell<T>`` is itself abstract, and its derived
classes are the orderings. The bases own all of the state described above and
all of the behavior which does not depend on the order; what a derived class
supplies is one thing only: the map from an offset within the shell to the
angular index at that offset. For a Cartesian shell that is the powers
:math:`(i,j,k)`, reported by ``cartesian_powers``; for a spherical shell it is
the component :math:`m_\ell`, reported by ``magnetic_index``. Which of the two a
shell has depends on its purity, so neither is part of ``AOShellBase``, where
asking for the wrong one could only fail at runtime; per
:ref:`ao_hierarchy_purity_as_a_type` they are part of ``AOShell<T>``. Code
holding an ``AOShellBase`` can still ask each AO it indexes about itself.
``SphericalAO``, the one consumer which needs the powers of a shell whose
ordering it does not know, needs them for every :math:`(i,j,k)` rather than in
any particular order, so it enumerates them itself.

``CCAShell`` implements the Common Component Architecture ordering, which is
what libint calls its *standard* ordering. Its Cartesian order is generated by
letting :math:`i` run from :math:`\ell` down to zero and, within each
:math:`i`, letting :math:`j` run from :math:`\ell - i` down to zero:

.. math::

   \ell = 1:&\quad x,\; y,\; z \\
   \ell = 2:&\quad xx,\; xy,\; xz,\; yy,\; yz,\; zz \\
   \ell = 3:&\quad xxx,\; xxy,\; xxz,\; xyy,\; xyz,\; xzz,\;
                   yyy,\; yyz,\; yzz,\; zzz

and its spherical order runs :math:`m_\ell` from :math:`-\ell` to
:math:`+\ell`.

Other orderings become other derived classes. Libint alone can be built against
several --- its GAMESS, ORCA, and BAGEL Cartesian orderings, and its Gaussian
solid harmonic ordering, which runs :math:`m_\ell` as :math:`0, +1, -1, +2,
-2, \ldots` rather than monotonically --- and each is a candidate for a class
alongside ``CCAShell``.

Making the ordering a type rather than a stored enumerator is deliberate. Per
:ref:`aoh_ordering` a disagreement about order is silent, and the whole point
of encoding it in the type is that a function which requires one ordering
cannot be handed another. A tensor built over a ``CCAShell`` basis and one
built over a GAMESS-ordered basis are not interchangeable, and that is
something the compiler can enforce rather than something each consumer must
remember to check.

The cost is that a container of ``AOShellBase`` can, in principle, hold shells
of different orderings. That is not meaningful, and ``MolecularBasisSet`` treats
it the same way it treats a mixture of normalization conventions: it can be
asked whether all of its shells agree, and the answer is part of what makes a
basis set usable with a given integral library.

``AtomicBasisSet`` is a container of shells sharing one center, and owns
the ``Point`` per :ref:`aoh_center_ownership`. It also carries the basis set
name and atomic number, which are per-center rather than per-shell because
mixing basis sets across centers is not unusual.

No shell exists as an object inside an ``AtomicBasisSet``. Per
:ref:`aoh_shared_radial` and :ref:`aoh_flattening`, the set stores the
coefficients of all of its shells in one contiguous array and the exponents in
another, together with the :math:`\ell` of each shell and the offset of each
shell into those arrays. Indexing the set builds a view of the requested shell
from that state --- a ``ContractedGaussianView`` over a slice of each array,
the shell's :math:`\ell`, and the set's center --- and returns an owning
pointer to it, exactly as indexing a shell does for its AOs. The flattened
state is equally available as the set's primitives or as the raw parameter
arrays. A shell can be added from its parameters or copied in from an existing
shell; the latter is rejected if the shell's center is not the set's. This is
motivated by the fact that this is how many legacy codes handle basis sets,
so having an internal representation mirroring this makes the basis set easier
to interface with those codes.

Every shell in a set has the same type, i.e. the same purity and ordering, but
``AtomicBasisSet`` is not templated on it. Instead the type is chosen at runtime
from two enumerations, ``ShellPurity`` and ``AOOrdering``, which select the
implementation the set holds (a PIMPL templated on the shell type), and the
set hands its shells out polymorphically, as ``AOShellBaseView``, since their
purity is only known at runtime; code which needs it statically uses
``as_cartesian_shell`` or ``as_spherical_shell`` on the result. The
implementations come in an owning and an aliasing form sharing one CRTP base,
and ``AtomicBasisSetView`` holds the aliasing one. Because that form aliases
each piece of the state separately, rather than aliasing an
``AtomicBasisSet``, a view can be built over any storage with the same layout,
which is how ``MolecularBasisSet`` hands out atomic basis sets over slices of
its own arrays.

``MolecularBasisSet`` is a container of ``AtomicBasisSet``. It reports totals
--- numbers of AOs, shells, and primitives --- and can be asked whether all of
its shells agree on a normalization convention.

As with the shells of an ``AtomicBasisSet``, no atomic basis set exists as an
object inside a ``MolecularBasisSet``. The set stores the state of all of its
atoms back to back: one coefficient array and one exponent array for every
primitive in the molecule, the :math:`\ell` of each shell, the offset of each
shell into the parameter arrays, the offset of each atom into the shells, and,
per atom, the center, basis set name, atomic number, purity, and ordering. The
centers are stored as a ``PointSet``, so they too are contiguous. Indexing the
set builds an ``AtomicBasisSetView`` over the requested atom's slice of that
state. Since the purity and ordering are stored per atom, different atoms may
hold different types of shell; the set can be asked whether its atoms agree on
purity and on ordering. The value/view split is the same as for ``PointSet``:
the molecular level has no runtime type of its own, so ``MolecularBasisSet``
and ``MolecularBasisSetView`` share a CRTP base written in terms of the state
each reaches, with no PIMPL. The one runtime dispatch, from an atom's purity
and ordering to the implementation of its view, happens when the view is
built.

Every container also offers a flattened view of the primitives beneath it, per
:ref:`aoh_flattening`.

Factoring the API
=================

Per :ref:`aoh_value_view`, every class above comes as a value/view pair whose
shared API is written once in a CRTP base, exactly as described in
:ref:`designing_the_point_component`. ``PrimitiveCommon<DerivedType>``,
``ContractedGaussianCommon<DerivedType>``, and so on implement the API in terms
of whatever the derived class provides for reaching its state; the value class
reaches into storage it owns and the view class into storage it does not.

Two kinds of polymorphism are therefore in play, and it is worth being explicit
that they are orthogonal.

The value/view distinction is resolved at compile time through the CRTP bases,
because which of the two a caller holds is always known statically and a
virtual call there would be pure overhead.

The kind distinctions --- Cartesian versus spherical at the AO layer, and which
ordering at the shell layer --- are resolved through abstract bases. So
``CartesianAO`` and ``CartesianAOView`` share one
``CartesianAOCommon<DerivedType>`` carrying their API, and ``CCAShell`` and
``CCAShellView`` share one ``CCAShellCommon<DerivedType>`` carrying theirs. In
each case the CRTP base carries the API and an abstract base carries the kind.

Which abstract base, though, is itself a value/view distinction. There are two
of them at the AO layer: ``CartesianAO`` derives from ``AO`` and
``CartesianAOView`` derives from ``AOView``, and the questions the two bases
answer are identical --- both get them from a shared
``AOCommon<DerivedType>``, which ``AO`` and ``AOView`` each instantiate with
themselves.

Keeping them separate, rather than having both concrete classes derive from
``AO``, is so that not owning the underlying state stays visible in the type
system. An ``AOView`` is cheap to copy and observes writes made through
whatever does own its contracted Gaussian; a function which requires those
properties can say ``AOView`` and get them. Only two members differ as a
result:

- ``clone`` returns the same kind of thing it was called on, so ``AO::clone``
  is a deep copy and ``AOView::clone`` returns another view of the same state.
- ``AOView::as_ao`` materializes the aliased state into an owning ``AO``,
  exactly as ``ContractedGaussianView::as_contracted_gaussian`` does one level
  down.

The abstract bases are there for different reasons at the two layers, though.
``AO`` and ``AOView`` are polymorphic because a shell genuinely does not know
which kind of AO it holds until it is built. The shell bases are polymorphic so
that the purity and the ordering reach the type system, per
:ref:`aoh_purity_dispatch` and :ref:`aoh_ordering`; code which knows statically
that it wants CCA ordering can say ``CCAShell`` and never pay for dispatch at
all.

The shell layer has one more level than the AO layer, and so one more shared
API. ``AOShellBaseCommon`` is shared by ``AOShellBase`` and
``AOShellBaseView``, and ``AOShellCommon`` by ``AOShell<T>`` and
``AOShellView<T>``. Unlike the other ``*Common`` classes, ``AOShellCommon``
derives from the base it extends, rather than sitting beside it as a CRTP base,
so that a concrete shell does not inherit the same names from three unrelated
bases. ``CCAShellCommon`` is then the CRTP base shared by ``CCAShell`` and its
view, and carries only what is specific to the CCA ordering, plus the state
access which differs between owning and aliasing.

The views compose. A ``ContractedGaussianView`` obtained from a
``CartesianAOView`` obtained from an ``AOShellView<CartesianAO>`` still aliases
the one contracted Gaussian in the original shell, and for a spherical shell
the chain simply has one more link in it.

*******
Summary
*******

:ref:`aoh_explicit_aos`
   AOs are real objects, obtainable by indexing a shell, an atomic basis set,
   or a molecular basis set, and able to report their own angular momentum:
   ``CartesianAO`` reports :math:`(i,j,k)` and ``SphericalAO`` reports
   :math:`m_\ell`.

:ref:`aoh_two_kinds_of_ao`
   ``AO`` is an abstract base providing :math:`\ell`, the center, the
   contracted Gaussian, the normalization constant, and the value at a point.
   ``CartesianAO`` and ``SphericalAO`` derive from it, so code needing only the
   common interface never learns which kind it holds.

:ref:`aoh_composition`
   ``CartesianAO`` pairs a ``ContractedGaussian`` with :math:`(i,j,k)`;
   ``SphericalAO`` pairs an ``AOShell<CartesianAO>`` with :math:`m_\ell`. Both
   check that the two halves agree on :math:`\ell`.

:ref:`aoh_shared_radial`
   A Cartesian shell stores one ``ContractedGaussian`` and a spherical shell
   stores one Cartesian shell; the AOs above them hold views, so no copies
   exist to drift.

:ref:`aoh_transform_coupling`
   Chemist's Cartesian AOs have one convention, which includes
   :math:`N^{AO}_{ijk}`, so ``SphericalAO`` divides :math:`N^{AO}_{ijk}` back
   out of each coefficient before applying it to a normalized Cartesian AO. Its
   ``normalization_constant`` is :math:`N^{G}`.

:ref:`aoh_angular_momentum_storage`
   :math:`\ell` is stored once per contracted Gaussian; the primitives it
   contains reach it rather than owning it.

:ref:`aoh_center_ownership`
   The center is owned by ``AtomicBasisSet``; everything below sees it as a view.

:ref:`aoh_ordering`
   ``AOShell<T>`` is abstract and its derived classes are the orderings, so the
   order a shell enumerates its AOs in is part of its type. ``CCAShell``
   implements the Common Component Architecture ordering; other conventions
   become sibling classes. ``MolecularBasisSet`` can be asked whether all of
   its shells agree.

:ref:`aoh_purity_dispatch`
   ``AOShellBase`` fixes neither the purity nor the ordering, ``AOShell<T>``
   fixes the purity, and the orderings derive from ``AOShell<T>``. Code which
   needs only the purity takes an ``AOShell<T>`` and gets the angular index
   for that purity; ``as_cartesian_shell`` and ``as_spherical_shell`` recover
   the purity of an ``AOShellBase`` in one checked step.

:ref:`aoh_normalization_placement`
   ``Primitive`` owns :math:`N^{\chi}`, ``ContractedGaussian`` owns
   :math:`N^{G}`, and ``CartesianAO`` owns :math:`N^{AO}_{ijk}`. Each class's
   ``normalization_constant`` reports the product of the factors which sit in
   front of the contraction sum, so ``CartesianAO`` reports
   :math:`N^{AO}_{ijk} N^{G}`; :math:`N^{\chi}` differs from primitive to
   primitive and so is applied inside the sum, by ``normalized_evaluate``.
   The shell carries the convention.

:ref:`aoh_value_view`
   Every class is a value/view pair sharing one CRTP-implemented API, following
   :ref:`designing_the_point_component`. At the AO layer the split reaches the
   abstract bases too: ``CartesianAO`` derives from ``AO`` and
   ``CartesianAOView`` from ``AOView``, with the two bases sharing one
   ``AOCommon``, so that a non-owning AO is one the type system can require.

:ref:`aoh_flattening`
   Every container can present the primitives beneath it as a flat sequence.
