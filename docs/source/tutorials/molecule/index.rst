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

.. _molecule:

########
Molecule
########

In its formal definition, a molecule is a group of two or more atoms held
together by chemical bonds. In Chemist, ``Molecule`` class represents any group
of atoms (set of ``Atom`` objects) or nuclei (set of ``Nucleus`` objects forming
``Nuclei`` object) without any restrictions about bonding. 

*************
Construction
*************

There are five different ways of constructing a ``Molecule`` object including
the default and copy constructors to create an Atom object. The codes below
shows these methods for both C++ and Python. The most explicit constructor
enables a user to set the charge and multiplicity of the molecule, which would
be set to 0 and 1 if not given. It should also be noted that once a molecule
object is created, the atoms that create the molecule cannot be accessed or
modified directly, only the nuclei can be accessed.
.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/molecule_example.cpp
         :language: c++
         :start-after: // Begin constructors
         :end-before: // End constructors

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/molecule_example.py
         :language: python
         :start-after: # Begin constructors
         :end-before: # End constructors

***********
Properties
***********

Molecule class provides read access to the charge, multiplicity, and number of
electrons of a molecule object with ``charge()``, ``multiplicity()``, and
``n_electrons()`` methods, respectively. One can modify the charge and multiplicity of a
molecule with ``set_charge(int)`` and ``set_multiplicity(int)`` methods. The
code below shows how to access and modify these properties in both C++ and Python.

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/molecule_example.cpp
         :language: c++
         :start-after: // Begin properties
         :end-before: // End properties
   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/molecule_example.py
         :language: python
         :start-after: # Begin properties
         :end-before: # End properties

***********
Serializing
***********

Users can serialize a ``Molecule`` object into a binary, XML, or JSON archive and
deserialize accordingly with the C++ interface. We use Cereal library for
serialization. Note that corresponding Cereal header files need to be included.


.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/molecule_example.cpp
         :language: c++
         :start-after: // Begin serialization
         :end-before: // End serialization
