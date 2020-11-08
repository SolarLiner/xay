# Conventions

To make using xây simple, the project sets up a series of convention that all projects must follow. In
addition, plugins may enforce further language or framework-related conventions.

## Global conventions

### One project = one public artifact

xây's convention is to keep things simple. Therefore any project corresponds to one public artifact.
This can be multiple files (for example, a TypeScript projects compiles to JavaScript along with source
maps and a definition file), however a project does not generate multiple artifacts. 

### Sources in the source folder

Sources files for a project will only be searched in the `src` folder at the root of the project. Other source
files may be in subfolders, however source files outside that folder will not be included in the build.

### Dependencies are a tree of projects.


!!! warning "Unimplemented feature"
    This is an unimplemented feature as of now, but is planned to be added in the future, and will be
    available by the first stable release.

    
xây provides means to depend on other source code, provided they also are projects. This means you can easily
reuse source code in different projects.

Source dependencies go in the `lib` folder at the root of the project, with every direct dependency as a child
of that folder. For example, a `foo` executable might have dependencies on `libfoo` and `argparse`. The
directory structure will be as follows:

    - foo/
      - src/
        - main.c
      - lib/
        - libfoo/
          - include/   # Plugins can add supported directories, such as an include directory for C projects
            - foo.h
          - lib/
            - <indirect dependencies>
          - src/
            - foo.c
        - argparse/
          - src/       -> git submodule linking to https://github.com/cofyc/argparse.git

This allows xây projects to be immediately reusable in other projects.