********************
Sparsifying a Tensor
********************

Things to note
==============

We are creating an object, ``t`` of type ``DistArray<Tensor<Tensor<T>>>`` from
an object ``T`` of type ``DistArray<Tensor<T>>``. Since the tiles are not the
same type we are going to have to copy data. At best we are going to be able to
copy full tiles of ``T`` into ``t`` at worst we are going to be copying pieces
of tiles from ``T`` into ``t``. To my knowledge there is no way to alias the
full tiles and the copy must occur.

To capitalize on

Algorithm
=========

Assume we are given a ``SparseMap`` instance, ``sm`` and a tensor ``T``. Our
present goal is to create a tensor-of-tensors (ToT), ``t``,  out of ``T``. The
outer rank of ``t`` is given by ``sm.ind_rank()`` and the inner rank of ``t`` is
given by ``sm.dep_rank()``.

We start by assuming ``sm`` is a ``SparseMap<TileIndex, ElementIndex>``
instance. If we were given a ``SparseMap<ElementIndex, U>`` instance (``U``
being either ``TileIndex`` or ``ElementIndex``) it can be turned into a
``SparseMap<TileIndex, U>`` instance given the ``TiledRange`` of ``t``.
Similarly a ``SparseMap<U, TileIndex>`` instance can be turned into a
``SparseMap<U, ElementIndex>`` instance given the ``TiledRange`` of ``T``. There
are now two scenarios:

#. ``sm.dep_rank()`` equals the rank of ``T``, or
#. ``sm.dep_rank()`` is less than the rank of ``T``

In the former scenario we assume that the dependent indices are valid tile
indices for ``T``. To be more concrete, we assume that given the dependent index
``{0, 1}`` the corresponding tile of ``T`` is ``{0, 1}``, *i.e.*, no offsets or
permutations are required. In the second scenario we assume that by injecting
one or more independent index modes into the dependent index we get a valid
index for  ``T``. This is a bit trickier and arises when one (or more) of the
modes of  ``T`` is spanned by an independent index. For example assume that the
columns of ``T`` are spanned by the independent index and the rows are spanned
by the dependent index. ``sm`` is then such that
``sm.ind_rank() == sm.dep_rank() == 1`` and to get a full tile index for ``T``
we need to combine the independent index with the dependent index.


- For the :math:`i`-th outer tile of ``t``

  - Get from ``sm`` the domain, ``d``, associated with :math:`i`

  - If we need to inject indices create a new ``Domain<ElementIndex>``, ``d2``,
    from ``d`` and use that domain in place of ``d`` in the next step.

  - Loop over elements in the :math:`i`-th tile

