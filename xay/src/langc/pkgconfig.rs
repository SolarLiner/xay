use std::{collections::HashMap, process::Command};

#[derive(Clone, Debug)]
pub struct PkgConfig {
    pub libname: String,
    pub cflags: Vec<String>,
    pub libs: Vec<String>,
    pub variables: HashMap<String, String>,
}

impl PkgConfig {
    pub fn for_library<S: AsRef<str>>(name: S) -> Option<Self> {
        let name = name.as_ref();
        if Self::exists(name) {
            Some(Self {
                libname: name.to_string(),
                cflags: shlex::split(&Self::get_value(name, "cflags").unwrap()).unwrap(),
                libs: shlex::split(&Self::get_value(name, "libs").unwrap()).unwrap(),
                variables: Self::get_variables(name),
            })
        } else {
            None
        }
    }

    fn cmd() -> Command {
        Command::new(Self::get_program())
    }

    fn get_program() -> String {
        std::env::var("PKG_CONFIG")
            .ok()
            .unwrap_or("pkg-config".to_owned())
    }

    fn exists(name: &str) -> bool {
        if let Ok(s) = Self::cmd().arg("--exists").arg(name).status() {
            s.success()
        } else {
            false
        }
    }

    fn get_value(library: &str, name: &str) -> Option<String> {
        if let Ok(out) = Self::cmd().arg(format!("--{}", name)).arg(library).output() {
            String::from_utf8(out.stdout)
                .map(|s| s.trim().to_string())
                .ok()
        } else {
            None
        }
    }

    fn get_variables(library: &str) -> HashMap<String, String> {
        let vars = Self::get_value(library, "print-variables")
            .map(|s| s.split('\n').map(|s| s.to_owned()).collect())
            .unwrap_or(vec![]);
        let mut map = HashMap::new();
        for var in vars {
            map.insert(
                var.clone(),
                Self::get_value(library, &format!("variable={}", var)).unwrap(),
            );
        }
        map
    }
}
