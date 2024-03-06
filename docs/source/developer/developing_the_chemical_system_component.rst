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

########################################
Developing the Chemical System Component
########################################

This page contains assorted notes on developing new features for the chemical
system component.

***********************
Understanding Couplings
***********************

Modifications to the Chemical System component should be done with great care.
This is because the classes in the Chemical System component form the building
blocks for many other classes and components. In particular:

- If you add state to the ``Nuclei``, ``Atom``, ``Molecule``, or
  ``ChemicalSystem`` classes this will affect the corresponding views it also
  may affect classes in the "fragmenting" component which containers of the
  chemical system state.
