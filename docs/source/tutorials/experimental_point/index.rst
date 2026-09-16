.. Copyright 2026 NWChemEx-Project
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

.. _experimental_point_tutorial:

##################################
Points: Measuring a Water Molecule
##################################

This tutorial introduces the experimental point component --- ``Point``,
``PointView``, ``PointSet``, and ``PointSetView``, all in the
``chemist::experimental`` namespace --- by working through a single problem:

    Given the three nuclear positions of a water molecule, move the molecule
    so that its geometric center sits at the origin, measure its bond lengths
    and bond angle, and then hand its coordinates to an external library which
    knows nothing about Chemist.

Every code block below is compiled and run as part of Chemist's test suite;
see ``tests/cxx/doc_snippets/experimental_point_example.cpp`` and
``tests/python/doc_snippets/test_experimental_point_example.py``.

.. note::

   These classes live in ``chemist::experimental`` and are being developed
   alongside the existing ``chemist::Point<T>``. Nothing in ``chemist::`` has
   changed. The reasoning behind the design is written up in
   ``docs/source/developer/design/point/point.rst``.

*************************
Getting a Number Back Out
*************************

Coordinates are stored type-erased, which means a point never has to expose
whether it uses ``float`` or ``double``, and code which takes a point never has
to be templated on that choice. Similarly, asking a point for its ``x``-
coordinate gives you back a type-erased value. If you want an actual ``double``
you have to ask for it.

.. literalinclude:: ../../../../tests/cxx/doc_snippets/experimental_point_example.cpp
   :language: c++
   :start-after: // Begin as_double
   :end-before: // End as_double

Users of points are encouraged to avoid unwrapping the type-erased value unless
they have to. Chemist is designed to work with Point objects directly to
facilitate this.

*****************************
One API, Two Ownership Models
*****************************

A point can either own its coordinates or alias coordinates owned by something
else. ``Point`` does the former and ``PointView`` does the latter, but they
share a single API, so a function which computes a distance does not have to
care which one it was handed:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/experimental_point_example.cpp
         :language: c++
         :start-after: // Begin distance
         :end-before: // End distance

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_experimental_point_example.py
         :language: python
         :start-after: # Begin distance
         :end-before: # End distance

``const_point_view`` is an alias for ``PointView<const Point>``, a view of a
point you may read from, but not write to. Python has ``ImmutablePointView`` to
serve the same purpose as ``const_point_view``. ``Point`` converts to
``const_point_view`` (or in Python ``ImmutablePointView``)  implicitly, so
callers pass a ``Point`` into functions expecting a ``const_point_view`` and
the conversion happens on its own.

The same relationship holds one level up, between ``PointSet`` and
``PointSetView``:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/experimental_point_example.cpp
         :language: c++
         :start-after: // Begin centroid
         :end-before: // End centroid

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_experimental_point_example.py
         :language: python
         :start-after: # Begin centroid
         :end-before: # End centroid

*********************
Building the Molecule
*********************

With those two helpers in hand, the molecule itself is unremarkable:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/experimental_point_example.cpp
         :language: c++
         :start-after: // Begin building
         :end-before: // End building

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_experimental_point_example.py
         :language: python
         :start-after: # Begin building
         :end-before: # End building

Internally, however, ``water`` is *not* a container of three ``Point``
objects. It holds three arrays --- one for every point's ``x``-coordinate, one
for every ``y``, one for every ``z``. That layout is what many HPC libraries
expect because it vectorizes, and we will take advantage of it at the end of
this tutorial.

That leaves the set with no ``Point`` objects inside it to hand out. Indexing
into a ``PointSet`` gives you a ``PointView`` instead:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/experimental_point_example.cpp
         :language: c++
         :start-after: // Begin indexing
         :end-before: // End indexing

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_experimental_point_example.py
         :language: python
         :start-after: # Begin indexing
         :end-before: # End indexing

The important part is the second half. Indexing the set does not copy a point
out of it. You get a handle onto the set's own storage, and writing through
that handle writes into the set.

*********************************
Centering the Molecule
*********************************

That property is what makes translating the molecule a three-line loop. Each
``p`` is a view of the set, so each ``set_*`` call lands in the set's
coordinate arrays directly:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/experimental_point_example.cpp
         :language: c++
         :start-after: // Begin centering
         :end-before: // End centering

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_experimental_point_example.py
         :language: python
         :start-after: # Begin centering
         :end-before: # End centering

Note that ``centroid_of`` was written to take a read-only view of a set, and
is being called here with a ``PointSet``. As with ``distance``, the conversion
is implicit.

**********************
Measuring the Molecule
**********************

Bond lengths and bond angles come out of the same shared API. Subtracting two
points gives the vector between them, and that vector is an owning ``Point``
even when both operands were views --- so it stays valid after the expression
which produced it:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/experimental_point_example.cpp
         :language: c++
         :start-after: // Begin measuring
         :end-before: // End measuring

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_experimental_point_example.py
         :language: python
         :start-after: # Begin measuring
         :end-before: # End measuring

***************************
Handing the Coordinates Out
***************************

Finally, the reason the set is laid out the way it is. Suppose an external
library wants the coordinates as three bare arrays:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/experimental_point_example.cpp
         :language: c++
         :start-after: // Begin external
         :end-before: // End external

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_experimental_point_example.py
         :language: python
         :start-after: # Begin external
         :end-before: # End external

Because the set already stores one array per Cartesian direction, satisfying
that request costs nothing. There is no repacking step; you ask the set for
the array you want and take a pointer to it:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/experimental_point_example.cpp
         :language: c++
         :start-after: // Begin raw_data
         :end-before: // End raw_data

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_experimental_point_example.py
         :language: python
         :start-after: # Begin raw_data
         :end-before: # End raw_data

.. warning::

   A pointer obtained this way is only valid while the set's storage is
   unchanged. Anything which grows the set --- ``push_back``, for instance ---
   may reallocate and invalidate both the pointer and every ``PointView`` you
   are holding.

*******************
Passing Sets Around
*******************

The set-level views close the loop. A ``PointSetView`` is a handle onto
somebody else's points: it can read and write them, but it cannot grow or
shrink the set, which makes it the right thing to hand to code that has no
business resizing your molecule. The read-only flavor cannot even write:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../../tests/cxx/doc_snippets/experimental_point_example.cpp
         :language: c++
         :start-after: // Begin views
         :end-before: // End views

   .. tab:: Python

      .. literalinclude:: ../../../../tests/python/doc_snippets/test_experimental_point_example.py
         :language: python
         :start-after: # Begin views
         :end-before: # End views

*****
Recap
*****

- ``Point`` owns three coordinates; ``PointView`` aliases three coordinates
  owned by something else. They share one API.
- ``PointSet`` owns three *arrays* of coordinates, one per Cartesian
  direction; ``PointSetView`` aliases them. They share one API.
- Indexing a set gives a view into it, not a copy out of it.
- ``Point`` converts implicitly to ``PointView``, ``PointSet`` converts
  implicitly to ``PointSetView``, and a mutable view converts implicitly to a
  read-only one. The reverse conversions do not exist.
- The concrete floating-point type is never part of the API. Consumers which
  need one name it themselves.
