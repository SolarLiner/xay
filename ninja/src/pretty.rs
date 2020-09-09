use std::borrow::Cow;

use pretty::{DocAllocator, DocBuilder};
use termcolor::{Color, ColorSpec};

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum Annotation {
    Keyword,
    StrLiteral,
    Variable,
    Equals,
}

pub fn to_ansi_color(ann: Annotation) -> ColorSpec {
    use Annotation::*;
    use Color::*;
    match ann {
        Keyword => ColorSpec::new().set_fg(Some(Red)).set_bold(true).clone(),
        StrLiteral => ColorSpec::new().set_fg(Some(Yellow)).clone(),
        Variable => ColorSpec::default(),
        Equals => ColorSpec::default().set_fg(Some(White)).clone(),
    }
}

pub trait Pretty<Annotation: Clone> {
    fn pretty<'a, 's, D>(&'s self, allocator: &'a D) -> DocBuilder<'a, D, Annotation>
    where
        D: DocAllocator<'a, Annotation>,
        D::Doc: Clone;
}

pub fn variable<'a, S, D, V>(name: S, value: V, allocator: &'a D) -> DocBuilder<'a, D, Annotation>
where
    D: DocAllocator<'a, Annotation>,
    S: Into<Cow<'a, str>>,
    V: Into<DocBuilder<'a, D, Annotation>>,
{
    allocator
        .text(name)
        .append(allocator.space())
        .append(allocator.text("="))
        .append(allocator.space())
        .append(value.into())
}
