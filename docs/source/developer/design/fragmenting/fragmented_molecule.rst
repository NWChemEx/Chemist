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

.. _designing_the_fragmented_molecule_class:

######################################
Designing the FragmentedMolecule Class
######################################

The point of this page is to document the design decisions which went into
the ``FragmentedMolecule`` class.

******************************************
Why Do We Need a FragmentedMolecule Class?
******************************************


The need for the ``FragmentedMolecule`` class was motivated by the
:ref:`fc_chemical_system_hierarchy` consideration of
:ref:`designing_the_fragmenting_component`. In short, each layer of the
``ChemicalSystem`` class will require a corresponding container for holding
fragments. This is because each layer of the ``ChemicalSystem`` introduces new
state.

*********************************
FragmentedMolecule Considerations
*********************************

.. _fmc_molecule_state:

molecule state
   The ``FragmentedMolecule`` class must hold the state for a series of
   ``Molecule`` objects. In addition to the nuclei in each fragment, this
   includes the number of electrons in each fragment as well as the
   multiplicity/spin of each fragment.

.. _fmc_molecule_compatible:

molecule compatible
   The fragments should be usable anywhere ``Molecule`` objects are.

***********************
FragmentedMolecule APIs
***********************

To construct a ``FragmentedMolecule``:

.. code-block:: C++

   // It's assumed users already have a Molecule object
   Molecule a_molecule = make_a_molecule_object();

   FragmentedMolecule null; // See FragmentedBase for more details
   FragmentedMolecule empty(a_molecule); // Empty

   // If you already have fragmented nuclei:
   FragmentedNuclei frags = make_fragments();
   FragmentedMolecule has_frags(frags, a_molecule); // dispatches to next ctor
   FragmentedMolecule has_frags2(frags, a_molecule.charge(), a_molecule.multiplicity());

   // If you also want to set the charges and multiplicities
   // Here we have fragment which is a neutral singlet and another which is a
   // doublet with a charge of +1
   auto charges = {0, 1};
   auto multiplicities = {1, 2};
   FragmentedMolecule has_frags3(frags, a_molecule, charges, multiplicities);

A couple of notes on the above:

- ``make_a_molecule_object`` and ``make_fragments`` are opaque functions which
  respectively encapsulate creating a ``Molecule`` and a ``FragmentedNuclei``
  object. The details of these functions are irrelevant for our purposes.
- Similar to ``FragmentedNuclei`` we expect that ``FragmentedMolecule`` objects
  will be created as part of an algorithm rather than defining the entire state
  initially.
- Charges and multiplicities are assumed to be such that the 0-th fragment in
  ``frags`` is the neutral singlet and the 1-st is the +1 doublet.

Following from the second note we expect the following to be the usual workflow:

.. code-block:: C++

   for(const auto& frag_i : has_frags){
      auto charge, mult  = compute_electronic_state(frag_i.nuclei());
      frag_i.set_charge(charge);
      frag_i.set_multiplicity(mult);
   }

Notably this just builds off of the functionality of ``MoleculeView``.

*******************
FragmentView Design
*******************

.. _fig_fragmented_molecule_class:

.. figure:: assets/fragmented_molecule.png
   :align: center

   State of the ``FragmentedMolecule`` class and its relationship to related
   classes.

:numref:`_fig_fragmented_molecule_class` shows the design of the
``FragmentedMolecule`` class and its relationship to other classes in Chemist.
Stemming from the :ref:`fmc_molecule_state` consideration, the
``FragmentedMolecule`` class contains a ``FragmentedNuclei`` object and two
containers which hold the number of electrons per fragment and the
spin/multiplicity of the fragments.

*******
Summary
*******

:ref:`fmc_molecule_state`
   ``FragmentedMolecule`` objects contain enough data to construct
   ``MoleculeView`` objects for each fragment.

:ref:`fmc_molecule_compatible`
   Fragments are returned as ``MoleculeView`` objects, which can be implicitly
   converted to ``Molecule`` objects.
