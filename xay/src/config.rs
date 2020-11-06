use crate::langc::opts::BuildOptionsFile;
use serde::Deserialize;
use std::collections::HashMap;

#[derive(Clone, Debug, Deserialize)]
#[serde(tag = "language")]
pub enum Configuration {
    #[serde(rename="c")]
    C {
        #[serde(flatten)]
        opts: BuildOptionsFile,
    },
}
