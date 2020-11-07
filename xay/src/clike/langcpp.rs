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
        .gen_ast(ctx.map_inner(|f| BuildOptions::from_file(f, default_name)))
}