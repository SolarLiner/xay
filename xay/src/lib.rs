use std::ops::{Deref, DerefMut};
use std::path::{Path, PathBuf};

pub mod config;
pub mod langc;

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Context<T> {
    pub name: String,
    pub cwd: PathBuf,
    pub src_dir: PathBuf,
    pub dest_dir: PathBuf,
    pub inner: T,
}

impl<T> Deref for Context<T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl<T> DerefMut for Context<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.inner
    }
}

impl<T> Context<T> {
    pub fn map_inner<U, F: FnOnce(T) -> U>(self, f: F) -> Context<U> {
        let Context {
            name,
            cwd,
            dest_dir,
            src_dir,
            inner,
        } = self;
        Context {
            name,
            cwd,
            dest_dir,
            src_dir,
            inner: f(inner),
        }
    }
}
