.. Copyright 2024 NWChemEx-Project
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

.. _designing_the_cap_set_class:

##########################
Designing the CapSet Class
##########################

This page describes the design of the ``CapSet`` class.

*************************
What is the CapSet class?
*************************

The ``CapSet`` class is a container of ``Cap`` objects.

********************************
Why do we need the CapSet class?
********************************

The need for the ``CapSet`` class stemmed from the :ref:`ca_container`
consideration. In short, we need a class which can hold ``Cap`` objects.

*********************
CapSet Considerations
*********************

.. |A| replace:: :math:`A`
.. |B| replace:: :math:`B`
.. |C| replace:: :math:`C`
.. |D| replace:: :math:`D`
.. |AB| replace:: :math:`A-B`
.. |AC| replace:: :math:`A-C`
.. |AD| replace:: :math:`A-D`
.. |CB| replace:: :math:`C-B`

.. _cc_asymmetry:

asymmetry
   Given a severed bond |AB|, let caps |C| and |D| respectively replace |A|
   and |B|. It is generally the case that |CB| and |AD| will in general lead
   to two caps because |C| and |D| will not usually be value equal.

.. _cc_multiple_caps:

multiple caps
   An object |A| may make bonds to several atoms which are not in the fragment.
   If |A| is bonded to |B| and |C|, and |B| and |C| are both not in the
   fragment we need caps to address fragmenting the |AB| bond, but also the
   |AC| bond

*************
CapSet Design
*************

Following the :ref:`ca_container` consideration, the ``CapSet`` object is a
container-like object whose elements are instances of the ``Cap`` class. The
:ref:`cc_multiple_caps` consideration is addressed by adding multiple caps,
with the same anchor, to the ``CapSet`` object. In a similar vein, the
:ref:`cc_asymmetry` consideration is addressed by adding two ``Cap`` instances
to the ``CapSet`` object, each having a different anchor atom.

*********************
CapSet Design Summary
*********************

:ref:`cc_asymmetry`
   The asymmetry of capping a bond is handled by adding multiple ``Cap``
   objects to the ``CapSet`` object.

:ref:`cc_multiple_caps`
   ``Cap`` objects can be created for each of the replaced atoms.
