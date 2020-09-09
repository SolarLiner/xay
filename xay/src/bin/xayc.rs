use ninja::Writer;
use std::{fmt::Display, path::PathBuf};
use structopt::StructOpt;
use xay::langc::{
    get_build_ast,
    opts::{BuildOptions, ProcessOptions},
};

#[derive(Copy, Clone, Debug, Eq, PartialEq, StructOpt)]
#[structopt()]
enum Command {
    #[structopt(about = "Generate the build directory for ninja to build the project")]
    Generate,
    #[structopt(about = "Generate the build files and build the project")]
    Build,
    #[structopt(
        about = "Generate the build files, build the project and run the executable (implies executable mode)"
    )]
    Run,
}

impl Default for Command {
    fn default() -> Self {
        Command::Generate
    }
}

#[derive(Debug, StructOpt)]
#[structopt(
    name = "xayc",
    about = "An automatic build system for C projects",
    author = "Nathan Graule <solarliner@gmail.com>"
)]
struct CmdArgs {
    #[structopt(subcommand)]
    cmd: Option<Command>,
    #[structopt(default_value = ".", parse(from_os_str))]
    path: PathBuf,
    #[structopt(default_value = "build")]
    dest: PathBuf,
    #[structopt(short, long, default_value = "xayc.yml")]
    config: String,
}

fn main() -> Result<(), String> {
    let opt: CmdArgs = CmdArgs::from_args();

    let cwd = std::env::current_dir().map_err(display_prefix("cwd"))?;
    let base_dir = cwd.join(opt.path);
    let src_dir = base_dir.join("src");
    let dest_dir = base_dir.join(opt.dest);
    let config_path = base_dir.join(opt.config);
    let ninja_path = dest_dir.join("build.ninja");
    let name = base_dir.file_name().unwrap().to_string_lossy().into();

    if !src_dir.exists() {
        return Err(format!(
            "Folder does not have a src folder: {}",
            base_dir.display()
        ));
    }

    std::fs::create_dir_all(&dest_dir).map_err(display_prefix("Create build dir"))?;
    let build_opts = if config_path.exists() {
        let c = serde_yaml::from_reader(
            std::fs::File::open(config_path).map_err(display_prefix("Reading configuration"))?,
        )
        .map_err(display)?;
        BuildOptions::from_file(c, name)
    } else {
        BuildOptions::new(name)
    };
    let name = build_opts.name.clone();
    let procopts = ProcessOptions {
        build_opts,
        src_dir,
        dest_dir: dest_dir.clone(),
    };

    let ast = get_build_ast(procopts).map_err(display_prefix("Generate build"))?;

    let mut writer = Writer::default();
    writer.add_ast(ast);
    let mut ninja = std::fs::File::create(&ninja_path)
        .map_err(display_prefix(display(ninja_path.display())))?;
    writer
        .write_file(&mut ninja, 80)
        .map_err(display_prefix(display(ninja_path.display())))?;

    match opt.cmd {
        None | Some(Command::Generate) => {
            println!("Wrote output to {}", dest_dir.display());
        }
        Some(Command::Build) => {
            run("ninja", vec!["-C".to_owned(), display(dest_dir.display())])
                .map_err(display_prefix("ninja"))?;
        }
        Some(Command::Run) => {
            run("ninja", vec!["-C".to_owned(), display(dest_dir.display())])
                .map_err(display_prefix("ninja"))?;
            let tgt_file = dest_dir.join(name);
            println!();
            run(tgt_file.clone().clone().to_string_lossy(), vec![])
                .map_err(display_prefix(display(tgt_file.display())))?;
        }
    }
    Ok(())
}

fn run<S: Into<String>, I: IntoIterator<Item = String>>(cmd: S, args: I) -> std::io::Result<()> {
    let mut child = std::process::Command::new(cmd.into()).args(args).spawn()?;
    child.wait()?;
    Ok(())
}
/*
fn do_pipe<R: Read, W: Write>(child: &mut R, parent: &mut W) -> std::io::Result<bool> {
    let mut buf = vec![];
    match child.read(&mut buf)? {
        0 => Ok(true),
        len => {
            parent.write_all(&mut buf[..len])?;
            Ok(false)
        }
    }
} */

fn display<T: Display>(value: T) -> String {
    format!("{}", value)
}

fn display_prefix<S: Into<String>, T: Display>(prefix: S) -> impl (FnOnce(T) -> String) {
    |value| format!("{}: {}", prefix.into(), value)
}
