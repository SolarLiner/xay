# `xayc`-specific CLI options

## The `CC` environment variable

`xayc` expects the compiler to accept GCC flags and options. By default `xayc` will output building rules using gcc regardless of system availability; a system to check and select compiler with regards to availability still needs to be implemented.

The `CC` variable, if present, changes the compiler used, **but not the flags passed to it**.

## The `LD` environment variable

Similar to `CC`, `LD` can be used to specify the linker program. Again, GCC flags need to be accepted by the linker, as by default `gcc` is also used.