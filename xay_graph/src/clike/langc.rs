use crate::Context;
use crate::clike::{BuildOptionsFile, GCC, CLikeProject, BuildOptions};
use ninja::DependencyGraph;
use crate::utils::find_binary;

pub fn handle_project(ctx: Context<BuildOptionsFile>) -> anyhow::Result<DependencyGraph> {
    let default_name = ctx.name.clone();
    let fallback_gcc = find_binary("gcc").map(GCC::new);
    let compiler = GCC::from_env("cc")
        .ok_or(anyhow::anyhow!(""))
        .or_else(|_| fallback_gcc)?;
    CLikeProject {
        compiler: compiler.clone(),
        linker: GCC::from_env("ld").unwrap_or(compiler)
    }
        .into_graph(ctx.map_inner(|f| BuildOptions::from_file(f, default_name)))
}