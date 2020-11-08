# Welcome to xây

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

## Non-goals

xây does not pretend to be a one-size-fits-all solution. Large projects will have convoluted build systems
and may not be able to fit within the conventions described below. However the author of this project believes
that many projects not only can fit the conventions, but will benefit from it also.  
What makes xây powerful and easy to use is also its Achille's heel: it's relative lack of configurability from
its rigid conventions.
