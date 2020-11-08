# Getting started (C++ Projects)

xây doesn't require any dependencies to execute, but needs `ninja` to actually perform the build.

## New project

1. In an empty directory, create a `src` folder. Write your source code in it. In general, make sure to adhere
 to [xây's conventions].
1. If you have source code dependencies, add them to the `lib` folder.
1. If you have system dependencies, add them to `xay.yml` as follows:
```yaml
language: cpp
system-dependencies:
  - <dependency>
  - <dependency>
  - ...
```

Each system dependency found will add a define `HAS_<DEPENDENCY>` (cf. [Technicalities]). This allows you to use the preprocessor to conditionally include code, or to error out the build altogether.


1. When ready to build, execute the command `xay`. A `build` folder
 has been created with a Ninja build definition file.
1. Execute `ninja` in the build directory to build your files.

Alternatively, use the [`build` subcommand] to both generate and build the project at once.

## Existing project

1. Make sure to adhere to [xây's conventions]. This might require a restructure of the project.
2. When ready to build, execute the command `xay`. A `build` folder
 has been created with a Ninja build definition file.
1. Execute `ninja` in the build directory to build your files.

## Technicalities

* `xay` sanitizes library names when passing them as defines to the compiler. The following rules
  are followed:
  
  - The library name is uppercased, to follow define naming conventions.
  - Alphanumeric characters are preserved as-is
  - All other characters are replaced by underscores.
  
  This means that, for example, specifying the library `cairo-svg` as system dependency will add a
  get the following word defined during compilation: `HAS_CAIRO_SVG`. 

[Technicalities]: #technicalities
[xây's conventions]: /conventions
[`build` subcommand]: /cli