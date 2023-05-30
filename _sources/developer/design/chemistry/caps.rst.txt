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

.. _designing_the_caps_class:

##########################
Designing the CapSet Class
##########################

In :ref:`designing_fragmented_molecule_class` consideration :ref:`fm_caps`
gave rise to the need for a ``CapSet`` class (the name is chosen to avoid
having two classes which differ by only an "s"). This page describes the design 
of the ``CapSet``

**************
What is a cap?
**************

.. |A| replace:: :math:`A`
.. |B| replace:: :math:`B`
.. |AB| replace:: :math:`A-B`

When fragmenting large covalently bonded systems it is usually the case that
there exists at least one bond between say atoms |A| and |B|,
such that there is a fragment which contains |A|, but not |B|.
In this case we say that in forming the fragments we have broken the
|AB| bond. The resulting fragments would be radicals (assuming the
electrons in the bond are split evenly). Treating the fragments as radicals
would lead to substantially different electronic structure, and we instead
usually prefer to instead approximate the severed bond as existing
between |A| and an added monovalent atom (usually hydrogen). The 
monovalent atom (or sometimes group of atoms) is referred to as a "cap".

********************
Why do we need caps?
********************

If we want to apply fragment-based methods to large covalently-bonded systems
then we will necessarily have to break covalent bonds. Since we will not want
to treat the resulting fragments as radicals, we will also need to cap the
fragments. 

*******************
Caps considerations
*******************

.. |C| replace:: :math:`C`
.. |D| replace:: :math:`D`
.. |CB| replace:: :math:`C-B`
.. |AD| replace:: :math:`A-D`
.. |AC| replace:: :math:`A-C`

.. _cc_new_atoms:

New atoms.
   Caps typically are placed in locations in space where no supersystem atom
   resides (they usually are not placed directly where the atom they replace
   resided). This means that the caps will have to be treated as new atoms
   since they are not actually present in the target supersystem.

   - As a corollary, this consideration implies that caps are ``Atom`` or
     potentially ``Molecule`` instances depending on how many atoms comprise
     the cap.

.. _cc_asymmetry:

Asymmetry.
   Given a severed bond |AB|, let caps |C| and |D| repsectively replace |A|
   and |B|. It is generally the case that |CB| and |AD| will in general lead
   to dtwo caps because |C| and |D| will not usually reside at the same
   place.

.. _cc_bond_memory:

Bond memory.
   It is important for a cap to remember what atom it replaced and what atom
   it is bonded to.

.. _cc_multiple_caps:

Multiple caps.
   An atom |A| may make bonds to several atoms which are not in the fragment.
   If |A| is bonded to |B| and |C|, and |B| and |C| are both not in the 
   fragment we need caps to address fragmenting the |AB| bond, but also the
   |AC| bond

.. _cc_container:

Container.
   The ``CapSet`` class should be container-like with the elements being the
   individual caps. It is assumed that each cap is only added once (thus it
   is set-like), but is also indexable.

*************
CapSet Design
*************

.. _fig_caps_design:

.. figure:: assets/caps.png
   :align: center

   Overview of the ``Cap`` and ``CapSet`` classes as well as the purposed
   terminology.

The right side of :numref:`fig_caps_design` shows a typical capping scenario.
Assuming the molecule is pentane, we are forming a fragment which contains the
first two carbon atoms and their attached hydrogen atoms. Carbons three, four,
and five, as well as the hydrogen atoms attached to those carbons are not in
the fragment. In turn, this fragmentation choice results in breaking the bond
between carbons two and three. In this scenario we refer to carbon two as the
"anchor atom", since it is the atom the cap will be anchored to and carbon three
as the "replaced" atom.

Following the :ref:`cc_container` consideration, the ``CapSet`` object is a
container-like object whose elements are instances of the ``Cap`` class. Each
``Cap`` object knows the identity of the anchor atom, and the atom(s) replaced
by the cap(s). This satisfies the :ref:`cc_bond_memory` and 
:ref:`cc_multiple_caps` considerations. Consideration :ref:`cc_asymmetry` is
handled by adding two ``Cap`` instances to the ``CapSet`` object, each having a
different anchor atom. Finally, to satisfy :ref:`cc_new_atoms` each ``Cap`` 
holds ``Atom`` objects for the new atom(s).

*******************
Caps Design Summary
*******************

:ref:`cc_new_atoms`
   Each ``Cap`` object holds the atom(s) used to cap the broken bond.

:ref:`cc_asymmetry`
   The asymmetry of capping a bond is handled by adding multiple ``Cap`` objects
   ot the ``CapSet`` object.

:ref:`cc_bond_memory`
   The ``Cap`` object holds the anchor and replaced atoms in addition to the
   literal cap.

Bond memory.
   It is important for a cap to remember what atom it replaced and what atom
   it is bonded to.

:ref:`cc_multiple_caps`
   Each ``Cap`` object can hold multiple replaced atoms.

:ref:`cc_container`
   The ``CapSet`` class is container-like.