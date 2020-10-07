********************
SparseMap Background
********************

Sparse maps are a way of storing tensor sparsity. Physically speaking, we use
the notation :math: `L(\mathbb{V}\rightarrow \mathbb{U})` to denote a sparse
map, :math:`L`, which  maps from vector space :math:`\mathbb{V}` to a vector
space :math:`\mathbb{U}`. In practice each member of :math:`\mathbb{V}` will map
to a subspace of :math:`\mathbb{U}`. The subspace that the :math:`i`-th member
of :math:`\mathbb{V}` maps to is termed the "domain" of :math:`i` in
:math:`\mathbb{U}`.
