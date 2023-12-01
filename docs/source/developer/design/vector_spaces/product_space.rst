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

The two factor spaces of a ProductSpace should be commutable, which means

..  code-block:: c++
    ProductSpace(A, B) = ProductSpace(B, A);

If all the factor spaces of the product have explicit basis functions, in the 
resulted ``ProductSpace<R...>`` the basis functions are the prodcut of the basis 
functions of all the factor spaces. If some factor spaces only have abstract 
basis functions which are not explicitly present in the factor spaces, the 
basis functions of the resulted ``ProductSpace`` would omit the terms from 
the abstract basis functions. However, the labels of the ``ProductSpace`` are 
always the combinations of the labels from all the factor spaces. Remember the
factor spaces are mutable, so two equal ``ProductSpaces`` may have labels with
different combinational orders. Hence in the comparison of two 
``ProductSpaces``, one should not check their basis functions and labels 
directly but check their two factor spaces in both orders. The size of a 
``ProductSpace`` is the product of the no.s of basis functions of the two 
factor spaces.

The UML diagram of this class can be seen as below.

.. _umldiagram_product_space:

.. figure:: ../assets/UML_diagram-ProductSpace.png
   :align: center

   The UML diagram of the ProductSpace class.

Usage
=====

A ``ProductSpace`` can be created by:

..  code-block:: c++

    auto s = ProductSpace(A, B);

where A and B are the two factor spaces. Examples of generating new useful 
orbital spaces with ``ProductSpace`` in quantum chemistry are 

..  code-block:: c++

    // Generating atomic/molecular spin orbital spaces from 
    // atomic/molecular orbital spaces and spin spaces
    auto aso_space = ProductSpace<AOSpace_a, SpinSpace_b>;
    auto mso_space = ProductSpace<MOSpace_a, SpinSpace_b>;


