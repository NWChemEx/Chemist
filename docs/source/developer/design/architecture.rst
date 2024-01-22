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

.. _architecture_of_chemist:

#######################
Architecture of Chemist
#######################

:ref:`statement_of_need` summarizes what Chemist needs to do. The point of this
page is to use these needs to motivate the architecture of Chemist.

***************************
Architecture Considerations
***************************

Performance.
   Quantum chemistry is computationally expensive. Software used in the stack
   must be performant on not only current hardware, but should also be
   readily adaptable to performant execution on future hardware. An admittedly
   tall order without knowledge of said future hardware. (TODO: point to
   high-level argument)

Object-oriented
   We believe that the complexity of quantum chemistry mandates the need for
   object-oriented programming (TODO: point to high-level argument)

Chemical systems.
   It is natural to express the phenomena we want to describe using chemistry
   concepts. Hence many users will feel most comfortable specifying their
   problem in terms of these chemical concepts. We therefore must have a
   software representation of the relevant concepts.

Quantum mechanics.
   The heart of quantum chemistry is that chemistry is fully described by
   quantum mechanics. Thus whatever chemical question we have, quantum
   mechanics should be able to answer. Since we want to get an answer from
   quantum mechanics, it's best to phrase the question using quantum mechanics,
   and we thus also need a software representation of quantum mechanical
   concepts.

Classical mechanics.
   Quantum is expensive some mix of QM and classical mechanics is likely to be
   inevitable for the foreseeable future. We need the ability to seamlessly
   transition between domains.

*********************
Architecture Overview
*********************

TODO: summarize the high-level architecture

.. _fig_architecture:

.. figure:: assets/architecture.png
   :align: center

   High-level architecture of Chemist. Chemist is at present split into three
   components: Chemical System, Operators, and Wavefunctions.

The overall architecture of Chemist is depicted in :numref:`fig_architecture`
and the individual components are summarized below.

Chemical System Component
-------------------------

Main Discussion: :ref:`architecture_of_chemical_system`.

Operator Component
-------------------

Main Discussion: :ref:`architecture_of_operator`.

Wavefunction Component
----------------------

Main Discussion: :ref:`architecture_of_wavefunction`.
