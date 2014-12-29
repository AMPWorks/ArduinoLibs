Debugging Print Macros
======================

This library contains a number of debugging macros which can be enabled by 
defining DEBUG_LEVEL in a given source file.  Most of these are print statements,
which will put the text of the statement in program memory (with the Arduino
environment the default for character arrays is to put them in the much more
limited RAM), and so enabling these macros will reduce the available space for
a program.  See the example for demonstration of how these can be used.


Enabling via compiler environment
---------------------------------

These debugging macros can also be enabled via environment variables passed to
the compiler, either from the command line for command line compilation or via
the build_flags parameter in a platformio configuration file.

If compiling with platformio, add the following to your platformio.ino to
globally set the debugging print level:

```
build_flags = "-D DEBUG_LEVEL=<int>"
```

Where the integer value is the level of debugging to enable.  This can also
be set on a per-file basis if something along these lines is included in the
source file:

```c
#ifdef DEBUG_LEVEL_FILENAME
  #define DEBUG_LEVEL DEBUG_LEVEL_FILENAME
#endif
```

And put this in the platformio.ino:

```
build_flags = "-D DEBUG_LEVEL_FILENAME=<int>
```

Future improvements
-------------------

* Expand the #def separation in Debug.h to only define the levels currently
being used.
