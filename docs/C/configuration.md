# Project configuration (for C)

The configuration is a YAML file containing the following (optional) set of keys:

## `name`

Name the project without renaming the folder.

## `system-dependencies`

List all dependencies from the system the project needs to build. **All dependencies are optional by default**; you need to explicitely halt the build through the preprocessor if the dependency is mandatory.

## `extra_build_flags`

Extraneous flags that will be added to the compiler call.

## `extra_link_flags`

Extraneous flags that will be added to the linker call.