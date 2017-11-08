NWChemEx Base Repository
==============================

The purpose of this repository is to store a crisp, clean version of the CMake
build infrastructure common to the NWChemEx project in such a manner that it:

1. avoids copy/pasting,
2. facilitates uniform maintainable builds for all dependencies,
3. is customizable on a per dependency basis

If you are building a library that is meant to provide core NWChemEx
functionality it is strongly recommended you use this repository to save
yourself a lot of headaches in putting together a build.

Conventions
-----------

For the purposes of this documentation assume your project is called `MyRepo`.
The infrastructure contained in the `NWChemExBase` repository will be called
`NWChemExBase`.

:memo: CMake is case-sensitive so the capitalization of your project matters. To
that end in the following `MyRepo` is the case used by your project.


What This Repository Provides
-----------------------------

By using this repository (and adhering to its standards) your library will have
a robust build that ensures its compatibility with other parts of the NWChemEx
ecosystem.  Additionally, this repository will ensure:

1. A library named `MyRepo.so` is built (other extensions and the ability for
   static libraries will be added later).
2. `MyRepo.so` will be testable from the build directory via CMake's `ctest`
   command.
3. A file `MyRepoConfig.cmake` will automatically be created for you using the
   build settings you provided.
4. `MyRepo.so` will be installable from the build directory along with the
   header files representing its public API and the aforementioned CMake 
   config file.
5. Your library will be locatable and includable by other CMake projects via 
   CMake's `find_package` mechanism.
6. Because `NWChemExBase` takes care of 99.9% of the build for you, we
   went ahead and wrote your build documentation.  You're welcome.  Just link to
   `NWChemExBase/dox/Building.md` in your documentation and take all the credit
   for some of the best build documentation around.

How To Piggy-Back Your Build Off NWChemExBase
---------------------------------------------

0. "Install" [git subrepo](https://github.com/ingydotnet/git-subrepo) if you
   haven't already (it's just a bash script)
1. In the top-level directory of your project run:
   ~~~git
   git subrepo clone https://github.com/NWChemEx-Project/NWChemExBase
   ~~~
2. In the same directory run:
   ~~~bash
   ./NWChemExBase/bin/BasicSetup.sh <MyRepo>
   ~~~
3. Add your dependencies to generated `CMakeLists.txt` file on the line:
   ~~~cmake
   set(NWX_DEPENDENCIES <dependencies_go_here>)
   ~~~
   - :memo: Whatever values you use must be the case-sensitive name to pass to
     `find_package`.  See 
     [here](dox/ExtendingNWChemExBase.md#supported-dependencies) for a list of
     supported dependencies.
4. Fill in the source files, public headers, and compile flags of your library 
   in `MyRepo/CMakeLists.txt`
   - Ultimately the call will look like:
   ~~~cmake
   nwchemex_add_library(<MyRepo>   <variable_holding_source>
                                   <variable_holding_public_includes>
                                   <variable_holding_flags>)
   ~~~                                
5. Add your tests to `MyRepo-Test/CMakeLists.txt`
   - Format is `add_cxx_unit_test(<test_name> <MyRepo>)` where `<test_name>` is
     the name of your test (it must match a `*.cpp` file in the same directory)
   - Currently we only support unit tests     
   

More detailed instructions can be found [here](dox/ExtendingNWChemExBase.md).
