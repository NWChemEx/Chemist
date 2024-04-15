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

.. _designing_the_cap_class:

#######################
Designing the Cap Class
#######################

This page describes the design of the ``Cap`` class.

**********************
What is the Cap class?
**********************

The ``Cap`` class is responsible for storing the information associated with a
nucleus (or set of nuclei) which have been added to a fragment to fix a broken
bond. See :ref:`ca_what_is_a_cap` for more detail.

*****************************
Why do we need the Cap class?
*****************************

The need for the ``Cap`` class was motivated by the architectural consideration
:ref:`ca_new_state`. In short, caps added to complete severed bonds contain more
state than the ``Nucleus`` objects in the cap and a new class is warranted.

************************
Cap class considerations
************************

.. _cc_new_objects:

new objects
   Caps typically are placed in locations in space where no supersystem object
   resides (they usually are not placed directly where the replaced object
   resided). This means that the caps will have to be treated as new objects
   since they are not actually present in the target supersystem.

.. _cc_replaced_type:

replaced type
   When we fragment an object of type ``Nuclei`` we break bonds between
   ``Nucleus`` objects. The cap we add to fix this bond will have state
   consistent with a ``Nucleus`` object (or more generally a ``Nuclei`` object,
   *vide infra*).

.. _cc_bond_memory:

bond memory
   It is important for a cap to remember what object it replaced and what object
   it is bonded to.

.. _cc_multiple_objects:

multiple objects
   Caps are usually monovalent. They also are usually comprised of a single
   center; however, this is not always the case. In general the caps needed
   for breaking up ``Nuclei`` objects will also be ``Nuclei`` objects.

Out of Scope
============

electrons
   We treat caps purely as nuclei. The motivation is that typically users
   assign the number of electrons and multiplicity to the *capped* system, i.e.,
   electrons are not individually added to the uncapped system and then the
   caps.

**********************
Capping Classes Design
**********************

Each ``Cap`` object knows the identity of the anchor object, the identity of the
replaced object, and the nuclei forming the cap. This satisfies the
:ref:`cc_bond_memory` and :ref:`cc_multiple_objects` considerations. Finally, to
satisfy :ref:`cc_new_objects` each ``Cap`` holds a ``Nuclei`` object for the new
nucleus or nuclei.

*******************
Caps Design Summary
*******************

:ref:`cc_new_objects`
   Each ``Cap`` object holds the object(s) used to cap the broken bond.

:ref:`cc_replaced_type`
   ``Cap`` objects replace ``Nucleus`` objects, not ``Atom`` objects, i.e., they
   do not include electrons.

:ref:`cc_bond_memory`
   The ``Cap`` object holds the indices of the anchor and replaced objects
   in addition to the literal state of the cap.

:ref:`cc_multiple_objects`
   Each ``Cap`` object can contain multiple ``Nucleus`` objects.
