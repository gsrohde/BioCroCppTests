Draft Issue About C++ BioCro Interface and C++ Testing


# Towards a C++ Interface for BioCro

Since we have discussed for some time that the BioCro C++ library should be able to be used on its own without the R interface provided by the BioCro R package, I've been thinking about what the public interface to the C++ library should look like.  Concomitantly, I've been experimenting with testing BioCro at the C++ level using the _GoogleTest_ testing framework.

## Public interface versus publicly-available classes and functions

The number of public funhctions and methods in the BioCro C++ library is vast whereas the number of R functions that interact with this library is relatively small--only about ten.  Clearly a public interface to the BioCro C++ need only make available a tiny fraction of the classes and functions that could be called from a user of the library having access to the requisite header files.

## An experimental Public Interface/Test Suite repository

To test out the design of a public interface, I've created a public repository `gsrohde/BioCroCppTests` on GitHub.  This consists of two public-interface header files, several tests, a Make file for building the tests, and a submodule `testBML` used in testing the use of user-created module libraries.  Also included is a README file that tells how to use the repository's collection of files with BioCro.

## Notes on the interface files.

There are two interface header files, `BioCro.h` and `BioCro-extended.h`.

`BioCro.h` is intended to provide only the highest-level classes and data structures needed to use BioCro.so in writing C++ programs.  To wit, it provides the class most central to BioCro’s functioning, called here _Simulator_, and it provides the data types needed in constructing a simulator, namely _State_ (for specifying the initial state), _Parameter_set_, _System_drivers_, _Module_set_, and _Standard_BioCro_library_module_factory_.  The type returned by _Simulator.run_simulation_ is called _Simulation_result_.  I have chosen these names to further distance conceptual types embodied by these classes from their implementation.  For example, both_State_ and _Parameter_set_ implemented by the standard library type _unordered_map<string, double>_ (aliased to _state_map_ in BioCro), but they seemed conceptually distinct enough to give them different names.  As another example, I use the name _Module_set_ to emphasize that order of the elements used in initializing objects of this class is immaterial.  (It is also meant to suggest that any given module appears at most once in a _Module_set_, but this isn’t enforced in the case of sets of differential modules.)

(A drawback using these concept-oriented names is that they give the user little clue as to how objects of such classes may be used—in other words, what methods are available when working with such objects.  The solution is to document the class and functions provided by the interface files directly.

Note that names declared in the interface header files are mostly aliases.  Thus, the class functions available for classes declared in these header files are exactly the same as the class functions of the class aliased.  But we could have gone further: we could have, for example, made the names of classes that wrap, rather than alias, the old ones.  In this way, we could restrict, if desired, the functions available









The focus of these tests is more to demonstrate the proposed interface and not so much thoroughness.


I