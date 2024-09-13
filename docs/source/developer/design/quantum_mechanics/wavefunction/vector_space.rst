#########################
Design of the VectorSpace
#########################

Some notes:

- Having a common base class is at odds with inheriting from
  ``IndexableContainerBase``. We'd have to template ``VectorSpace`` on the
  derived class to have ``IndexableContainerBase`` implement it.

  - CRTP for ``VectorSpace`` is fine if we never need a ``VectorSpace`` object.
    Would we want a property type like:
    ``BraKet<VectorSpace, Operator, VectorSpace>`` vs say
    ``BraKet<AOs, Operator, AOs>``? The former is useful for avoiding a
    combinatorial explosion in property types, e.g.
    ``BraKet<MOs, Operator, AOs>`` and ``BraKet<AOs, Operator, MOs>``.
