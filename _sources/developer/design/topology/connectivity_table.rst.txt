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

.. _designing_the_connectivity_table:

################################
Designing the Connectivity Table
################################

TODO: More formal writeup

- Initially deal with indices (convenient as long as Molecule near-by)
- Need to think about how this interacts with angles, and torsions
- Need to think about how thi interacts with the graph
- Use std::array vs std::pair b/c former supports ``operator[]``
- Should there be a ``Bond`` class?
