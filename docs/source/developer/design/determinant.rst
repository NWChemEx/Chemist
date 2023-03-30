.. Copyright 2023 NWChemEx-Project
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

##################
Determinant Design
##################

This page describes the design of the determinant component of Chemist.

**********************
What is a Determinant?
**********************

.. |N| replace:: :math:`N`
.. |i| replace:: :math:`i`
.. |j| replace:: :math:`j`
.. |M| replace:: :math:`\mathbf{M}`
.. |Mij| replace:: :math:`M_{ij}`


Our present interest in determinants is purely from a quantum chemistry
perspective, not the general mathematical angle. In the context of
quantum chemistry determinants are basis functions for |N|-electron
wavefunctions. The name determinant comes from the fact that if you
create an |N| by |N| matrix, |M|, such that element |Mij| is the |i|-th
electron occupying the |j|-th orbital, then the determinant of |M| is a
properly antisymmetrized |N|-electron basis function.

.. note::
   One obtains the same result by instead defining |Mij| as
   the |j|-th electron in the |i|-th orbital, since
   :math:`det\left(\mathbf{M}\right)=det\left(\mathbf{M}^T\right)`.

****************************
Why Do We Need Determinants?
****************************

In quantum chemistry, determinants pair a one-electron basis set with an
occupation vector (for |N| orbitals, an |N|-element vector such that the
|i|-th element is the occupation of the |i|-the orbital). In turn, we use
determinants as a means of defining occupied/virtual orbitals and potentially
tracking non-standard occupations.

**************************
Determinant Considerations
**************************

.. _dd_ov:

Occupied/Virtual
   As mentioned in the previous section the determinant class is primarily
   envisioned as a means of storing how we partitioned the orbitals into
   occupied and virtual orbitals. In practice, most methods implicitly create
   all determinants they will need by referring to the original partitioning.
   Hence we are primarily interested in using the determinant component to
   track the reference determinant.

.. _dd_occ_restrict:

Occupation Restriction
