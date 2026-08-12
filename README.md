# pm

`pm` is a small project manager for C projects that use Makefiles.

## Global project registry

The first command that needs configuration creates:

```text
~/.config/pm/
└── projects
```

`projects` contains one registered project per line:

```text
project-name=/absolute/path/to/project
```

Commands can select a registered project with `-p <project-name>`. The value
is a project name, not a filesystem path. Without `-p`, commands resolve the
registered project that contains the current directory.

Each project may contain a `.pm` file. This file does not identify a pm
project; it only stores local settings:

```text
target=a
```

If `.pm` or `target` is missing, `run` uses `a` and can fall back to searching
for an executable in the project root.

## Commands

### Create a project

```sh
pm init <name> [-v] [-c] [-m] [-M] [-d]
```

- `-v`: verbose logs.
- `-c`: do not create `.clangd`.
- `-m`: do not create `src/main.c`.
- `-M`: do not create `Makefile`.
- `-d`: do not create any default file.

The project is created below the current directory and registered globally.

### Add or remove modules

```sh
pm add <module> [-p <project>] [-v] [-f] [-c] [-h]
pm rm  <module> [-p <project>] [-v] [-f] [-c] [-h]
```

- `-p`: select a registered project by name.
- `-v`: verbose logs.
- `-f`: force the operation; for `add`, use the current directory without a
  registered project; for `rm`, ignore missing files.
- `-c`: do not create/remove the `.c` file.
- `-h`: do not create/remove the `.h` file.

### Build, run and clean

```sh
pm build [-v] [-p <project>]
pm run [-v] [-b] [-p <project>] [target] [-a arguments...]
pm clean [-v] [-p <project>]
```

- `build` runs `make all`.
- `run -b` builds before running.
- `run -a` sends all following values to the executable. Put `-p` before `-a`.
- `clean` runs `make clean`.

### Manage configuration

```sh
pm config
pm config -p <project>
pm config -p <project> -t <target>
pm config -a <name> <path>
pm config -r <project>
pm config -s
```

- No flags: list every `name=path` registration.
- `-p`: show a project's absolute path and target.
- `-t`: update the selected project's target.
- `-a`: register an existing directory under a name.
- `-r`: remove the global registration without deleting the project.
- `-s`: install the Bash integration needed by `pm goto` in `~/.bashrc`.

### List projects

```sh
pm list
pm list -t
```

- Without flags: show the name and full path of every registered project.
- `-t`: also show the target configured in each project's `.pm` file.
- Registrations whose directories no longer exist are marked as `[missing]`.

### Go to a project

Programs cannot directly change the directory of the shell that started them.
`pm goto` therefore validates the project and prints its absolute path:

```sh
pm goto <project>
```

To install the wrapper that makes `pm goto` change the current terminal
directory, run:

```sh
pm config -s
```

The command adds this block to `~/.bashrc` only when it is not already present:

```sh
pm() {
    if [ "$1" = "goto" ]; then
        local directory
        directory="$(command pm goto "${@:2}")" || return
        cd "$directory" || return
    else
        command pm "$@"
    fi
}
```

Reload the shell configuration:

```sh
source ~/.bashrc
```

Then use:

```sh
pm goto <project>
```

`goto` accepts no flags. It returns a warning when the project argument is
missing, when the registration does not exist, or when its directory was
deleted.

### Help and version

```sh
pm help [command...]
pm --version
```
