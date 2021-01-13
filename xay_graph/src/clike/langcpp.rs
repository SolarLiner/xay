use crate::Context;
use crate::clike::{BuildOptionsFile, GCC, CLikeProject, BuildOptions};
use ninja::DependencyGraph;
use crate::utils::find_binary;

pub fn handle_project(ctx: Context<BuildOptionsFile>) -> anyhow::Result<DependencyGraph> {
    let default_name = ctx.name.clone();
    let fb_cc = find_binary("gcc").map(GCC::new);
    let fb_ld = find_binary("gcc").map(GCC::new);
    CLikeProject {
        compiler: GCC::from_env("cxx")
            .ok_or(anyhow::anyhow!(""))
            .or(fb_cc)?,
        linker: GCC::from_env("ld")
            .or_else(|| GCC::from_env("cxx"))
            .ok_or(anyhow::anyhow!(""))
            .or(fb_ld)?,
    }
        .into_graph(ctx.map_inner(|f| BuildOptions::from_file(f, default_name)))
}