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

In the current design, the two factor spaces of a ProductSpace are not 
commutable, which means

..  code-block:: c++

    ProductSpace(A, B) != ProductSpace(B, A);

This is due to the fact that the order of the factor spaces determines the 
order of the basis functions in the ``ProductSpace``. For example, we have an
``AOSpace`` and a ``SpinSpace`` as the factor spaces to form a ``ProductSpace`` 
called ``ASOSpace``:

..  code-block:: c++

    // ao_1, ao_2 are atomic basis functions
    ao_space_1 = AOSpace{ao_1, ao_2}; 
    // alpha and beta are spin basis functions
    spin_space = SpinSpace{alpha, beta}; 

    product_space_1 = ProductSpace(ao_space_1, spin_space) 
                    = {ao_1*alpha, ao_2*alpha, ao_1*beta, ao_2*beta};
    product_space_2 = ProductSpace(spin_space, ao_space_1)
                    = {alpha*ao_1, beta*ao_1, alpha*ao_2, beta*a0_2};

One can see that although ``product_space_1`` and ``product_space_2`` share the
same set of basis functions, the basis functions are in different order. Hence 
we do not assume the commutativity of the two factor spaces of a 
``ProductSpace``.

If all the factor spaces of the product have explicit basis functions, in the 
resulted ``ProductSpace<R...>`` the basis functions are the prodcut of the basis 
functions of all the factor spaces. If some factor spaces only have abstract 
basis functions not explicitly present in the factor spaces, the basis 
functions of the resulted ``ProductSpace`` would omit the terms from the 
abstract basis functions. However, the labels of the ``ProductSpace`` are 
always the combinations of the labels from all the factor spaces, e. g., 
"product_space_label" = "factor_space_1_label":"factor_space_2_label". In order
to compare two ``ProductSpaces``, one should just check their two factor spaces.
The size of a ``ProductSpace`` is the product of the no.s of basis functions of
the two factor spaces.

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


