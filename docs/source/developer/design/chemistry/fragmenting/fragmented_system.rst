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

.. _designing_fragmented_system:

#########################################
Designing the Fragmented System Component
#########################################

The point of this page is to record the design process of the 
``FragmentedPIMPL<ChemicalSystem>`` class and affiliated classes.

****************************
What is a Fragmented System?
****************************

The ``ChemicalSystem`` (main discussion: :ref:`csd_chemical_system_design`)
represents the physical system we are trying to model. For traditional
electronic structure calculations the ``ChemicalSystem`` is the part of the
input independent from the method/model specification. Many approximate
methods require breaking the target chemical system up into subsets. We term
those subsets fragments and a ``Fragmented<ChemicalSystem>`` is a 
``ChemicalSystem`` which has been decomposed into fragment/field pairs.

***********************************
Why do we need a Fragmented System?
***********************************

As mentioned, there are a number of approximate methods --- *e.g.*, QM/MM,
fragment based-methods, electronic embedding, and symmetry-adapted perturbation
theory --- which require fragmenting the physical system of interest into a
series of subsystems. The ``Fragmented<ChemicalSystem>`` class allows us to 
represent fragments of the original ``Molecule`` along with the field the 
fragment is embedded in. The ``FragmentedPIMPL<ChemicalSystem>`` is charged with
implementing the ``Fragmented<ChemicalSystem>`` class.

********************************
Fragmented System Considerations
********************************

Ultimately the ``FragmentedPIMPL<ChemicalSystem>`` class will end up 
paralleling the  ``ChemicalSystem`` class and thus many of the considerations in 
:ref:`csd_considerations` apply here too. In addition:

.. _fs_hierarchy:

Hierarchy
   While we have talked about a single ``FragmentedSystem`` class up until this
   point, the ``ChemicalSystem`` class we intend to fragment is itself a 
   hierarchy. It thus makes sense to parallel the ``ChemicalSystem``
   hierarchy with a "fragmented" system hierarchy.

.. _fs_fields:

Fields.
   Each fragment may have a different field associated with it. The 
   ``FragmentedSystem`` needs to know the field of each fragment.

   - Assigning fields to a fragment usually requires knowledge of the original
     ``ChemicalSystem`` and the ``FragmentedMolecule`` object.
   - Some fields are subsets of the field for the entire system.   

.. _fs_performance:

Performance
   It is expected that the implementation under the hood will need to be
   somewhat clever in how it stores data and preserves fragment mapping. Of
   note, for fragment-based methods the number of fragments often grows 
   exponetially with system size (many of those fragments need not be
   considered, but that's a different problem). Storing an exponential number
   of copies (even if those copies are only subsets) gets expensive quickly.

   - Views allow array-of-structures API, while maintaining structure-of-array
     innards.

.. _fs_container:

Container
   ``FragmentedSystem`` is essentially a set of fragments. In terms of C++
   concepts this means the ``FragmentedSystem`` class should be container-like.

.. _fs_generality:

Generality
   Ideally the ``FragmentedSystem`` class should be applicable to as many
   methods which rely on fragments as possible. To this extent the class should
   be able to handle:

   - Disjoint and non-disjoint fragments.
   - Covalently-bonded fragments.

.. _fs_chemical_system_compatability:

ChemicalSystem compatability.
   Following from the :ref:`fs_container` consideration, ``FragmentedSystem``
   will conceptually be a container filled with ``ChemicalSystem`` objects. We
   anticipate that the primary usage of the ``FragmentedSystem`` class will be
   to pass elements of the container to algorithms which expect 
   ``ChemicqlSystem`` objects. 



************************
Fragmented System Design
************************

.. _fig_fragmented_system_design:

.. figure:: assets/fragmented_system.png
   :align: center

   How the major pieces of the ``ChemicalSystem`` map to pieces of the 
   fragmented system.

As :numref:`fig_fragmented_system_design` shows, in satisfying 
:ref:`fs_hierarchy` we have opted to mirror the existing ``ChemicalSystem``
hierarchy. More specifically, what was the ``Nuclei`` in the original 
``ChemicalSystem`` becomes the ``FragmentedNuclei`` (subsets of the original
nuclei), what was the ``Molecule`` becomes the ``FragmentedMolecule`` (subsets
of the original atoms), and what was the ``ChemicalSystem`` becomes the
``FragmentedChemicalSystem``. These three major components are described briefly
in the following subsections.

FragmentedNuclei Class
======================

Main page: :ref:`designing_fragmented_nuclei`.

Most algorithms for fragmenting a molecular system focus on the nuclei. Given
a ``Nuclei`` object, a ``FragmentedNuclei`` is a container whose elements are
subsets of ``Nuclei``. As shown in :numref:`fig_fragmented_system_design` the
actual elements are ``NucleiView`` objects, which behave like a ``Nuclei``
object, but do not own their state. As users create fragments from the 
``Nuclei`` class they add them to the ``FragmentedNuclei`` object, which tracks
them. The ``Caps`` object is needed to deal with severed
valencies.


FragmentedMolecule Class
========================

Main page: :ref:`designing_fragmented_molecule_class`.

As shown in :numref:`fig_fragmented_system_design` the ``FragmentedMolecule``
class has three components: a ``FragmentedNuclei`` object, the caps for each
element in the ``FragmentedNuclei`` object, and a mapping from
the elements of the ``FragmentedNuclei`` to their respective 
charge/multiplicities. 

FragmentedSystem Class
======================

As shown in :numref:`fig_fragmented_system_design`, the ``FragmentedSystem``
class has two pieces: a ``FragmentedMolecule`` object and a mapping from the
elements of the ``FragmentedMolecule`` object to their respective fields. At

************************
FragmentedSystem Summary
************************

:ref:`fs_hierarchy`
   As :numref:`fig_fragmented_system_design` shows the nesting of the
   ``FragmentedSystem`` class mirrors that of the ``ChemicalSystem`` class.

:ref:`fs_fields`
   The ``FragmentedSystem`` class internally contains a list of fields for
   each element of the ``FragmentedSystem``. 

:ref:`fs_performance`
   The design relies on views for accessing elements of the 
   ``FragmentedSystem``, ``FragmentedMolecule``, and ``FragmentedNuclei``.
   In turn, the innards of the classes are free to store the fragmentation
   information in whatever format is convenient for performance.

:ref:`fs_container`
   The adherenance to the container concept is reflected in the API of the
   ``FragmentedSystem`` class which is not shown in 
   :numref:`fig_fragmented_system_design`.

:ref:`fs_generality`
   In this design, disjoint/non-disjoint and covalent vs. non-covalent are 
   considerations for the ``FragmentedNuclei`` and ``FragmentedMolecule``
   classes respectively. 

:ref:`fs_chemical_system_compatability`
   Not apparent from :numref:`fig_fragmented_system_design`, but impacts the
   API of the ``ChemicalSystemView`` class. Namely, the ``ChemicalSystemView``
   class needs to be easily converted to a ``ChemicalSystem`` class.