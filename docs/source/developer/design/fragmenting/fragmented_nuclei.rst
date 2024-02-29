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

.. _designing_fragmented_nuclei_class:

####################################
Designing the FragmentedNuclei Class
####################################

This page describes the design of the ``FragmentedNuclei`` class.

*******************************
FragmentedNuclei Considerations
*******************************

.. _fn_nuclei_to_fragment_map:

nuclei to fragment map
   The ``FragmentedNuclei`` must track which nuclei are in which fragments.

.. _fn_caps:

caps
   Responsibility for satisfying the :ref:`fc_caps` consideration from
   :ref:`designing_the_fragmenting_component` was given to the
   ``FragmentedNuclei`` class.

   - The same cap may appear multiple times in a set of fragments. The
     ``FragmentedNuclei`` object should store a single set of caps and map
     each fragment to the caps it needs.

.. _fn_non_disjoint:

non-disjoint
   The :ref:`fc_non_disjoint` consideration from
   :ref:`designing_the_fragmenting_component` is ultimately the responsibility
   of the ``FragmentedNuclei`` object.

********************
FragmentedNuclei API
********************

It's assumed that ``FragmentedNuclei`` objects will be built up over the course
of an algorithm, e.g., via a series

.. code-block:: C++

   // Range-based for loop over fragments
   for(NucleiView frag_i : fragments){
       // fragments are interchangeable with Nuclei
       function_expecting_nuclei_object(frag_i);
   }



***********************
FragmentedNuclei Design
***********************

.. _fig_fragmented_nuclei_design:

.. figure:: assets/fragmented_nuclei.png
   :align: center

   The state of the ``FragmentedNuclei`` class and its relation to other
   classes.

************************
FragmentedNuclei Summary
************************

:ref:`_fn_nuclei_to_fragment_map`
   Internally the ``FragmentedNuclei`` class tracks the nuclei in each fragment
   via their offset in the supersystem.
