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

.. _designing_fragmented_chemical_system_class:

############################################
Designing the FragmentedChemicalSystem Class
############################################

.. note::

   At present the ``ChemicalSystem`` class is somewhat of a stub. When the
   design of the ``ChemicalSystem`` class evolves the design of the
   ``FragmentedChemicalSystem`` class will evolve too.

This page describes the design of the ``FragmentedChemicalSystem`` class.

**************************************************
Why Do We Need the FragmentedChemicalSystem Class?
**************************************************

Like the ``FragmentedNuclei`` and ``FragmentedMolecule`` classes, the need for
the ``FragmentedChemicalSystem`` class was motivated by the
:ref:`fc_chemical_system_hierarchy` consideration raised in
:ref:`designing_the_fragmenting_component`. In short, fragmenting a
``ChemicalSystem`` class requires us to fragment each layer of the hierarchy.
The ``FragmentedChemicalSystem`` class is responsible for fragmenting the
``ChemicalSystem`` part.

***************************************
FragmentedChemicalSystem Considerations
***************************************

.. _fcs_chemical_system_state:

chemical system state
   The ``FragmentedChemicalSystem`` class must act like it contains a series of
   ``ChemicalSystem`` objects. This requires it to hold a ``FragmentedMolecule``
   and eventually a set of fields (fields are currently an experimental
   feature and this will need modified).

.. _fcs_chemical_system_compatible:

chemical system compatible
   The fragments in a ``FragmentedChemicalSystem`` are expected to be usable
   wherever ``ChemicalSystem`` objects can be used.


*****************************
FragmentedChemcialSystem APIs
*****************************

To construct a ``FragmentedChemicalSystem``:

.. code-block:: C++

   // It's assumed that the user already has a ChemicalSystem object
   ChemicalSystem chem_sys = make_a_chemical_system();

   FragmentedChemicalSystem empty; // See FragmentedBase for more details
   FragmentedChemicalSystem empty(chem_sys);

   //If you already have a FragmentedMolecule
   FragmentedMolecule frags = make_fragments();
   FragmentedChemicalSystem has_frags(frags);

Some notes on the above:

- ``make_a_chemical_system`` and ``make_fragments`` are opaque functions which
  respectively encapsulate the details of creating ``ChemicalSystem`` and
  ``FragmentedMolecule`` objects respectively. The details of these functions
  are irrelevant for the purposes of this discussion.

*******************************
FragmentedChemicalSystem Design
*******************************

.. _fig_fragmented_chemical_system:

.. figure:: assets/fragmented_chemical_system.png
   :align: center

   State of the ``FragmentedChemicalSystem`` class and its relationship to
   related classes.

:numref:`fig_fragmented_chemical_system` shows the design of the
``FragmentedChemicalSystem`` class. At present the class is just a thin wrapper
around a ``FragmentedMolecule`` object. Eventually we foresee the state
expanding in accordance with the :ref:`fcs_chemical_system_state` consideration.
To address the :ref:`fcs_chemical_system_compatible` consideration the
fragments will be returned as ``ChemicalSystemView`` objects.

*******
Summary
*******

:ref:`fcs_chemical_system_state`
   Right now ``FragmentedChemicalSystem`` is a thin wrapper around a
   ``FragmentedMolecule`` object. In the future it will need to be extended to
   fields.

:ref:`fcs_chemical_system_compatible`
   Fragments in the ``FragmentedChemicalSystem`` class are returned as
