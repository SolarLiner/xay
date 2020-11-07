use ninja::NinjaAst;
use std::path::{PathBuf, Path};
use crate::clike::{Compiler, Linker};
use crate::utils::shell_join;

macro_rules! map(
    { $($key:expr => $value:expr),+ } => {
        {
            let mut m = ::std::collections::HashMap::new();
            $(
                m.insert($key, $value);
            )+
            m
        }
     };
);

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct Source(pub PathBuf);

impl Into<NinjaAst> for Source {
    fn into(self) -> NinjaAst {
        NinjaAst::Source(self.0.into_os_string().to_string_lossy().into())
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct Object<C> {
    pub name: String,
    pub source: Source,
    pub flags: Vec<String>,
    pub compiler: C,
}

impl<C: Compiler> Into<NinjaAst> for Object<C> {
    fn into(self) -> NinjaAst {
        NinjaAst::Generated {
            name: self.name,
            rule: self.compiler.into_rule(),
            deps: vec![self.source.into()],
            vars: map! {
                "flags".to_owned() => shell_join(self.flags.into_iter())
            },
        }
    }
}

impl<C> Object<C> {
    pub fn new<S: AsRef<str>>(compiler: C, name: S, source: Source) -> Self {
        Self {
            compiler,
            name: name.as_ref().to_owned(),
            source,
            flags: vec![],
        }
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct Library<C, L> {
    pub name: String,
    pub tgt_dir: PathBuf,
    pub objects: Vec<Object<C>>,
    pub libraries: Vec<Library<C, L>>,
    pub flags: Vec<String>,
    pub linker: L,
}

impl<C: Compiler, L: Linker> Into<NinjaAst> for Library<C, L> {
    fn into(mut self) -> NinjaAst {
        let mut lib_dirs: Vec<_> = self.libraries.iter().map(|l| &l.tgt_dir).collect();
        lib_dirs.dedup();
        self.linker.add_library_dirs(lib_dirs.into_iter());
        self.linker.set_position_independent(true);
        NinjaAst::Generated {
            name: self.name,
            rule: self.linker.into_lib_rule(),
            deps: {
                let mut asts = self
                    .libraries
                    .into_iter()
                    .map(|mut l| l.into())
                    .collect::<Vec<NinjaAst>>();
                asts.extend(self.objects.into_iter().map(|o| o.into()));
                asts
            },
            vars: map! {
                "flags".to_owned() => shell_join(self.flags.into_iter())
            },
        }
    }
}

impl<C, L> Library<C, L> {
    fn new<S: Into<String>, P: AsRef<Path>>(linker: L, name: S, tgt_dir: P) -> Self {
        Self {
            linker,
            name: name.into(),
            tgt_dir: tgt_dir.as_ref().to_owned(),
            objects: vec![],
            libraries: vec![],
            flags: vec![],
        }
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) struct Executable<C, L> {
    pub name: String,
    pub objects: Vec<Object<C>>,
    pub libraries: Vec<Library<C, L>>,
    pub flags: Vec<String>,
    pub linker: L,
}

impl<C: Compiler, L: Linker> Into<NinjaAst> for Executable<C, L> {
    fn into(mut self) -> NinjaAst {
        let mut lib_dirs: Vec<_> = self.libraries.iter().map(|l| &l.tgt_dir).collect();
        lib_dirs.dedup();
        self.linker.add_library_dirs(lib_dirs.into_iter());
        NinjaAst::Generated {
            name: self.name,
            rule: self.linker.into_exe_rule(),
            deps: {
                let mut asts: Vec<NinjaAst> =
                    self.libraries.into_iter().map(|l| l.into()).collect();
                asts.extend(self.objects.into_iter().map(|o| o.into()));
                asts
            },
            vars: map! {
                "flags".to_owned() => shell_join(self.flags.into_iter())
            },
        }
    }
}