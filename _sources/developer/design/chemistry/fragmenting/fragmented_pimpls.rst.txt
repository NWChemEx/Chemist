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

.. _designing_fragmented_pimpls:

######################################
Designing the Fragmented PIMPL Classes
######################################

This page describes the design of the ``FragmentedPIMPL<T>`` classes.

*********************************
What is a Fragmented PIMPL Class?
*********************************

The ``FragmentedPIMPL<T>`` class is the class which implements ``Fragmented<T>`` 
objects.

****************************************
Why Do We Need a Fragmented PIMPL Class?
****************************************

The need for a ``FragmentedPIMPL<T>`` class came about as part of the discussion
on the :ref:`designing_fragmented_class` page. Briefly, different ``T`` choices
have different state that the ``Fragmented<T>`` objects need to account for.
The ``FragmentedPIMPL<T>`` class is responsible for implementing methods and
storing state associated with ``T``. ``Fragmented<T>`` implements common
functionality for the class hierarchy and dispatches to the PIMPL for state-
specific functionality. 

*******************************
Fragmented PIMPL Considerations
*******************************

Most of the considerations for the ``FragmentedPIMPL<T>`` class comes from the
:ref:`designing_fragmented_class` page. 

.. _fp_additional_state:

additional state
   Storing the non-container state of an object of type ``T`` is the 
   responsibility of the ``FragmentedPIMPL<T>`` object.

.. _fp_fragment_state:

fragment state
   The ``FragmentedPIMPL<T>`` objects are responsible for storing not just
   state associated with ``T`` objects, but also the state associated with
   the fragments, e.g., caps.

.. _fp_non_disjoint:

Non-disjoint.
   The ``FragmentedPIMPL<T>`` classes must be able to store non-disjoint
   fragments.

.. _fp_container:

container.
   ``FragmentedPIMPL<T>`` objects will implement ``Fragmented<T>`` objects,
   which are containers.

.. _fp_immutable_superset:

immutable superset
   For the purposes of the super-/sub-set relation we assume that the
   superset has been fully initialized and will not change.

   - If the superset changes it will invalidate the elements of the 
     entire class hierarchy, which is difficult to recover from.

***********************
Fragmented PIMPL Design
***********************

.. _fig_fragmented_pimpl_design:

.. figure:: assets/fragmented_pimpl.png
   :align: center

   The state of the specializations of the ``FragmentedPIMPL<T>`` class.

:numref:`fig_fragmented_pimpl_design` shows the state of the 
``FragmentedPIMPL<Nuclei>``, ``FragmentedPIMPL<Molecule>``, and
``FragmentedPIMPL<ChemicalSystem>`` specializations of ``FragmentedPIMPL<T>``.
In accordance with consideration :ref:`fp_additional_state` 
``FragmentedPIMPL<Molecule>`` and ``FragmentedPIMPL<ChemicalSystem>`` each
contain the additional, non-container state. Consideration 
:ref:`fp_fragment_state` is satisfied by having the caps live in
``FragmentedPIMPL<Nuclei>``. 

The :ref:`fp_non_disjoint` consideration primarily affects how we store the
fragmented state. For example, if we assumed disjoint fragments we could just
have a "number of nuclei" long vector where the ``i``-th element is the
fragment number nucleus ``i`` belongs to. Instead, ``FragmentedPIMPL<Nuclei>``
stores the nuclei indices for each fragment and does not require nuclei to
only appear in a single fragment. Similar considerations hold for the
fields in the ``FragmentedPIMPL<ChemicalSpace>`` class.

Somewhat related to the last point, each of the specializations store the
state per fragment and thus are container like. In turn the various
``FragmentedPIMPL<T>`` specializations are able to ensure that ``Fragmented<T>``
is container-like, in accordance with the :ref:`fp_container` consideration.
Finally to address the :ref:`fp_immutable_superset` consideration we store
a pointer to a ``const Nuclei`` object in the ``FragmentedPIMPL<Nuclei>``
specialization.

*******
Summary
*******

:ref:`fp_additional_state`
   The ``FragmentedPIMPL<T>`` class template is specialized for each type ``T``
   in order to ensure that any additional, non-container, state can be
   accomodated.

:ref:`fp_fragment_state`
   At present the only non-fragment state are the caps. 
   ``FragmentedPIMPL<Nuclei>`` is capable of storing the caps.

:ref:`fp_non_disjoint`
   The nuclei in each fragment are stored in ``FragmentedPIMPL<Nuclei>``. The
   storage mechanism allows the same nucleus to appear in more than one
   fragment.

:ref:`fp_container`
   Each ``FragmentPIMPL<T>`` specialization holds the state for each fragment.
   Fragments are returned as ``FragmentView<U>`` objects (where ``U`` is
   an object which behaves like a reference to a ``T`` object).

:ref:`fp_immutable_superset`
   The superset is stored as a ``const Nuclei`` in the 
   ``FragmentedPIMPL<Nuclei>`` specialization.