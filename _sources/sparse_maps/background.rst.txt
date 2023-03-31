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

********************
SparseMap Background
********************

Sparse maps are a way of storing tensor sparsity. Physically speaking, we use
the notation :math:`L(\mathbb{V}\rightarrow \mathbb{U})` to denote a sparse
map, :math:`L`, which  maps from vector space :math:`\mathbb{V}` to a vector
space :math:`\mathbb{U}`. In practice each member of :math:`\mathbb{V}` will map
to a subspace of :math:`\mathbb{U}` termed its "domain". Letting :math:`v` be a
vector in  :math:`\mathbb{V}` we denote the domain of :math:`v` in
:math:`\mathbb{U}` as :math:`\mathbb{U}_{v}`. A member of :math:`\mathbb{U}_{v}`
is denoted by :math:`u_v`.
