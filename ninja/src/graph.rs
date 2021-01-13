use crate::Rule;
use daggy::{
    petgraph::{graph::NodeReferences, visit::IntoNodeReferences, Direction, Graph},
    Dag, NodeIndex, WouldCycle,
};
use std::{
    collections::{HashMap, HashSet},
    ops::Deref,
    path::{Path, PathBuf},
};
use thiserror::Error;
use daggy::petgraph::graph::DiGraph;

#[derive(Clone, Debug, Error)]
pub enum GraphError {
    #[error("Dependency graph would cycle")]
    WouldCycle(#[from] WouldCycle<()>),
    #[error("Path does not exist in the graph")]
    UnknownSource(String),
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub enum Node {
    Source(String),
    Generated {
        rule: Rule,
        outputs: HashSet<String>,
        vars: HashMap<String, String>,
    },
}

impl Node {
    pub fn files(&self) -> HashSet<&str> {
        match self {
            Self::Source(s) => std::iter::once::<&str>(s).collect(),
            Self::Generated { outputs, .. } => outputs.iter().map(|f| f.as_str()).collect(),
        }
    }
}

#[derive(Clone, Debug, Default)]
pub struct DependencyGraph {
    dag: Dag<Node, ()>,
    node_ref: HashMap<String, NodeIndex>,
}

impl Deref for DependencyGraph {
    type Target = Dag<Node, ()>;

    fn deref(&self) -> &Self::Target {
        &self.dag
    }
}

impl DependencyGraph {
    pub fn add_source<P: AsRef<str>>(&mut self, source: P) -> NodeIndex {
        self.add_node(Node::Source(source.as_ref().to_string()))
    }

    pub fn add_node(&mut self, node: Node) -> NodeIndex {
        let files = node
            .files()
            .into_iter()
            .map(|f| f.to_string())
            .collect::<Vec<_>>();
        let n = self.dag.add_node(node);
        for path in files {
            self.node_ref.insert(path, n);
        }
        return n;
    }

    pub fn add_dependency(&mut self, ix: NodeIndex, dep: NodeIndex) -> Result<(), GraphError> {
        self.dag.add_edge(ix, dep, ())?;
        Ok(())
    }

    pub fn add_dependency_name<P: AsRef<str>>(
        &mut self,
        ix: NodeIndex,
        dep: P,
    ) -> Result<(), GraphError> {
        let name = dep.as_ref();
        let dep = *self
            .node_ref
            .get(name)
            .ok_or(GraphError::UnknownSource(name.to_string()))?;
        self.add_dependency(ix, dep)
    }

    pub fn add_dependencies(
        &mut self,
        ix: NodeIndex,
        deps: impl IntoIterator<Item = NodeIndex>,
    ) -> Result<(), GraphError> {
        for dep in deps {
            self.dag.add_edge(ix, dep, ())?;
        }
        Ok(())
    }

    pub fn add_dependencies_names(
        &mut self,
        ix: NodeIndex,
        deps: impl IntoIterator<Item = String>,
    ) -> Result<(), GraphError> {
        for path in deps {
            self.add_dependency_name(ix, path)?;
        }
        Ok(())
    }

    pub fn indices(&self) -> impl Iterator<Item = NodeIndex> {
        self.dag.graph().node_indices()
    }

    pub fn nodes(&self) -> impl Iterator<Item = &Node> {
        self.dag.graph().node_references().map(|(_, f)| f)
    }

    pub fn nodes_dependencies(&self) -> impl Iterator<Item = Context<Node>> {
        let graph = self.dag.graph();
        self.indices().map(move |n| {
            let incoming = graph
                .neighbors_directed(n, Direction::Incoming)
                .map(|n| graph.node_weight(n).unwrap())
                .collect();
            let outgoing = graph
                .neighbors_directed(n, Direction::Outgoing)
                .map(|n| graph.node_weight(n).unwrap())
                .collect();
            Context {
                incoming,
                node: graph.node_weight(n).unwrap(),
                outgoing,
            }
        })
    }

    pub fn rules(&self) -> HashSet<&Rule> {
        self.nodes()
            .filter_map(|n| match n {
                Node::Source(_) => None,
                Node::Generated { rule, .. } => Some(rule),
            })
            .collect()
    }

    pub fn into_graph(self) -> DiGraph<Node, (), u32> {
        self.dag.into_graph()
    }
}

pub struct Context<'a, N> {
    pub incoming: Vec<&'a N>,
    pub node: &'a N,
    pub outgoing: Vec<&'a N>,
}
