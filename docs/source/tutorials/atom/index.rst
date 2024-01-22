.. Copyright 2022 NWChemEx-Project
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

.. _atom:

#####
Atom
#####

Atoms are the fundamental building blocks of all matter. In Chemist, the
``Atom`` class represents an individual atom. This class is composed of the
``Nucleus`` class, which facilitates the access and modification of
nucleus-specific properties. In addition, the ``Atom``` class introduces
atom-centric properties such as ``n_electrons()``, denoting the number of
electrons, and ``charge()``, representing the net charge on the atom.

*************
Construction
*************

There are five different ways including the default and copy constructors to create an Atom object.
The codes below shows how to create a Hydrogen atom using different constructors
for both C++ and Python. The most explicit constructor enables a user to set the
name, atomic number, mass, coordinates, nuclear charge, and number of electrons
of an atom.

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/atom_example.cpp
         :language: c++
         :start-after: // Begin constructors
         :end-before: // End constructors

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_atom_example.py
         :language: python
         :start-after: # Begin constructors
         :end-before: # End constructors

***********
Properties
***********

Atom class provides read and write access to the following properties:
- name: name of the atom
- Z: atomic number of the atom
- mass: mass of the atom in atomic units
- x: x-coordinate of the atom
- y: y-coordinate of the atom
- z: z-coordinate of the atom
- nuclear_charge: charge of the nucleus in atomic units
One can also access the net charge of the atom using the ``charge()`` method,
but this is a read only access. Below, you can find how to access the properties
and modify them when possible both through the C++ and Python interfaces. You
can also see that two ``Atom`` objects can be compared using the ``==`` operator.

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/atom_example.cpp
         :language: c++
         :start-after: // Begin properties
         :end-before: // End properties
   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_atom_example.py
         :language: python
         :start-after: # Begin properties
         :end-before: # End properties

***********
Serializing
***********

Users can serialize an ``Atom`` object into a binary, XML, or JSON archive and
deserialize accordingly with the C++ interface. We use Cereal library for
serialization. Note that corresponding Cereal header files need to be included.


.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/atom_example.cpp
         :language: c++
         :start-after: // Begin serializing
         :end-before: // End serializing
