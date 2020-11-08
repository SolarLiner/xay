# CLI usage

!!! note
    There may be more options specialized for each language `xay` support.

## Global options

    USAGE:
        xay [OPTIONS] [SUBCOMMAND]
    
    FLAGS:
        -h, --help       Prints help information
        -V, --version    Prints version information
    
    OPTIONS:
            --config <config>    Sets the path to the configuration file, for cases where the file name might be non-
                                 standard or that the project has several configuration files [default: xay.yml]
        -d, --dest <dest>        Sets the destination folder, the path to the build artifacts [default: build]
        -C <path>                Sets the project folder [default: .]
    
    SUBCOMMANDS:
        build       Generate the build files and build the project
        generate    Generate the build directory for ninja to build the project
        help        Prints this message or the help of the given subcommand(s)
        run         Generate the build files, build the project and run the executable (implies artifactis executable)


## `xay` or `xay generate`

Generate Ninja build files for compilation.

## `xay build`

Generate Ninja build files and compile the project.

## `xay run`

Generate Ninja build files, compile the project, and run the executable.

!!! note
    This command only works with executable projects.