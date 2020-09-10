# Getting started (C Projects)

xây doesn't require any dependencies to execute, but needs `ninja` to actually perform the build.

## New project

1. In an empty directory, create a `src` folder. Write your source code in it. In general, make sure to adhere
 to [xây's conventions].
1. If you have source code dependencies, add them to the `mod` folder.
1. If you have system dependencies, add them to `xayc.yml` as follows:
```yaml
system-dependencies:
  - <dependency>
  - <dependency>
  - ...
```
  Each system dependency found will add a define `HAS_<DEPENDENCY>`. This allows you to use the preprocessor to conditionally include code, or to error out the build altogether.
1. When ready to build, execute the command `xay`. A `build` folder
 has been created with a Ninja build definition file.
1. Execute `ninja` in the build directory to build your files.

Alternatively, use the [`build` subcommand] to both generate and build the project at once.

## Existing project

1. Make sure to adhere to [xây's conventions]. This might require a restructure of the project.
2. When ready to build, execute the command `xay`. A `build` folder
 has been created with a Ninja build definition file.
1. Execute `ninja` in the build directory to build your files.

[xây's conventions]: /conventions
[`build` subcommand]: /cli