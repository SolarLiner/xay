use std::borrow::{Cow, Borrow};
use std::ffi::OsStr;
use std::path::{PathBuf};

use ninja::{NinjaAst};

use crate::clike::ast::{Executable, Library, Object,  Source};
use crate::clike::opts::{BuildOptions};
use crate::clike::pkgconfig::PkgConfig;
use compilation::{Compiler, Linker};
use crate::Context;

pub mod ast;
pub mod compilation;
pub mod langc;
pub mod langcpp;
pub mod opts;
pub mod pkgconfig;

#[derive(Clone, Debug)]
pub struct CLikeProject<C, L> {
    pub compiler: C,
    pub linker: L,
}

impl<C: Compiler + Clone, L: Linker + Clone> CLikeProject<C, L> {
    pub fn gen_ast(self, ctx: Context<BuildOptions>) -> anyhow::Result<NinjaAst> {
        let CLikeProject { compiler, linker } = self;
        let compiler: Cow<C> = Cow::Owned(compiler);
        let linker: Cow<L> = Cow::Owned(linker);
        let dependencies: Vec<_> = ctx
            .system_dependencies
            .iter()
            .cloned()
            .filter_map(PkgConfig::for_library)
            .collect();
        let dep_flags: Vec<_> = dependencies
            .iter()
            .flat_map(|p| {
                let mut v = p.cflags.clone();
                let flag_lib_name = p
                    .libname
                    .to_uppercase()
                    .chars()
                    .map(|c| match c {
                        'A'..='Z' | '0'..='9' => c,
                        _ => '_',
                    })
                    .collect::<String>();
                v.push(format!("-DHAS_{}", flag_lib_name));
                v
            })
            .collect();
        let libraries: Vec<_> = dependencies.into_iter().flat_map(|p| p.libs).collect();
        let sources = walkdir::WalkDir::new(&ctx.src_dir)
            .into_iter()
            .flatten()
            .map(|f| f.into_path())
            .filter(|f| f.extension() == Some(OsStr::new("c")))
            .map(|f| pathdiff::diff_paths(f, &ctx.dest_dir).unwrap())
            .map(Source);
        let objects: Vec<_> = sources
            .map(|s| {
                let Source(src) = &s;
                let obj_type = if ctx.is_library { "lib" } else { "exe" };
                let mut obj = Object::new(
                    Cow::clone(&compiler),
                    PathBuf::from(format!("{}.{}", obj_type, &ctx.name))
                        .join(src.with_extension("o").file_name().unwrap())
                        .to_string_lossy(),
                    s,
                );
                obj.flags.extend(dep_flags.iter().cloned());
                obj.flags
                    .extend(ctx.extra_build_flags.iter().cloned());
                obj
            })
            .collect();
        if objects.len() == 0 {
            anyhow::bail!("No source files were found");
        } else {
            let Context {
                name,
                dest_dir,
                inner: BuildOptions {
                    extra_link_flags,
                    is_shared,
                    is_library,
                    ..
                },
                ..
            } = ctx;
            if is_library {
                let mut linker = Cow::clone(&linker);
                linker.set_shared(is_shared);
                let exe = Executable {
                    name,
                    objects,
                    libraries: vec![], // TODO: Add source dependencies support
                    flags: vec![libraries, extra_link_flags].concat(),
                    linker,
                };

                Ok(NinjaAst::Default(Box::new(exe.into())))
            } else {
                let mut linker = Cow::clone(&linker);
                linker.set_shared(is_shared);

                let lib = Library {
                    name,
                    tgt_dir: dest_dir,
                    objects,
                    libraries: vec![],
                    flags: vec![libraries, extra_link_flags].concat(),
                    linker,
                };

                Ok(NinjaAst::Default(Box::new(lib.into())))
            }
        }
    }
}
