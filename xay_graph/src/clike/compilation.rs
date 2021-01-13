use std::borrow::{BorrowMut, Cow, Borrow};
use std::path::{Path, PathBuf};

use ninja::Rule;

use crate::utils::find_binary;

pub trait Compiler {
    fn add_include_dirs<I: Iterator<Item = impl AsRef<Path>>>(&mut self, dirs: I);
    fn set_opt(&mut self, level: u8);
    fn set_warning_level(&mut self, level: u8);
    fn as_rule(&self) -> Rule;
}

impl<C: Compiler + Clone> Compiler for Cow<'_, C> {
    fn add_include_dirs<I: Iterator<Item = impl AsRef<Path>>>(&mut self, dirs: I) {
        C::add_include_dirs(self.to_mut(), dirs)
    }

    fn set_opt(&mut self, level: u8) {
        C::set_opt(self.to_mut(), level)
    }

    fn set_warning_level(&mut self, level: u8) {
        C::set_warning_level(self.to_mut(), level)
    }

    fn as_rule(&self) -> Rule {
        C::as_rule(Cow::borrow(self))
    }
}

impl<C: Compiler> Compiler for Box<C> {
    fn add_include_dirs<I: Iterator<Item=impl AsRef<Path>>>(&mut self, dirs: I) {
        C::add_include_dirs(self.as_mut(), dirs)
    }

    fn set_opt(&mut self, level: u8) {
        C::set_opt(self.as_mut(), level)
    }

    fn set_warning_level(&mut self, level: u8) {
        C::set_warning_level(self.as_mut(), level)
    }

    fn as_rule(&self) -> Rule {
        C::as_rule(self)
    }
}

pub trait Linker {
    fn add_library_dirs<I: Iterator<Item = impl AsRef<Path>>>(&mut self, dirs: I);
    fn set_shared(&mut self, is_shared: bool);
    fn set_position_independent(&mut self, pic: bool);
    fn as_lib_rule(&self) -> Rule;
    fn as_exe_rule(&self) -> Rule;
}

impl<L: Linker + Clone> Linker for Cow<'_, L> {
    fn add_library_dirs<I: Iterator<Item = impl AsRef<Path>>>(&mut self, dirs: I) {
        L::add_library_dirs(self.to_mut(), dirs)
    }

    fn set_shared(&mut self, is_shared: bool) {
        L::set_shared(self.to_mut(), is_shared)
    }

    fn set_position_independent(&mut self, pic: bool) {
        L::set_position_independent(self.to_mut(), pic)
    }

    fn as_lib_rule(&self) -> Rule {
        L::as_lib_rule(self)
    }

    fn as_exe_rule(&self) -> Rule {
        L::as_exe_rule(self)
    }
}

impl<L: Linker> Linker for Box<L> {
    fn add_library_dirs<I: Iterator<Item=impl AsRef<Path>>>(&mut self, dirs: I) {
        L::add_library_dirs(self.as_mut(), dirs)
    }

    fn set_shared(&mut self, is_shared: bool) {
        L::set_shared(self.as_mut(), is_shared)
    }

    fn set_position_independent(&mut self, pic: bool) {
        L::set_position_independent(self.as_mut(), pic)
    }

    fn as_exe_rule(&self) -> Rule {
        L::as_exe_rule(self)
    }

    fn as_lib_rule(&self) -> Rule {
        L::as_lib_rule(self)
    }
}

#[derive(Clone, Debug)]
pub struct GCC {
    path: PathBuf,
    inc_dirs: Vec<PathBuf>,
    lib_dirs: Vec<PathBuf>,
    is_shared: bool,
    opt: u8,
    warnings: Vec<String>,
    is_werror: bool,
    is_pic: bool,
}

impl GCC {
    pub fn from_env(var: &str) -> Option<Self> {
        let cmd = std::env::var(var).ok()?;
        match cmd.as_str() {
            "gcc" | "clang" => find_binary(&cmd).ok().map(GCC::new),
            _ => None,
        }
    }

    pub fn new<P: AsRef<Path>>(path: P) -> Self {
        Self {
            path: path.as_ref().to_owned(),
            inc_dirs: vec![],
            lib_dirs: vec![],
            is_shared: true,
            opt: 0,
            warnings: vec![],
            is_werror: false,
            is_pic: false,
        }
    }
}

impl Compiler for GCC {
    fn add_include_dirs<I: Iterator<Item = impl AsRef<Path>>>(&mut self, dirs: I) {
        self.inc_dirs.extend(dirs.map(|p| p.as_ref().to_owned()));
    }

    fn set_opt(&mut self, level: u8) {
        self.opt = level;
    }

    fn set_warning_level(&mut self, level: u8) {
        let mut w = vec![];
        if level > 1 {
            w.push("all".to_string());
        }
        if level > 2 {
            w.push("extra".to_string());
        }
        self.warnings = w;
    }

    fn as_rule(&self) -> Rule {
        let mut command = self.path.display().to_string();
        for w in &self.warnings {
            command.push_str(&format!(" -W{}", w));
        }
        for d in &self.inc_dirs {
            command.push_str(&format!(" -I{}", d.display()));
        }
        if !self.is_shared {
            command.push_str(" -static");
        }
        if self.opt > 0 {
            command.push_str(&format!(" -O{}", self.opt));
        }
        if self.is_werror {
            command.push_str(" -Werror");
        }
        command.push_str(" -MD -MMD $flags -c -o $out $in");
        Rule::new("cc".into(), command)
            .set_depfile("$out.d".to_string())
            .set_description("Compiling file $in".to_string())
    }
}

impl Linker for GCC {
    fn add_library_dirs<I: Iterator<Item = impl AsRef<Path>>>(&mut self, dirs: I) {
        self.lib_dirs.extend(dirs.map(|p| p.as_ref().to_owned()))
    }

    fn set_shared(&mut self, is_shared: bool) {
        self.is_shared = is_shared;
    }

    fn set_position_independent(&mut self, pic: bool) {
        self.is_pic = pic;
    }

    fn as_lib_rule(&self) -> Rule {
        let mut command = self.path.display().to_string();
        for d in &self.lib_dirs {
            command.push_str(&format!(" -L{}", d.display()));
        }
        if self.is_pic && self.is_shared {
            command.push_str(" -fPIC");
        }
        if !self.is_shared {
            command.push_str(" -static");
        }
        command.push_str(" $flags -o $out $in");
        Rule::new("ldlib".to_owned(), command)
            .set_description("Linking $out".to_owned())
            .set_depfile("$out.d".to_owned())
    }

    fn as_exe_rule(&self) -> Rule {
        let mut command = self.path.display().to_string();
        for d in &self.lib_dirs {
            command.push_str(&format!(" -L{}", d.display()));
        }
        if !self.is_shared {
            command.push_str(" -static");
        }
        command.push_str(" $flags -o $out $in");
        Rule::new("ldexe".to_owned(), command)
            .set_description("Linking $out".to_owned())
            .set_depfile("$out.d".to_owned())
    }
}
