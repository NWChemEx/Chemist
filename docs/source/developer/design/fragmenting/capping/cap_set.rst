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

.. _designing_the_cap_set_class:

##########################
Designing the CapSet Class
##########################

This page describes the design of the ``CapSet`` class.

*************************
What is the CapSet class?
*************************

The ``CapSet`` class is a container of ``Cap`` objects.

********************************
Why do we need the CapSet class?
********************************

The need for the ``CapSet`` class stemmed from the :ref:`ca_container`
consideration. In short, we need a class which can hold ``Cap`` objects.

*********************
CapSet Considerations
*********************
