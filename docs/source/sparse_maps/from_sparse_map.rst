********************
Sparsifying a Tensor
********************

This page details the algorithms used to create a tensor-of-tensors from a
SparseMap and a tensor.

Things to note
==============

We are creating an object, ``t`` of type ``DistArray<Tensor<Tensor<T>>>`` from
an object ``T`` of type ``DistArray<Tensor<T>>``. Since the tiles are not the
same type we are going to have to copy data. At best we are going to be able to
copy full tiles of ``T`` into ``t`` at worst we are going to be copying pieces
of tiles from ``T`` into ``t``. To my knowledge there is no way to alias the
full tiles and the copy must occur. For this reason the main kernel assumes a
``SparseMap<ElementIndex, ElementIndex>`` instance is provided as this affords
us the most flexibility in defining tiles. Specialized kernels may be added at
a later point for other types of SparseMap instances.

Assumptions
===========

Assume we are given a ``SparseMap`` instance, ``sm`` and a tensor ``T``. Our
present goal is to create a tensor-of-tensors (ToT), ``t``,  out of ``T``. The
outer rank of ``t`` is given by ``sm.ind_rank()`` and the inner rank of ``t`` is
given by ``sm.dep_rank()``.

We start by assuming ``sm`` is a ``SparseMap<ElementIndex, ElementIndex>``
instance. Conversions exist to ``SparseMap<ElementIndex, ElementIndex>`` for all
other specializations of the SparseMap class. There are now two scenarios:

#. ``sm.dep_rank()`` equals the rank of ``T``, or
#. ``sm.dep_rank()`` is less than the rank of ``T``

In the former scenario we assume that the dependent indices are valid tile
indices for ``T``. To be more concrete, we assume that given the dependent index
``{0, 1}`` we take this to mean that the user wants us to retrieve element
``{0, 1}`` of ``T`` as opposed to say ``{1, 0}```, *i.e.*, no offsets or
permutations are required to convert an index in the domain to an index in
``T``. In the second scenario we assume that by injecting one or more
independent index modes into the dependent indices we get a valid index for
``T``. In other words we assume that the elements of the Domain are indices of
reduced rank slices of ``T``. Slices of the same rank as ``T`` are formed by
inserting one (or more) of the modes of the independent mode into the dependent
indices. These two scenarios are unified by realizing the first results from the
second if we have no injections. Thus as long as we code up the second scenario
in such a manner that it works with no injections we cover both scenarios.

``make_tot_tile_`` Algorithm
============================

The guts of the ``from_sparse_tensor`` function are contained in
``make_tot_tile_``. ``make_tot_tile_`` is more-or-less a lambda which will be
passed to ``TA::make_array`` to form the tensor-of-tensors. ``make_tot_tile_``
works by:

- Given:

  - the outer tile to fill, ``tile``,
  - the SparseMap from independent element indices to dependent element indices,
    ``sm``,
  - the tensor to sparsify ``T``, and
  - an ``std::map``, ``ind2mode``, such that ``ind2mode[i]`` is the mode of
    ``T`` that the ``i``-th mode of the independent indices in ``sm`` maps to.

- Loop over outer element indices in ``tile``, ``oeidx``

  - If domain associated with ``oedix`` is empty move on
  - Otherwise:

    #. Allocate inner tile, ``buffer``
    #. Create Domain with indices of ``oeidx`` injected, ``injected_d``
    #. Create Domain with tiles of ``injected_d``, ``tdomain``
    #. Loop over tiles in ``tdomain``, ``itidx``

       #. Get the tile from ``T``, ``t``
       #. Loop over elements in ``injected_d``, ``ieidx``

          - Add ``t[ieidx]`` to ``buffer``

    #. Set ``tile[oeidx]`` to ``buffer``

- Return ``tile``
