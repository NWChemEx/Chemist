###################
Overview of Chemist
###################

As discussed in :ref:`chemist_motivation` the ultimate goal of Chemist can be
summarized as a performant :term:`DSL` for expressing computational chemistry
calculations. This page summarizes how this goal is achieved in Chemist.

.. note::

   For simplicity version 1.0 of Chemist will limit its focus to Gaussian-based
   quantum chemistry (including :term:`DFT`). That said, care has been taken in
   designing Chemist to ensure that it can be expanded to other computational
   chemistry techniques down the road.

*************
Input vs. DSL
*************

If you already have experience with other computational chemistry packages you
are probably used to thinking of the input to a computational chemistry
package as specifying:

- the chemical system of interest (e.g., the Cartesian coordinates),
- the level of theory (i.e., method and atomic basis set), and
- a series of options (usually key/value pairs).

Most legacy codes require the user to specify the above in a text file which
then gets parsed. More modern codes usually provide mechanisms for specifying
the above programatically (such as in a Python script), but it's still usually
specified as string-based input. Regardless of the actual input mechanism, what
we just described is the :term:`UI` of the software package.

UIs are designed to ensure that the user can easily specify their intent. In
practice, this usually is accomplished by relying on implicit state. More
specifically, many packages rely on small amounts of human-readable text to
convey very complicated ideas. A familiar example is specifying the atomic basis
set; typically this is done by providing a string like ``cc-pVDZ``. The
implicit state stems from the fact that the code must know how to convert from
the user's input string to the actual exponents and coefficients comprising the
molecular basis set. In other words, the literal string ``cc-pVDZ`` does NOT
contain the explicit information the code needs.

Compared to the UI just described, the DSL contained in Chemist is much more
explicit. This is because classes comprising the DSL, like ``AOBasisSet``,
actually contain the parameters for the concepts they represent. In turn,
developers can simply prepare objects in whatever state they want (as opposed
to for example requiring extensions to the UI to handle say assigning different
atomic basis sets to different atoms). The real power of a DSL like Chemist is
in that routines which work with Chemist objects don't need to understand
strings like ``cc-pVDZ`` they just work with the data provided.


.. _fig_gaussian_input:

.. figure:: assets/gaussian_input.png
   :align: center

   Traditional workflow of a Gaussian-based quantum chemistry method.
