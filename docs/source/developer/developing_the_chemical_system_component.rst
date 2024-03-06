########################################
Developing the Chemical System Component
########################################

This page contains assorted notes on developing new features for the chemical
system component.

***********************
Understanding Couplings
***********************

Modifications to the Chemical System component should be done with great care.
This is because the classes in the Chemical System component form the building
blocks for many other classes and components. In particular:

- If you add state to the ``Nuclei``, ``Atom``, ``Molecule``, or
  ``ChemicalSystem`` classes this will affect the corresponding views it also
  may affect classes in the "fragmenting" component which containers of the
  chemical system state.
