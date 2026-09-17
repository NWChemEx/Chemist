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

.. _ao_basis_set_background:

#######################
AO Basis Set Background
#######################

The purpose of this page is to provide a brief introduction to Gaussian-based
atomic orbital (AO) basis sets. This is primarily intended to establish a
common vocabulary and notation, rather than to provide a comprehensive review
of the topic.

**************************
Pieces of and Types of AOs
**************************

A Cartesian AO, :math:`\mu_{ijk}(r; d, \zeta)` is given by:

.. math::

    \mu_{ijk}(r; d, \zeta) = N_{ijk}(d, \zeta) x^iy^jz^k G(r; d, \zeta),

where :math:`N_{ijk}(d, \zeta)` is a normalization constant,
:math:`x`, :math:`y`, and :math:`z` are the Cartesian coordinates,
:math:`i+j+k = \ell` is the orbital quantum number, and :math:`G(r; d, \zeta)`
is a contracted Gaussian function with contraction coefficients :math:`d` and
exponents :math:`\zeta`. The contracted Gaussian function is given by:

.. math::

   G(r; d, \zeta) = \sum_p d_p \chi_p(r; \zeta_p),

where :math:`\chi_p` is the primitive Gaussian function with exponent
:math:`\zeta_p`. The primitive Gaussian function is given by:

.. math::

    \chi_p(r; \zeta_p) = e^{-\zeta_p r^2}.

A spherical AO, :math:`\mu_{\ell m}(r; d, \zeta)` is usually written as a
linear combination of Cartesian AOs with the same orbital quantum number:

.. math::

    \mu_{\ell m}(r; d,\zeta_p) = \sum_{i+j+k=\ell}
        c^{(ijk)}_{\ell m} \mu_{ijk}(r; d, \zeta_p),

where the sum is over all possible combinations of :math:`i`, :math:`j`, and
:math:`k` such that :math:`i+j+k = \ell`. The :math:`c` coefficients are the
transformation coefficients from Cartesian to spherical AOs and include a
normalization constant.

.. _how_are_basis_sets_stored:

**************************
How are Basis Sets Stored?
**************************

Computers didn't use to be as powerful as they are today and as a result
chemists had to be clever about how they represented data. Long story short,
they decided to implicitly store as much of the AO as possible. In practice,
this means that only the parameters for the :math:`G` piece are stored
explicitly. For a given atomic basis set, e.g. cc-pVDZ, it is the explicit
state of :math:`G`which is stored, for example on the Basis Set Exchange (BSE)
website :cite:`pritchard2019new`.
