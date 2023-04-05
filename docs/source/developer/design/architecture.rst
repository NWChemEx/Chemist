#######################
Architecture of Chemist
#######################

***********************************
Chemist Architecture Considerations
***********************************

Chemical systems.
   It is natural to express the phenomena we want to describe using chemistry
   concepts. Hence many users will feel most comfortable specifying their
   problem in terms of these chemical concepts. We therefore must have a
   software representation of the relevant concepts.

Quantum mechanics.
   The heart of quantum chemistry is that chemistry is fully described by
   quantum mechanics. Thus whatever chemical question we have, quantum
   mechanics should be able to answer. Since we want to get an answer from
   quantum mechanics, it's best to phrase the question using quantum mechanics,
   and we thus also need a software representation of quantum mechanical
   concepts.

Performance.
   Quantum chemistry is computationally expensive. Software used in the stack
   must be performant on not only current hardware, but should also be
   readily adaptable to performant execution on future hardware. An admittedly
   tall order without knowledge of said future hardware.
