use ninja::NinjaAst;

use crate::{
    clike::{
        compilation::GCC,
        opts::{BuildOptions, BuildOptionsFile},
        CLikeProject,
    },
    utils::find_binary,
    Context,
};

pub fn handle_project(ctx: Context<BuildOptionsFile>) -> anyhow::Result<NinjaAst> {
    let default_name = ctx.name.clone();
    //let fallback_gcc = || GCC::new(find_binary("gcc").or_else(anyhow::anyhow!("GCC not found")));
    let fallback_gcc = find_binary("gcc").map(GCC::new);
    let compiler = GCC::from_env("cc")
        .ok_or(anyhow::anyhow!(""))
        .or_else(|_| fallback_gcc)?;
    CLikeProject {
        compiler: compiler.clone(),
        linker: GCC::from_env("ld").unwrap_or(compiler)
    }
    .gen_ast(ctx.map_inner(|f| BuildOptions::from_file(f, default_name)))
}
