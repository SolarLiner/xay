use std::collections::HashMap;

use crate::pretty::variable;

use super::pretty::{
    Annotation::{self, *},
    Pretty,
};
use pretty::{DocAllocator, DocBuilder};
use std::hash::{Hash, Hasher};

#[derive(Clone, Debug)]
pub struct Rule {
    pub(crate) name: String,
    pub(crate) command: String,
    pub(crate) description: Option<String>,
    pub(crate) depfile: Option<String>,
}

impl PartialEq<Rule> for Rule {
    fn eq(&self, other: &Rule) -> bool {
        self.name == other.name
    }
}

impl PartialEq<String> for Rule {
    fn eq(&self, other: &String) -> bool {
        &self.name == other
    }
}

impl Eq for Rule {}

impl Hash for Rule {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.name.hash(state)
    }
}

impl Pretty<Annotation> for Rule {
    fn pretty<'a, D>(&self, allocator: &'a D) -> DocBuilder<'a, D, Annotation>
    where
        D: DocAllocator<'a, Annotation>,
        D::Doc: Clone,
    {
        let doc = allocator
            .text("rule")
            .annotate(Keyword)
            .append(allocator.space())
            .append(allocator.text(self.name.clone()))
            .append(allocator.hardline())
            .append(variable("command", allocator.text(self.command.clone()), allocator).indent(2))
            .append(allocator.hardline());
        let doc = if let Some(d) = &self.description {
            doc.append(variable("description", allocator.text(d.clone()), allocator).indent(2))
                .append(allocator.hardline())
        } else {
            doc
        };
        if let Some(d) = &self.depfile {
            doc.append(
                variable("depfile", allocator.text(d.clone()), allocator)
                    .indent(2)
                    .append(allocator.hardline()),
            )
        } else {
            doc
        }
    }
}

impl Rule {
    pub fn new(name: String, command: String) -> Self {
        Self {
            name,
            command,
            description: None,
            depfile: None,
        }
    }

    pub fn set_description<S: Into<Option<String>>>(mut self, description: S) -> Self {
        self.description = description.into();
        self
    }

    pub fn set_depfile<S: Into<Option<String>>>(mut self, depfile: S) -> Self {
        self.depfile = depfile.into();
        self
    }

    pub fn get_ref(&self) -> RuleRef {
        RuleRef(self.name.clone())
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct RuleRef(pub String);

impl PartialEq<String> for RuleRef {
    fn eq(&self, other: &String) -> bool {
        &self.0 == other
    }
}

impl From<Rule> for RuleRef {
    fn from(rule: Rule) -> Self {
        Self(rule.name)
    }
}

impl Into<String> for RuleRef {
    fn into(self) -> String {
        self.0
    }
}

#[derive(Clone, Debug)]
pub struct Build {
    pub rule: String,
    pub outputs: Vec<String>,
    pub inputs: Vec<String>,
    pub vars: HashMap<String, String>,
}

impl PartialEq<Self> for Build {
    fn eq(&self, other: &Self) -> bool {
        self.rule == other.rule && self.outputs == other.outputs
    }
}
impl Eq for Build {}

impl Pretty<Annotation> for Build {
    fn pretty<'a, D>(&self, allocator: &'a D) -> DocBuilder<'a, D, Annotation>
    where
        D: DocAllocator<'a, Annotation>,
        D::Doc: Clone,
    {
        let inputs = self
            .inputs
            .iter()
            .map(|i| allocator.text(i.clone()) /* .double_quotes() */);
        let outputs = self
            .outputs
            .iter()
            .map(|o| allocator.text(o.clone()) /* .double_quotes() */);
        let vars = self
            .vars
            .iter()
            .map(|(k, v)| variable(k.clone(), allocator.text(v.clone()), allocator));
        allocator
            .text("build")
            .annotate(Annotation::Keyword)
            .append(allocator.space())
            .append({
                let base = allocator
                    .intersperse(outputs, allocator.space())
                    .append(":")
                    .append(allocator.space())
                    .append(self.rule.clone())
                    .append(allocator.space())
                    .append(allocator.intersperse(inputs, allocator.space()))
                    .append(allocator.hardline());
                if vars.len() == 0 {
                    base
                } else {
                    base.append(allocator.intersperse(vars, allocator.hardline()).indent(2))
                        .append(allocator.hardline())
                }
            })
    }
}

impl Build {
    pub fn new(RuleRef(rule): RuleRef, outputs: Vec<String>, inputs: Vec<String>) -> Self {
        Self {
            rule,
            outputs,
            inputs,
            vars: HashMap::new(),
        }
    }

    pub fn get_ref(&self) -> BuildRef {
        BuildRef(self.outputs[0].clone())
    }
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct BuildRef(String);

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct DefaultBlock(pub BuildRef);

impl Pretty<Annotation> for DefaultBlock {
    fn pretty<'a, 's, D>(&'s self, allocator: &'a D) -> DocBuilder<'a, D, Annotation>
    where
        D: DocAllocator<'a, Annotation>,
        D::Doc: Clone,
    {
        let DefaultBlock(BuildRef(build)) = self;
        allocator
            .text("default")
            .annotate(Annotation::Keyword)
            .append(allocator.space())
            .append(build.clone())
    }
}

#[cfg(test)]
mod tests {
    use pretty::RcAllocator;

    use crate::{Build, Pretty, Rule};

    #[test]
    fn rule_eq() {
        let r1 = Rule::new("a".to_owned(), "cmd".to_owned());
        let r2 = Rule::new("b".to_owned(), "cmd".to_owned());
        let r3 = Rule::new("a".to_owned(), "othercmd".to_owned());

        assert_ne!(r1, r2);
        assert_eq!(r1, r3);
    }

    #[test]
    fn pretty_rule() {
        let r1 = Rule::new("a".to_owned(), "cmd".to_owned());
        let r2 = r1.clone().set_description("description".to_owned());
        let r3 = r1.clone().set_depfile("$out.d".to_owned());

        let mut renderer = vec![];
        r1.pretty(&RcAllocator)
            .into_doc()
            .render(80, &mut renderer)
            .unwrap();
        assert_eq!(b"rule a\n  command = cmd\n"[..], renderer[..]);

        renderer.clear();

        r2.pretty(&RcAllocator)
            .into_doc()
            .render(80, &mut renderer)
            .unwrap();
        assert_eq!(
            b"rule a\n  command = cmd\n  description = description\n"[..],
            renderer[..]
        );

        renderer.clear();

        r3.pretty(&RcAllocator)
            .into_doc()
            .render(80, &mut renderer)
            .unwrap();
        assert_eq!(
            b"rule a\n  command = cmd\n  depfile = $out.d\n"[..],
            renderer[..]
        );
    }

    #[test]
    fn build_pretty() {
        let rule = Rule::new("a".to_owned(), "cmd".to_owned());
        let b1 = Build::new(
            rule.get_ref(),
            vec!["a.o".to_owned()],
            vec!["a.c".to_owned()],
        );
        let mut b2 = b1.clone();
        b2.vars.insert("flags".to_owned(), "-lm".to_owned());

        let mut renderer = vec![];
        b1.pretty(&RcAllocator)
            .into_doc()
            .render(80, &mut renderer)
            .unwrap();
        assert_eq!(b"build a.o: a a.c\n"[..], renderer[..]);

        renderer.clear();

        b2.pretty(&RcAllocator)
            .into_doc()
            .render(80, &mut renderer)
            .unwrap();
        assert_eq!(b"build a.o: a a.c\n  flags = -lm\n"[..], renderer[..]);
    }
}
