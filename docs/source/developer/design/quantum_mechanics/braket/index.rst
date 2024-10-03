.. Copyright 2024 NWChemEx-Project
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

.. _design_of_the_braket_component:

##############################
Design of the BraKet Component
##############################

This is a collection of notes on the design of the classes for the BraKet 
component and should be formalized at some point.

- Rigorously kets are vectors and bras are operations which project onto a 
  vector.
- The "projection" part of the bra involves antilinear projection on to a 
  vector. In practice, bras are labeled with the complex conjugate of the
  vector they project on to, i.e., if u is the complex conjugate of v than the
  bra which projects on to u is actually labeled v.

   - The points here being: one, we don't expect the user to specify the 
     projection, it's implied, and two based on the labeling convention we need 
     to internally take the complex conjugate of whatever the user gives us.

- Following from the last points a bra-ket is rigorously a scalar; however,
  since this scalar is a tensor element it is also common to represent the
  entire tensor by specifying a generic element of the tensor in bra-ket form.

   - Think of :math:`\textbf{A}=\braket{i\mid A\mid j}` as the analog of 
     :math:`\textbf{A} = a_{ij}`.
   - From a design standpoint this means we want to be able to specify both an
     entire tensor and a tensor element with bra-ket objects. The two are
     distinguished based on whether the usr provides a specific vector or a
     generic vector (which we represent by the entire vector space it can stand
     for).

      - We have separate base classes for Wavefunction and VectorSpace. Should 
        we have separate base classes for scalar vs tensor BraKets?

        - Users likely will expect "Wavefunction, Operator, Wavefunction" to 
          evaluate to a scalar (or vector depending on the operator) and 
          "VectorSpace, Operator, VectorSpace" to evaluate to a 
          tensor. 
        - Define ``TensorRepresentation`` for ``BraKet`` that evaluate to
          tensors and ``TensorElement`` for ``BraKet`` that evaluate to an
          element of the tensor.
        - Factor out common implementations into ``BraKetBase``.

- ``TensorRepresentation`` and ``TensorElement`` can be used to write generic
  drivers.

- Eventually ``BraKet`` should rely on a PIMPL to hold the state. PIMPLs can be
  used to optimize storage, e.g., only store the bra or the ket if its symmetric
- We probably will need ``BraKetView`` to allow existing state to be used as if
  it were a ``BraKet`` without copying it.

.. figure:: assets/braket_hierarcy.png
   :align: center

   BraKet class hierarcy.