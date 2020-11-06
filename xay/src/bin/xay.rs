use ninja::{NinjaAst, Writer};
use std::fs::File;
use std::{fmt::Display, path::PathBuf};
use structopt::StructOpt;
use xay::{config::Configuration, langc, Context};

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
    name = "xay",
    about = "An automatic build system for 95% of projects",
    author = "Nathan Graule <solarliner@gmail.com>"
)]
struct CmdArgs {
    #[structopt(subcommand)]
    cmd: Option<Command>,
    #[structopt(default_value = ".", parse(from_os_str))]
    path: PathBuf,
    #[structopt(default_value = "build")]
    dest: PathBuf,
    #[structopt(short, long, default_value = "xay.yml")]
    config: String,
}

fn main() -> anyhow::Result<()> {
    let opt: CmdArgs = CmdArgs::from_args();

    let cwd = std::env::current_dir()?;
    let base_dir = cwd.join(opt.path);
    let src_dir = base_dir.join("src");
    let dest_dir = base_dir.join(opt.dest);
    let config_path = base_dir.join(opt.config);
    let ninja_path = dest_dir.join("build.ninja");
    let name = base_dir.file_name().unwrap().to_string_lossy().into();

    let ctx = Context {
        name,
        cwd,
        src_dir,
        dest_dir,
        inner: (),
    };

    if !ctx.src_dir.exists() {
        return Err(anyhow::anyhow!(
            "Folder does not have a src folder: {}",
            base_dir.display()
        ));
    }

    std::fs::create_dir_all(&ctx.dest_dir).map_err(display_prefix("Create build dir"))?;

    let ast = match serde_yaml::from_reader::<_, Configuration>(
        File::open(&config_path).map_err(display_prefix("Config file"))?,
    ) {
        Ok(Configuration::C { opts }) => langc::handle_c_project(ctx.clone().map_inner(|_| opts)),
        Err(err) => Err(err.into()),
    }?;
    let mut writer = Writer::default();
    writer.add_ast(ast);
    let mut ninja = std::fs::File::create(&ninja_path)
        .map_err(display_prefix(display(ninja_path.display())))?;
    writer
        .write_file(&mut ninja, 80)
        .map_err(display_prefix(display(ninja_path.display())))?;

    match opt.cmd {
        None | Some(Command::Generate) => {
            println!("Wrote output to {}", ctx.dest_dir.display());
        }
        Some(Command::Build) => {
            run(
                "ninja",
                vec!["-C".to_owned(), display(ctx.dest_dir.display())],
            )
            .map_err(display_prefix("ninja"))?;
        }
        Some(Command::Run) => {
            run(
                "ninja",
                vec!["-C".to_owned(), display(ctx.dest_dir.display())],
            )
            .map_err(display_prefix("ninja"))?;
            let tgt_file = ctx.dest_dir.join(ctx.name);
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

fn display_prefix<S: Into<String>, T: Display>(prefix: S) -> impl (FnOnce(T) -> anyhow::Error) {
    |value| anyhow::anyhow!("{}: {}", prefix.into(), value)
}