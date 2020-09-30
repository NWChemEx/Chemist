*************************
Sparse Map Library Design
*************************

Key Considerations
==================

- Tile vs. Element sparsity


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
tile indices or element indices). Specializations exist, which would require us
to redefine all common functions; instead we factor these common functions out
into the ``DomainBase`` class.
