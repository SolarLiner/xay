use std::path::{PathBuf, Path};

#[cfg(not(target_os = "windows"))]
const PATHSEP: char = ':';

#[cfg(target_os = "windows")]
const PATHSEP: char = ';';

pub fn find_binary(name: &str) -> anyhow::Result<PathBuf> {
    let path = std::env::var("PATH").map_err(|err| anyhow::anyhow!(err))?;
    for p in path.split(PATHSEP) {
        let path = Path::new(p).join(name);
        if path.exists() {
            return Ok(path.to_owned());
        }
    }
    anyhow::bail!("Command {} not found in PATH.", name)
}

pub fn shell_join<I: Iterator<Item = String>>(values: I) -> String {
    values
        .map(|s| shlex::quote(&s).into())
        .collect::<Vec<String>>()
        .join(" ")
}