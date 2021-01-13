use std::path::PathBuf;

use serde::Deserialize;

#[derive(Clone, Debug, Deserialize)]
pub enum BuildType {
    #[serde(rename="shared executable")]
    SharedExecutable,
    #[serde(rename="shared library")]
    SharedLibrary,
    #[serde(rename="static executable")]
    StaticExecutable,
    #[serde(rename="static library")]
    StaticLibrary,
}

impl Default for BuildType {
    fn default() -> Self {
        Self::SharedExecutable
    }
}

#[derive(Clone, Debug, Deserialize)]
#[serde(deny_unknown_fields, rename_all = "kebab-case")]
pub struct BuildOptionsFile {
    pub name: Option<String>,
    #[serde(rename="type", default)]
    pub ty: BuildType,
    #[serde(default)]
    pub system_dependencies: Vec<String>,
    #[serde(default)]
    pub extra_build_flags: Vec<String>,
    #[serde(default)]
    pub extra_link_flags: Vec<String>,
}

impl Default for BuildOptionsFile {
    fn default() -> Self {
        Self {
            ty: BuildType::SharedExecutable,
            name: None,
            system_dependencies: vec![],
            extra_build_flags: vec![],
            extra_link_flags: vec![],
        }
    }
}

#[derive(Clone, Debug)]
pub struct BuildOptions {
    pub name: String,
    pub is_shared: bool,
    pub is_library: bool,
    pub system_dependencies: Vec<String>,
    pub extra_build_flags: Vec<String>,
    pub extra_link_flags: Vec<String>,
}

impl BuildOptions {
    pub fn new(name: String) -> Self {
        Self {
            name,
            is_shared: true,
            is_library: false,
            system_dependencies: vec![],
            extra_build_flags: vec![],
            extra_link_flags: vec![],
        }
    }
    pub fn from_file(opts: BuildOptionsFile, default_name: String) -> Self {
        let (is_shared, is_library) = match opts.ty {
            BuildType::SharedExecutable => (true, false),
            BuildType::SharedLibrary => (true, true),
            BuildType::StaticExecutable => (false, false),
            BuildType::StaticLibrary => (false, true),
        };
        Self {
            name: opts.name.unwrap_or(default_name),
            is_shared,
            is_library,
            system_dependencies: opts.system_dependencies,
            extra_build_flags: opts.extra_build_flags,
            extra_link_flags: opts.extra_link_flags,
        }
    }
}

#[derive(Clone, Debug)]
pub struct ProcessOptions {
    pub build_opts: BuildOptions,
    pub src_dir: PathBuf,
    pub dest_dir: PathBuf,
}
