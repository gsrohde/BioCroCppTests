\[Draft repository for defining and testing a C++ BioCro interface\]

_For instructions on using these materials, see [README](README)._

# Towards a C++ Interface for BioCro

We have bantered about the notion that the BioCro C++ library should
be able to be used on its own without the R interface provided by the
BioCro R package for some time now.  The [BioCro
II](https://academic.oup.com/insilicoplants/article/4/1/diac003/6527687)
paper even states "The central framework can conveniently be accessed
through the R package interface or directly through C/C++, and other
interfaces can be developed without duplicating the essential code."
So I have been thinking about what the public interface to the C++
library should look like.  Concomitantly, I have been experimenting
with testing BioCro at the C++ level using the _GoogleTest_ testing
framework.

### Public interface versus publicly-available classes and functions

The number of public funhctions and methods in the BioCro C++ library
is vast, whereas the number of R functions that interact with this
library is relatively small—only about ten.  Clearly a public
interface to the BioCro C++ need only make available a tiny fraction
of the classes and functions that could be called by a user of the
library who has access to the requisite header files.

### An experimental Public Interface/Test Suite repository

To test out the design of a public interface, I've created a public
repository `gsrohde/BioCroCppTests` on GitHub.  This consists of two
public-interface header files, several tests, a _Make_ file for
building the tests, and a submodule `testBML`, used in testing the use
of user-created module libraries.  Also included is a README file that
tells how to use the repository's collection of files with BioCro,
along with this file that you are reading.

## Notes on the interface files

There are two interface header files, `BioCro.h` and `BioCro-extended.h`.

`BioCro.h` is intended to provide only the highest-level classes and
data structures needed to use BioCro.so in writing C++ programs.  To
wit, it provides the class most central to BioCro’s functioning,
called here `Simulator`, and it provides the data types needed in
constructing a simulator:

* `State` (for specifying the initial state)
* `Parameter_set`
* `System_drivers`
* `Module_set` (for both the direct and the differential module collections)
* `Standard_BioCro_library_module_factory`

The type returned by `Simulator.run_simulation` is called
`Simulation_result`.

I have chosen these names to further distance conceptual types
embodied by these classes from their implementation.  For example,
both`State` and `Parameter_set` are implemented by the standard
library type `unordered_map<string, double>` (aliased to `state_map`
in BioCro).  But they seemed conceptually distinct enough to give them
different names.

As another example, I use the name `Module_set` to emphasize that
order of the elements used in initializing objects of this class is
immaterial.  (It is also meant to suggest that any given module
appears at most once in a `Module_set`, but this isn’t enforced in the
case of sets of differential modules.)

(A drawback using these concept-oriented names is that they give the
user little clue as to how objects of such classes may be used—in
other words, what methods are available when working with such
objects.  The solution is to document each of these aliases as if they
were classes defined directly.)

Note that names declared in the interface header files are mostly
aliases.  Thus, the class functions available for classes declared in
these header files are exactly the same as the class functions of the
class aliased.  But we could have taken a further stop in distancing
the public interface types from their BioCro implementations: we could
have, for example, made the names into names of classes that wrap
existing class instead of merely aliasing them.  In this way, we could
restrict, if desired, the functions available, or even rename or alter
them in some useful way.









The focus of these tests is more to demonstrate the proposed interface and not so much thoroughness.


I