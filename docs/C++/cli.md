# CLI options specific to C++ projects

## The `CXX` environment variable

`xay` only supports `gcc` and `clang` at the moment. Other compilers may be added
in the future.

The `CXX` variable, if present, changes the compiler used. The default is to use
`gcc` first, then trying for `clang`.

## The `LD` environment variable

Similar to `CC`, `LD` can be used to specify the linker program. Again, GCC flags
need to be accepted by the linker, as by default `gcc` (or `clang` if it is not
found) is also used.