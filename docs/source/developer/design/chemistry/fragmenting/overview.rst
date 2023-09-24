###################################
Designing the Fragmenting Component
###################################

**************
Considerations
**************

.. _fc_existing_classes:

existing classes
   The Fragmenting component of Chemist builds off of the ``Nuclei``, 
   ``Molecule``, and ``ChemicalSystem`` classes (and the associated views and
   single element versions).

parallel hierarchy
   Conceptually the easiest way to map a hierarchy of classes to a new hierarchy
   of classes is with a parallel hierarchy, i.e., ``Nuclei`` maps to
   ``Fragmented<Nuclei>``, ``Molecule`` maps to ``Fragmented<Molecule>``, and
   ``ChemicalSystem`` maps to ``Fragmented<ChemicalSystem>``.

performance
   Classes must ultimately be designed in a performant manner, e.g., they should
   provide opportunities to avoid copies and should store information in a
   succinct manner.

general use
   A number of methods involve fragmenting chemical systems. The classes in this
   component should be designed to be as applicable as possible and not tied to
   a single method.

   - Out of the gate we are targeting QM/MM, ONIOM, and (generalized) many-body 
     expansion methods.
   - The key unifying theme is we break the ``ChemicalSystem`` up into one or
     more (possibly non-disjoint) pieces and run calculations on those pieces.  


******************
Fragmenting Design
******************

.. figure:: assets/overview.png
   :align: center

   Architecture summary of the Fragmenting component of Chemist.  

Fragmented Class
================

Full discussion: :ref:`designing_fragmented_class`.



***************
Fragmenting API
***************

Most fragmentation workflows start with an already created ``ChemicalSystem``
class and then fragment that. Below is the proposed workflow and APIs for 
fragmenting a ``ChemicalSystem``.

.. code-block:: C++

   // Opaquely creates the system to fragment
   ChemicalSystem sys = get_chemical_system(); 

   // Step 1. We start by assigning nuclei to fragments.
   
   // This will be the sets of nuclei in each fragment
   Fragmented<Nuclei> frag_nuclei(sys.molecule().nuclei()); 
  
   // Usually assigning nuclei to fragments is much more complicated than this
   // but for illustrative purposes we just make each fragment a single nucleus
   for(auto i = 0; i < mol.nuclei().size(); ++i){
       frag_nuclei.add_fragment({i});
   }

   // Step 2. In many cases fragmenting will break covalent bonds and we will 
   // need to cap the nuclei. 
   
   // For demonstrative purposes we assume that there was only a bond between 
   // nuclei 0 and 1 that needs capped
   frag_nuclei.add_cap(0, 1, Nucleus{...}); // Cap attached to 0, replacing 1
   frag_nuclei.add_cap(1, 0, Nucleus{...}); // Cap attached to 1, replacing 0 

   // Step 3. Need to assign electrons to the fragments

   // This will be hold the Molecule piece of each fragment
   Fragmented<Molecule> frag_mol(sys.molecule());
   
   for(auto& frag_i : frag_nuclei){
       // Adds nuclei (and caps) and declares it as a neutral singlet
       frag_mol.add_fragment(frag_i, 0, 1);
   }

   // Step 4. Assign fields to each fragment

   // This will hold the final fragments (which are each a ChemicalSystem)
   Fragmented<ChemicalSystem> frag_sys(sys);

   for(auto& frag_i : frag_mol){
    // Adds molecule and its external field 
    frag_sys.add_fragment(frag_i, ...);
   }



