## Orbit

The NavStack is a stack of Segments, each one of which can be
initialised with a starting position and velocity, and can return
the position and velocity at any time T after the initial point,
while T < duration, if duration exists.

Segment is subclassed by Burn and Orbit, and possibly others in the
future.

### Style:

Note: This sub-project uses 
[Google C++ style](https://google.github.io/styleguide/cppguide.html), 
with the notable exception of exceptions. (See 
[Google's reasoning](
https://google.github.io/styleguide/cppguide.html#Exceptions)) 
Exceptions are to be used wherever they can reduce the quantity of code, and improve 
its readability.
