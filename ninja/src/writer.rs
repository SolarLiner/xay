use pretty::DocAllocator;
use std::io::stdout;
use std::{collections::HashSet, fs::File};

use pretty::{BuildDoc, DocBuilder, RcAllocator, RcDoc};

use crate::{
    ast::NinjaAst,
    blocks::Build,
    blocks::Rule,
    pretty::{Annotation, Pretty},
};

pub struct Writer<'a> {
    written_rules: HashSet<String>,
    content_builder: DocBuilder<'a, RcAllocator, Annotation>,
}

impl<'a> Default for Writer<'a> {
    fn default() -> Self {
        Self {
            written_rules: HashSet::new(),
            content_builder: DocBuilder(&RcAllocator, BuildDoc::from(RcDoc::nil())),
        }
    }
}

impl<'a> Writer<'a> {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn add_rule(&mut self, rule: &Rule) {
        if self.written_rules.contains(&rule.name) {
            return;
        }
        self.written_rules.insert(rule.name.clone());
        self.content_builder = self
            .content_builder
            .clone()
            .append(rule.pretty(&RcAllocator))
            .append(RcAllocator.hardline());
    }

    pub fn add_build(&mut self, build: &Build) {
        assert!(self.written_rules.contains(&build.rule));

        self.content_builder = self
            .content_builder
            .clone()
            .append(build.pretty(&RcAllocator))
            .append(RcAllocator.hardline());
    }

    pub fn add_default(&mut self, default: String) {
        let alloc = &RcAllocator;
        self.content_builder = self
            .content_builder
            .clone()
            .append("default")
            .annotate(Annotation::Keyword)
            .append(alloc.space())
            .append(default)
            .append(alloc.hardline());
    }

    pub fn add_ast<A: Into<NinjaAst>>(&mut self, ast: A) -> Option<String> {
        use NinjaAst::*;
        match ast.into() {
            Source(f) => return Some(f),
            Generated {
                name,
                rule,
                deps,
                vars,
            } => {
                let inputs = deps
                    .into_iter()
                    .map(|a| self.add_ast(a))
                    .filter_map(id)
                    .collect();
                self.add_rule(&rule);
                self.add_build(&{
                    let mut b = Build::new(rule.get_ref(), vec![name.clone()], inputs);
                    b.vars = vars;
                    b
                });
                Some(name)
            }
            Default(inner) => {
                let name = self.add_ast(*inner).unwrap();
                self.add_default(name);
                None
            }
        }
    }

    pub fn write_tty(self, width: usize) -> std::io::Result<()> {
        let stdout = stdout();
        let mut stdout = stdout.lock();
        self.content_builder.into_doc().render(width, &mut stdout)
    }

    pub fn write_file(self, file: &mut File, width: usize) -> std::io::Result<()> {
        self.content_builder.into_doc().render(width, file)
    }
}

fn id<T>(x: T) -> T {
    x
}

#[cfg(test)]
mod tests {
    use std::collections::HashMap;

    use crate::{NinjaAst::*, Rule, Writer};

    macro_rules! set {
        ($($value:expr),*) => {
            {
                let mut set = ::std::collections::HashSet::new();
                $(
                    set.insert($value);
                )*
                set
            }
        }
    }

    #[test]
    fn writer_adds_rules() {
        let ccrule = Rule::new("cc".to_owned(), "gcc -c $in -o $out".to_owned());
        let ldrule = Rule::new("ld".to_owned(), "gcc $in -o $out".to_owned());
        let ast = Generated {
            name: "gen".to_owned(),
            rule: ldrule.clone(),
            deps: vec![
                Generated {
                    name: "gen.o".to_owned(),
                    rule: ccrule.clone(),
                    deps: vec![Source("gen.c".to_owned())],
                    vars: HashMap::new(),
                },
                Generated {
                    name: "other.o".to_owned(),
                    rule: ccrule.clone(),
                    deps: vec![Source("other.c".to_owned())],
                    vars: HashMap::new(),
                },
            ],
            vars: HashMap::new(),
        };

        let mut writer = Writer::new();
        writer.add_ast(ast);
        assert_eq!(set![ccrule.name, ldrule.name], writer.written_rules);
    }
}
