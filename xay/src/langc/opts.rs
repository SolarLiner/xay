use std::path::PathBuf;

use serde::Deserialize;

#[derive(Clone, Debug, Deserialize)]
#[serde(deny_unknown_fields, rename_all = "kebab-case")]
pub struct BuildOptionsFile {
    name: Option<String>,
    system_dependencies: Option<Vec<String>>,
    extra_build_flags: Option<Vec<String>>,
    extra_link_flags: Option<Vec<String>>,
}

#[derive(Clone, Debug)]
pub struct BuildOptions {
    pub name: String,
    pub system_dependencies: Vec<String>,
    pub extra_build_flags: Vec<String>,
    pub extra_link_flags: Vec<String>,
}

impl BuildOptions {
    pub fn new(name: String) -> Self {
        Self {
            name,
            system_dependencies: vec![],
            extra_build_flags: vec![],
            extra_link_flags: vec![],
        }
    }
    pub fn from_file(opts: BuildOptionsFile, default_name: String) -> Self {
        Self {
            name: opts.name.unwrap_or(default_name),
            system_dependencies: opts.system_dependencies.unwrap_or(vec![]),
            extra_build_flags: opts.extra_build_flags.unwrap_or(vec![]),
            extra_link_flags: opts.extra_link_flags.unwrap_or(vec![]),
        }
    }
}

#[derive(Clone, Debug)]
pub struct ProcessOptions {
    pub build_opts: BuildOptions,
    pub src_dir: PathBuf,
    pub dest_dir: PathBuf,
}
