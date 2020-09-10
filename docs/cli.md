# CLI usage

In the following section, replace `xay` by `Dung.CLI` if using binaries compiled from source.

## Global options

    <path>      Path to the project (default: current directory)
    <dest>      Destination folder of the built files (relative to project dir)
                (default: "build")
    -c --config Path to the configuration file (default: xayc.yml)

## `xay` or `xay generate`

Generate Ninja build files for compilation.

## `xay build`

Generate Ninja build files and compile the project.

## `xay run`

Generate Ninja build files, compile the project, and run the executable.

!!! note
    This command only works with executable projects.