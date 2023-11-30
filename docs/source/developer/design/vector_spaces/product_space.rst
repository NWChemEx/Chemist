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

.. _productspace_design:

######################
Product Space Design
######################

Structure
=========

The current implementation of ``ProductSpace`` class on consider two vector
space factors. The product space of more than two spaces can be obtained 
through a composite construction process, e. g., 

..  code-block:: c++
    ProductSpace(A, B, C) = ProductSpace(A, ProductSpace(B, C));



The UML diagram of this class can be seen as below.

.. _umldiagram_product_space:

.. figure:: ../assets/UML_diagram-ProductSpace.png
   :align: center

   The UML diagram of the ProductSpace class.

Usage
=====

A ``ProductSpace`` can be created by:

..  code-block:: c++

    auto s = ProductSpace(3);

