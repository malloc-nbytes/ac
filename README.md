# ac
Active Compilation

This program will attempt to recompile/run the file(s) that are currently being worked on.

## Installation

Clone the repo:
- `git clone https://github.com/malloc-nbytes/ac.git`

Build the project:
- `./build.sh`

Install the project:
- `./install.sh`

It can now be ran anywhere using `ac`.

## Usage

ac [OPTION]... --build <build command> [WATCH FILES]...

where [OPTION] consists of

1. `--help`              display the help message
2. `--build <build cmd>` the build command (required)
3. `--run <filepath>`    run the program whenever it compiles
4. `--silent            only show exit code and status messages
5. `--verbose`           show extra debugging information

### Example

I have two files called `main.cpp` and `header.h`. I want to compile it using `g++ -Wextra -Wall -o main main.cpp`.

The command to utilize `ac` for this would be:

`ac --build "g++ -Wextra -Wall -o main main.cpp" main.c header.h`

Now whenever `main.cpp` or `header.h` is updated, it will attempt to run `g++ -Wextra -Wall -o main main.cpp`.

If I want to also run it with `./main` and see the output, the command would be:

`ac --run ./main --build "g++ -Wextra -Wall -o main main.cpp" main.c header.h`

[NOTE]: the "./" in `./main` is important here.
