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

.. _nucleus:

########
Nucleus
########

Nucleus is the core of an atom. Basically, it is the part of the atom that
remains if you remove all the electrons. In Chemist, Nucleus class extends the
PointCharge class with mass (``mass()``) and atomic number (``Z``) while
inheriting the coordinates (``x()``, ``y()``, ``z()``, ``coord()``) and charge
(``charge()``). In this tutorial, we will learn how to create a Nucleus object,
how to access its properties, and how to manipulate them.

*************
Construction
*************

There are four different ways to create a Nucleus object. #. The first one is
the default constructor, which creates a Nucleus object centered at the origin
(0.0, 0.0, 0.0) with zero mass and charge. #. The second one is the constructor
that takes the name of the nucleus as a string literal, its atomic number as an
integer and its mass as a float. This constructor creates the Nucleus object at
the origin with charge equal to the atomic number converted to a float. #. The
third constructor takes additionally the x, y, z positions of the nucleus as
floats. #. The fourth constructor takes additionally the charge of the Nucleus
object. Note that this is a point charge assigned to the Nucleus object, no
charged particles added or removed.

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/nucleus_example.cpp
         :language: c++
         :lines: 13-23

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/nucleus_example.py
         :language: python
         :lines: 4-6

***********
Properties
***********

Nucleus class provides access to the following properties: - name: name of the
nucleus - Z: atomic number of the nucleus - mass: mass of the nucleus in atomic
units - x: x-coordinate of the nucleus - y: y-coordinate of the nucleus - z:
z-coordinate of the nucleus - charge: charge of the nucleus in atomic units

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/nucleus_example.cpp
         :language: c++
         :lines: 26-52

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/nucleus_example.py
         :language: python
         :lines: 9-25

*********************
Modifying properties
*********************

Users can modify all the properties of a Nucleus object. Below, you can find how
you can modify the properties of a Nucleus object and compare two Nucleus
objects.

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/nucleus_example.cpp
         :language: c++
         :lines: 56-67

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/nucleus_example.py
         :language: python
         :lines: 32-38