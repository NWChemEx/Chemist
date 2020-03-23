Let :math:`A` and :math:`B` be two vector spaces, which are panned by sets of
basis functions :math:`a` and :math:`b` respectively.

Say we have a sparse map from LMOs to PAOs and for sake of argument we have 3
LMOs and 4 PAOs. A possible

.. code:: json

   { 0 : { 1, 3}},
   { 1 : { 0, 2}},
   { 2 : { 0, 1, 3}}

This map means that for sums over PAOs we only need to consider PAOs 1 and 3 for
LMO 0, PAOs 0 and 2 for LMO 1, and PAOs 0, 1, and 3 for LMO 2. Consequentially,
if we had some vector :math:`V` equal to ``[2, 3, 4, 5]`` in the PAO basis for
all intents and purposes :math:`V` behaves looks like ``[0, 3, 0, 5]`` for LMO
0, ``[2, 0, 4, 0]`` for LMO 1, and ``[2, 3, 0, 5]`` for LMO 2. We could then
define three vectors :math:`V_i` such that :math:`V_i` is the projection of
:math:`V` onto the subspace spanned by the PAOs for LMO :math:`i`. We get:

.. code::

   V_0 = [3, 5]
   V_1 = [2, 4]
   V_2 = [2, 3, 5]

Note that even though :math:`V_0` and :math:`V_1` have the same number of
components they do **NOT** live in the same subspace of :math:`V`.

How does this work with higher-order tensors? Consider some matrix, :math:`M`

.. code::

    2,  3,  4,  5
    6,  7,  8,  9
   10, 11, 12, 13
   14, 15, 16, 17

For all intents and :math:`M` for LMO 0 looks like:

.. code::

   0,  0, 0,  0
   0,  7, 0,  9
   0,  0, 0,  0
   0, 15, 0, 17

for LMO 1 it looks like:

.. code::

   2,  0,  4, 0
   0,  0,  0, 0
   10, 0, 12, 0
    0, 0,  0, 0

and for LMO 2 it looks like:

.. code::

    2,  3, 0,  5
    6,  7, 0,  9
    0,  0, 0,  0
   14, 15, 0, 17

In other words we can think of the sparse map as:

   { 0 : { {1, 1}, {1, 3}, {3, 1}, {3, 3}},
   { 1 : { {0, 0}, {0, 2}, {2, 0}, {2, 2}},
   { 2 : { {0, 0}, {0, 1}, {0, 3}, {1, 0},
           {1, 1}, {1, 3}, {3, 0}, {3, 1}, {3, 3}}

