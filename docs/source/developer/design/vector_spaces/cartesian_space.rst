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

.. _cartesianspace_design:

######################
Cartesian Space Design
######################

In ``CartesianSpace``, ``label_container`` (the type of the container for 
Cartesian axes labels), ``m_N_`` (no. of basis functions) and ``m_axis_vec_``
(vector of axis labels) are declared as private. ``size_type`` from 
``BaseSpace`` and ``label_type`` (string) are public. Public methods include 
the following:

* The default constructor; 
* A constructor setting up an N-dimensional ``CartesianSpace`` with all null 
  axis labels; 
* A specific constructor to create an N-dimensional ``CartesianSpace`` with 
  the axis labels set up with user-input iterators (``ItType`` is the template 
  parameter of the iterators);
* The copy and move constructors;
* The copy and move assignment operators;
* The ``label()`` method to access one of the axis labels;

Protected methods include

* The polymorphic form of the ``size()`` method which returns the no. of basis
  functions in the ``CartesianSpace``;
* The polymorphic form of the ``equal_()`` method which compares two 
  ``CartesianSpace`` instances;
* The polymorphic form of the ``clone_()`` method which clone the current
  ``CartesianSpace`` instance.

The comparison operators ``==`` and ``!=`` are also implemented.

The UML diagram of this class can be seen as below.

.. _umldiagram_base_space:

.. figure:: ../assets/UML_diagram-CartesianSpace.png
   :align: center

   The UML diagram of the CartesianSpace class.
