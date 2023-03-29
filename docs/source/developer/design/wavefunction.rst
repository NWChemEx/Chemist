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

###################
Wavefunction Design
###################

This page describes the design of the wavefunction component of Chemist.

***********************
What is a wavefunction?
***********************

In quantum mechanics the state of a system is described by a wavefunction. The
wavefunction is the probability amplitude and the probability of a measurement
on the system can be computed from the wavefunction.

For our purposes, the main thing to note is that most of the methods in
electronic structure theory compute the energy (or other property) of a system,
given the wavefunction of the system.

*****************************
Why do we need wavefunctions?
*****************************

.. |N| replace:: :math:`N`

Conceptually the wavefunction component of chemist can be seen as a
generalization of the vector space component. More specifically, the vector
space component models the spaces where the one-electron basis functions live.
In practice, basis functions for an |N|-electron system are formed by taking
products of |N| one-electron basis functions (and properly antisymmetrizing
the products).
