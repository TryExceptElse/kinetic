## Orbit

The NavStack is a stack of Segments, each one of which can be
initialised with a starting position and velocity, and can return
the position and velocity at any time T after the initial point,
while T < duration, if duration exists.

Segment is subclassed by Maneuvers and Trajectory, and possibly others
in the future.

### Style:

Note: This sub-project uses
[Google C++ style](https://google.github.io/styleguide/cppguide.html),
with the notable exception of exceptions. (See [Google's reasoning](
https://google.github.io/styleguide/cppguide.html#Exceptions))
Exceptions are to be used wherever they can reduce the quantity of code,
and improve its readability.

#### Glossary:

A number of variable names are used in this module that are, or are
adapted from mathematical symbols in common use.

Within this module, usage of these names to represent something other
than what is listed here is avoided.

* r : position vector
* v : velocity vector
* a : semi-major axis
* t : time
