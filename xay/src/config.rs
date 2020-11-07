use crate::clike::opts::BuildOptionsFile;
use serde::Deserialize;

#[derive(Clone, Debug, Deserialize)]
#[serde(tag = "language")]
pub enum Configuration {
    #[serde(rename="c")]
    C {
        #[serde(flatten)]
        opts: BuildOptionsFile,
    },
    #[serde(rename="c++")]
    CPP {
        #[serde(flatten)]
        opts: BuildOptionsFile,
    }
}
