Molecule and AOBasisSet Designs
===============================

This page details notes underlying the design philosophy of the Molecule and 
AOBasisSet designs.  Superficially the two classes share many common features
and thus are the result of a similar design philosophy.

Design Parameters
-----------------

Both the Molecule and the AOBasisSet are typically thought of as ordered sets
in the mathematical sense.  This entails several key points: 

1) The classes are container-like
2) The order of the elements in the containers are fixed
   - Order is specifically fixed to the input order, *i.e.*, the first element
     inserted is 0, the second is 1, *etc.*
3) The containers do not contain duplicate elements
4) It's not uncommon to want to take unions, intersections, and set differences
   of such containers

One of the other key considerations is that for efficient linear algebra we need
the coordinates of the atoms, and the basis function parameters to be 
contiguous in memory.  This is at odds with the physical picture where we expect
to be able to do things like:

```.cpp
for(auto atom : molecule) {
}

for(auto shell : basis_set) {
}
``` 

and have `atom` and `shell` be objects that provide APIs to that particular
atom's or shell's properties such as where it's centered in space or what its
exponent parameters are.

Design Strategy
---------------

The design parameters can be separated into two categories, those dealing with
the API (should be usable like a set, provide access to mass of atom, ...) and
those dealing with the implementation (should be contiguous in memory, avoid 
storing duplicate data, *etc.*).  The traditional strategy in C++ for achieving 
this is to use the Private IMPlementation (PIMPL) pattern.  The PIMPL pattern
has two parts:

1) The public API.  All member functions of this class are declared in a header 
   file and implemented in a source file.  The only member variable of the 
   class is a private pointer to a forward declared class, the PIMPL.
2) The definition of all member functions of the public API is then accomplished
   by using the PIMPL class, whose definition need only be visible to the 
   source file.
   
Conventionally the PIMPL pattern is simply used to separate out the 
implementation.  Here, however, we want the implementation to behave differently
depending on the situation.  In the case that the user is making a single atom/
shell, we need the resulting instance to store its own memory.  When that 
atom/shell is returned from a Molecule/AOBasisSet, we need it to point to its
position in the contiguous memory.  This can be accomplished with two different
PIMPLs, that operate via a common API, or *via* a single PIMPL that always looks
up the value from an array.  Either way, in order to provide a straightforward
means of modifying the behavior in the future, we design a pure abstract 
PIMPL base class, which defines the API for all future PIMPL instantiations.
Thus we have two levels of indirection, the first separates the public API of
the classes we expect end-users to use from the implementation, the second level
defines the API all implementations need to use.

