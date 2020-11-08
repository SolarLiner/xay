# Project configuration (for C++)

## `name`

Name the project without renaming the folder.

## `system-dependencies`

List all dependencies from the system the project needs to build. **All dependencies are
optional by default**; you need to explicitely halt the build through the preprocessor if
the dependency is mandatory.

## `extra-build-flags`

Extraneous flags that will be added to the compiler call.

## `extra-link-flags`

Extraneous flags that will be added to the linker call.