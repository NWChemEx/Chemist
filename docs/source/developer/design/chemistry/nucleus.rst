.. _designing_the_nucleus_component:

###############################
Designing the Nucleus Component
###############################

**************
Considerations
**************

point charge like
   For most computational chemistry applications the nucleus of an atom is
   essentially a point charge.

   - The ramification of this statement is that the ``Nucleus`` class should
     derive from (or at least be compatible with) the ``PointCharge`` class. 
   - Compared to a point charge, a nucleus additionally knows the number of 
     protons/neutrons, its mass, and its atomic number. 

Views
   Traditionally the molecular system has been stored as a structure of arrays.
   This is primarily for performance reasons. From a user-perspective an array
   of structures is easier to use. By introducing views we can have both. The
   view objects act like the values, but only alias their state.

   - We need views of ``Nucleus`` to be used as references into the ``Nuclei``
     container.
   - We need views of ``Nuclei`` to be used in containers like 
     ``FragmentedNuclei`` where the elements are meant to be ``Nuclei`` objects.