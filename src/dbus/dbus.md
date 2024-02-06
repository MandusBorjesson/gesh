# D-Bus handling

The D-Bus handling in gesh is responsible for isolating the core logic of the
setting handling from the surrounding world and all of its complications. The
idea is that the D-Bus handling should be possible to exchange with relative
ease and without touching other parts of the code.

## Type conversion
The D-Bus handler is responsible for converting the internal `setting_t` type
to the types used by `sdbus-cpp`. If more handlers are added they are expected
to manage the conversion to their type systems as well as any return values and
exceptions thrown by the setting handling similarly to how the current
implementation handles it.
