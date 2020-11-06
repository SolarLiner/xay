# xay
An automatic build system for 95% of projects.

```bash
# (Soon™)
cargo install xay
```

## Description

`xay` is a build system that relies on *convention* over *configuration*.
Instead of telling it how to build your project, it expects you to follow rules
on how to set your project up for it to build it.

What you lose in flexibility, you gain in ease of use. `xay` has been designed
to work on simpler projects with a single line of configuration (the source code
language). You can add source dependencies with no configuration changes, and
system dependencies easily with a single line of configuration.

The approach of *convention over configuration* means that `xay` will not be
suitable for the biggest projects, or those requiring bespoke build steps.
However the build systems that provide those features are often overkill for
other projects.

One of the far-reaching milestones of the project is to have `xay` build itself.
This might be hard to do with the dependencies that `xay` itself uses.

## Documentation

You can find the documentation at
[solarliner.github.io/xay](https://solarliner.github.io/xay)
