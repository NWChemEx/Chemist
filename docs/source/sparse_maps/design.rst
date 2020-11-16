*************************
Sparse Map Library Design
*************************

Key Considerations
==================

- Tile vs. Element sparsity
- Storage concerns (e.g. store range, store elements, etc.)
- Encapsulate basic SparseMap/Domain operations (e.g. union, intersection, etc.)

Domain Class Hierarchy
======================

The series of classes involved in the Domain class hierarchy:

- DomainTraits
- DomainPIMPL
- DomainBase
- Domain

are charged with modeling the concept of a "domain", *i.e.*, a set-like object
which holds indices. The ``DomainTraits`` class is used to set the types for
all objects in the class hierarchy. By having a single class we avoid needing to
change types in multiple places. The ``DomainPIMPL`` class actually holds the
data for the Domain. It is envisioned that additional PIMPLs will be implemented
at a later point with different memory semantics (such as implicitly holding all
elements of the range). By separating the implementation of the ``Domain`` class
from how the data is stored we can refactor later without changing the API. The
``Domain`` class itself is templated on the type of indices it holds (either
tile indices or element indices). Specializations exist for each index type.
Normally this would would require us to redefine all common functions; instead
we factor these common functions out into the ``DomainBase`` class.

SparseMap Class Hierarchy
=========================

The SparseMap class hierarchy mirrors the Domain class hierarchy for the same
reasons. Of note we have four specializations of SparseMap:

- SparseMap<ElementIndex, ElementIndex>
- SparseMap<ElementIndex, TileIndex>
- SparseMap<TileIndex, ElementIndex>
- SparseMap<TileIndex, TileIndex>
