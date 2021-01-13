use crate::{
    clike::{
        pkgconfig::PkgConfig,
    },
    Context,
};
use ninja::{DependencyGraph, GraphError, Node};
use std::{
    borrow::Cow,
    collections::HashSet,
    ffi::OsStr,
    path::PathBuf,
};
use mapset_macros::{map,set};

mod compilation;
pub(crate) mod opts;
mod pkgconfig;
pub mod langc;
pub mod langcpp;

pub use compilation::*;
pub use opts::*;

pub struct CLikeProject<C, L> {
    pub compiler: C,
    pub linker: L,
}

impl<C: Compiler, L: Linker> CLikeProject<C, L> {
    pub fn into_graph(self, ctx: Context<BuildOptions>) -> anyhow::Result<DependencyGraph> {
        eprintln!("Context: {:#?}", ctx);
        let CLikeProject { compiler, linker } = self;
        let mut graph = DependencyGraph::default();
        let dependencies = ctx
            .system_dependencies
            .iter()
            .cloned()
            .filter_map(PkgConfig::for_library)
            .collect::<HashSet<_>>();
        let dep_flags = dependencies
            .iter()
            .flat_map(|p| {
                let mut v = p.cflags.clone();
                let name = p
                    .libname
                    .to_uppercase()
                    .chars()
                    .map(|c| match c {
                        'A'..='Z' | '0'..='9' => c,
                        _ => '_',
                    })
                    .collect::<String>();
                v.push(format!("-DHAS_{}", name));
                v
            })
            .collect::<HashSet<_>>();
        let ldflags = dependencies
            .into_iter()
            .flat_map(|p| p.libs)
            .collect::<HashSet<_>>();
        let objects = walkdir::WalkDir::new(&ctx.src_dir)
            .into_iter()
            .collect::<Result<Vec<_>, _>>()?
            .into_iter()
            .map(|f| dbg!(f).into_path())
            .filter(|f| f.extension() == Some(OsStr::new("c")))
            .map(|f| pathdiff::diff_paths(f, &ctx.dest_dir).unwrap())
            .map(|f| f.display().to_string())
            .map(|s| {
                eprintln!("file: {}", s);
                let path = PathBuf::from(&s).with_extension("o");
                let nsource = graph.add_source(s);
                let objtype = if ctx.is_library { "lib" } else { "exe" };
                let path = format!(
                    "{}.{}/{}",
                    objtype,
                    &ctx.name,
                    path.file_name().unwrap().to_string_lossy()
                );
                let nnode = graph.add_node(Node::Generated {
                    rule: compiler.as_rule(),
                    vars: map! {"flags".to_owned() => dep_flags.iter().cloned().collect()},
                    outputs: set![path.clone()],
                });
                graph.add_dependency(
                    nnode,
                    nsource,
                )?;
                Ok(nnode)
            })
            .collect::<Result<Vec<_>, GraphError>>()?;
        let entrypoint = if ctx.is_library {
            let libname = format!("{}.{}", ctx.name, if ctx.is_shared { "so" } else { "a" });
            graph.add_node(Node::Generated {
                vars: map! { "flags".to_owned() => ldflags.into_iter().collect() },
                outputs: set![libname],
                rule: linker.as_lib_rule(),
            })
        } else {
            let exename = ctx.name.clone();
            graph.add_node(Node::Generated {
                vars: map! { "flags".to_owned() => ldflags.into_iter().collect() },
                outputs: set![exename],
                rule: linker.as_exe_rule(),
            })
        };
        graph.add_dependencies(entrypoint, objects);
        Ok(graph)
    }
}
