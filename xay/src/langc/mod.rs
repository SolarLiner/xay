pub mod opts;
mod pkgconfig;

use pkgconfig::PkgConfig;
use std::{ffi::OsStr, path::PathBuf};

use ninja::NinjaAst;

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
pub struct Source(pub PathBuf);

impl Into<NinjaAst> for Source {
    fn into(self) -> NinjaAst {
        NinjaAst::Source(self.0.into_os_string().to_string_lossy().into())
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Object {
    pub name: String,
    pub source: Source,
    pub flags: Vec<String>,
}

impl Into<NinjaAst> for Object {
    fn into(self) -> NinjaAst {
        NinjaAst::Generated {
            name: self.name,
            rule: build_rule(),
            deps: vec![self.source.into()],
            vars: map! {
                "flags".to_owned() => join(self.flags.into_iter())
            },
        }
    }
}

impl Object {
    pub fn new(name: String, source: Source) -> Self {
        Self {
            name,
            source,
            flags: vec![],
        }
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub enum Sharedness {
    Static,
    Shared,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Library {
    pub name: String,
    pub sharedness: Sharedness,
    pub objects: Vec<Object>,
    pub libraries: Vec<Library>,
    pub flags: Vec<String>,
}

impl Into<NinjaAst> for Library {
    fn into(self) -> NinjaAst {
        let mut flags = self.flags;
        match self.sharedness {
            Sharedness::Shared => (),
            Sharedness::Static => flags.push("-static".to_owned()),
        }
        NinjaAst::Generated {
            name: self.name,
            rule: build_rule(),
            deps: {
                let mut asts = self
                    .libraries
                    .into_iter()
                    .map(|mut l| {
                        l.flags.push("-fPIC".to_owned());
                        l.into()
                    })
                    .collect::<Vec<NinjaAst>>();
                asts.extend(self.objects.into_iter().map(|o| o.into()));
                asts
            },
            vars: map! {
                "flags".to_owned() => join(flags.into_iter())
            },
        }
    }
}

impl Library {
    fn new<S: Into<String>>(name: S, sharedness: Sharedness) -> Self {
        Self {
            sharedness,
            name: name.into(),
            objects: vec![],
            libraries: vec![],
            flags: vec![],
        }
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Executable {
    pub sharedness: Sharedness,
    pub name: String,
    pub objects: Vec<Object>,
    pub libraries: Vec<Library>,
    pub flags: Vec<String>,
}

impl Into<NinjaAst> for Executable {
    fn into(self) -> NinjaAst {
        let mut flags = self.flags;
        match self.sharedness {
            Sharedness::Shared => (),
            Sharedness::Static => flags.push("-static".to_owned()),
        }
        NinjaAst::Generated {
            name: self.name,
            rule: link_rule(),
            deps: {
                let mut asts: Vec<NinjaAst> =
                    self.libraries.into_iter().map(|l| l.into()).collect();
                asts.extend(self.objects.into_iter().map(|o| o.into()));
                asts
            },
            vars: map! {
                "flags".to_owned() => join(flags.into_iter())
            },
        }
    }
}

pub fn get_build_ast(opts: opts::ProcessOptions) -> std::io::Result<NinjaAst> {
    let dependencies = opts
        .build_opts
        .system_dependencies
        .iter()
        .cloned()
        .filter_map(PkgConfig::for_library)
        .collect::<Vec<_>>();
    let cflags = dependencies
        .iter()
        .flat_map(|p| {
            let mut v = p.cflags.clone();
            v.push(format!(
                "-DHAS_{}",
                p.libname
                    .to_uppercase()
                    .chars()
                    .map(|c| match c {
                        'A'..='Z' => c,
                        '0'..='9' => c,
                        _ => '_',
                    })
                    .collect::<String>()
            ));
            v
        })
        .collect::<Vec<_>>();
    let ldflags = dependencies
        .into_iter()
        .flat_map(|p| p.libs)
        .collect::<Vec<_>>();
    let sources = walkdir::WalkDir::new(&opts.src_dir)
        .into_iter()
        .flatten()
        .map(|f| f.into_path())
        .filter(|f| f.extension() == Some(OsStr::new("c")))
        .map(|f| pathdiff::diff_paths(f, &opts.dest_dir).unwrap())
        .map(Source);
    let objects: Vec<Object> = sources
        .map(|s| {
            let Source(src) = &s;
            let mut obj = Object::new(
                PathBuf::from(format!("exe.{}", &opts.build_opts.name))
                    .join(src.with_extension("o").file_name().unwrap())
                    .to_string_lossy()
                    .into(),
                s,
            );
            obj.flags.extend(cflags.iter().cloned());
            obj.flags
                .extend(opts.build_opts.extra_build_flags.iter().cloned());
            obj
        })
        .collect();
    if objects.len() == 0 {
        return Err(std::io::Error::new(
            std::io::ErrorKind::InvalidData,
            "No source files found".to_owned(),
        ));
    }
    let exe = Executable {
        sharedness: Sharedness::Shared,
        name: opts.build_opts.name,
        objects,
        libraries: vec![],
        flags: vec![ldflags, opts.build_opts.extra_link_flags].concat(),
    };

    Ok(NinjaAst::Default(Box::new(exe.into())))
}

fn build_rule() -> ninja::Rule {
    ninja::Rule::new(
        "cc".to_owned(),
        format!("{} -MD -MMD $flags -c $in -o $out", get_cc(),),
    )
    .set_description("Compiling file $in".to_owned())
    .set_depfile("$out.d".to_owned())
}

fn link_rule() -> ninja::Rule {
    ninja::Rule::new(
        "link".to_owned(),
        format!("{} -MD -MMD $flags $in -o $out", get_ld(),),
    )
    .set_description("Linking to $out".to_owned())
    .set_depfile("$out.d".to_owned())
}

fn get_cc() -> String {
    std::env::var("CC").ok().unwrap_or("gcc".to_owned())
}

fn get_ld() -> String {
    std::env::var("LD")
        .or_else(|_| std::env::var("CC"))
        .ok()
        .unwrap_or("gcc".to_owned())
}

fn join<I: Iterator<Item = String>>(values: I) -> String {
    values
        .map(|s| shlex::quote(&s).into())
        .collect::<Vec<String>>()
        .join(" ")
}
