use std::collections::HashMap;

use crate::blocks::Rule;

#[derive(Clone, Debug)]
pub enum NinjaAst {
    Source(String),
    Generated {
        name: String,
        rule: Rule,
        deps: Vec<NinjaAst>,
        vars: HashMap<String, String>,
    },
    Default(Box<Self>),
}
