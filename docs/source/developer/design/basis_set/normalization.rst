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

.. _designing_basis_set_normalization:

#################################
Designing Basis Set Normalization
#################################

This section contains notes on how normalization is represented in Chemist's
basis set component.

.. warning::

   References were added by AI and have not been human verified. Please
   report errors.

**********************
What is Normalization?
**********************

An atomic orbital (AO), :math:`\mu`, is said to be normalized if:

.. math::

   \langle \mu | \mu \rangle = 1.

:ref:`ao_basis_set_background` builds a Cartesian AO in three layers: primitive
Gaussians :math:`\chi_p` combine into a contracted Gaussian :math:`G`, which is
then paired with a Cartesian polynomial to give :math:`\mu_{ijk}`. Each layer
introduces something which is not normalized, and correspondingly each layer
contributes one factor to the overall constant. Writing those factors
separately,

.. math::

   \mu_{ijk}(\vec{r}) = \underbrace{N^{AO}_{ijk}}_{\text{AO}}\;
     \underbrace{N^{G}(d, \zeta; \ell)}_{\text{contracted Gaussian}}\;
     x^i y^j z^k \sum_p d_p\,
     \underbrace{N^{\chi}(\zeta_p; \ell)}_{\text{primitive}}\,
     \chi_p(\vec{r}; \zeta_p),

where :math:`\ell = i + j + k`. The three factors are developed below
:cite:`gill1994molecular,fermann2020fundamentals`.

Primitive normalization
=======================

The innermost factor scales a single primitive so that, paired with the
polynomial, it has unit norm:

.. math::

   N^{\chi}(\zeta_p; \ell) =
     \sqrt{\frac{2^{\ell}\left(2\zeta_p\right)^{\ell + 3/2}}
                {\sqrt{\pi^3}\,(2\ell-1)!!}}
   \;=\;
     \left(\frac{2\zeta_p}{\pi}\right)^{3/4}
     \sqrt{\frac{(4\zeta_p)^{\ell}}{(2\ell-1)!!}}.

Note this factor sits *inside* the sum, because it differs from primitive to
primitive. It is therefore not something which can be pulled out in front of
the contraction; folding it in to the contraction coefficients changes the
weights of the primitives. The coefficients are said to be "weight
normalized."

Contracted Gaussian normalization
=================================

Normalizing each primitive does not normalize their sum, because distinct
primitives are not orthogonal. The contracted Gaussian therefore needs its own
factor,

.. math::

   N^{G}(d, \zeta; \ell) =
     \left[\sum_{pq} d_p d_q\, S_{pq}\right]^{-1/2},
   \qquad
   S_{pq} = \left(\frac{2\sqrt{\zeta_p \zeta_q}}
                       {\zeta_p + \zeta_q}\right)^{\ell + 3/2},

where :math:`S_{pq}` is the overlap of the two *already normalized* primitives.
Because they are normalized, :math:`S_{pp} = 1`, so this factor measures
nothing but the non-orthogonality between distinct primitives. It is a single
scalar in front of the sum, and it depends on the whole contraction at once.

Cartesian AO normalization
==========================

The two factors so far depend on :math:`\ell` but not on how the angular
momentum is distributed among the axes. That distribution matters, and the
remaining factor accounts for it:

.. math::

   N^{AO}_{ijk} = \sqrt{\frac{(2\ell-1)!!}
                        {(2i-1)!!\,(2j-1)!!\,(2k-1)!!}}.

By construction :math:`N^{AO}_{ijk} = 1` when all the angular momentum sits on
one axis, *i.e.*, for :math:`z^\ell` and its permutations, and it is larger
otherwise. For a :math:`d` shell it is :math:`\sqrt{3}` for :math:`d_{xy}`
against :math:`1` for :math:`d_{zz}`, which is not a difference one can
neglect.

This is the only one of the three factors which distinguishes the components of
a shell, and it is the one which is usually dropped. Since the components of a
shell are typically stored implicitly, there is nowhere natural to put a
per-component constant, so the common practice is to stop after
:math:`N^{\chi}` and :math:`N^{G}` and let the single-axis component stand in
for the whole shell. Doing so leaves every other component short by a factor of
:math:`N^{AO}_{ijk}`.

Spherical shells do not have this problem because the Cartesian-to-spherical
transformation coefficients
:cite:`schlegel1995transformation` have :math:`N^{AO}_{ijk}` built into them,
and so they assume they are being handed Cartesian AOs that are only
normalized up to  :math:`N^{G}`.

***********************************
Why does Chemist need to own this?
***********************************

Chemist's philosophy is to be self-describing. Since the parameters will enter
chemist without a description, Chemist must be the one that establishes the
convention and records it.


.. _normalization_considerations:

****************************
Normalization Considerations
****************************

Topics in this section were considered in designing how normalization is
represented and ultimately were addressed by the design.

.. _n_raw_parameters_preserved:

Raw parameters are preserved
   Normalizing by rewriting the stored coefficients destroys what the user
   supplied.

   - The parameters a user provided should be recoverable unchanged.
   - Normalization constants should be computed from those parameters on
     demand rather than folded into them at construction.
   - Folding normalization in is still useful for handing data to an external
     library, but that should produce a new object rather than mutating the
     original.

.. _angular_momentum_in_primitive:

Angular momentum in primitive
   The primitive normalization factor depends on :math:`\ell` therefore the
   primitive must store it.

.. _n_single_source:

One source of truth
   Consumers should not each re-derive the normalization mathematics.

   - Each factor should be implemented once.
   - Both :math:`N^{\chi} N^{G}`, which integral libraries expect, and the full
     product including :math:`N^{AO}_{ijk}`, which real-space evaluation requires,
     should come from that one implementation, so that the two cannot drift
     apart.

.. _n_component_vs_shell:

Consumers stop at different layers
   Following from :ref:`n_single_source`, the two consumers of normalization
   want different numbers.

   - Code evaluating an AO at a point in space needs the full product,
     :math:`N^{AO}_{ijk}` included.
   - Code interfacing with integral libraries needs the product stopped after
     :math:`N^{G}`, since those libraries apply one scalar per shell and their
     Cartesian-to-spherical coefficients already carry :math:`N^{AO}_{ijk}`.
   - Both must be available, and it must be unambiguous which is which.

Out of Scope
============

Topics in this section were considered, but do not play a role in the current
design.

General contractions
   The design assumes segmented contractions, in which each contracted
   Gaussian has its own primitives. Normalization of general contractions,
   where several contractions share a primitive set, is deferred along with
   general contractions themselves.

Effective core potentials
   ECPs carry their own conventions and are not represented.

Non-Gaussian basis functions
   The normalization constants above are specific to Gaussians. Slater
   orbitals and numerical orbitals would need their own.

Orthonormalization
   AOs are non-orthonormal by default and most code assumes this. We're not
   going to worry about orthonormalizing the basis set.

*******
Summary
*******

:ref:`n_raw_parameters_preserved`
   Constants are computed on demand from the stored parameters. Converting
   between conventions produces a new object rather than mutating an existing
   one.

:ref:`angular_momentum_in_primitive`
   The primitive normalization factor depends on :math:`\ell` therefore the
   primitive must store it.

:ref:`n_single_source`
   The formulas are implemented once as free functions over the parameters
   they depend on, and every class-level accessor delegates to them.

:ref:`n_component_vs_shell`
   ``AOShell`` reports :math:`N^{\chi} N^{G}`; ``AO`` completes the product
   with :math:`N^{AO}_{ijk}`. The two consumers want the same product truncated at
   different points, not two different calculations.

***************
Further Reading
***************

The closed-form normalization constants, and the integral machinery they
belong to, are derived in Gill's review :cite:`gill1994molecular` and, in a
more recent and freely available treatment, by Fermann and Valeev
:cite:`fermann2020fundamentals`. Helgaker, Jørgensen, and Olsen
:cite:`helgaker2000molecular` cover the same ground at textbook length,
including the solid harmonics. For the Cartesian-to-spherical transformation
specifically, and the normalization convention embedded in its coefficients,
see Schlegel and Frisch :cite:`schlegel1995transformation`.

.. bibliography::
   :filter: docname in docnames
