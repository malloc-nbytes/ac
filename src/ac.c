#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define SAME(x, y) strcmp(x, y) == 0

#define BUILD_CMD_CAP 64
#define FILEPATH_CAP 64

void try_build(char *filepath, char *build_cmd)
{
  assert(0 && "unimplemented");
}

void usage(const char *prog_name)
{
  fprintf(stderr, "Usage: %s [OPTION]... [FILEPATH] [BUILD]\n", prog_name);
  fprintf(stderr, "Put description here.\n\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  --help        display this message\n");
  fprintf(stderr, "  --silent      only show exit code and status message\n");
  exit(EXIT_FAILURE);
}

const char *eat_arg(int *argc, char ***argv)
{
  (*argc)--;
  return *(*argv)++;
}

int main(int argc, char **argv)
{
  const char *prog_name = eat_arg(&argc, &argv);

  if (argc == 0) {
    usage(prog_name);
  }

  char build_cmd[BUILD_CMD_CAP] = {0};
  char filepath[FILEPATH_CAP] = {0};

  while (argc != 0) {
    const char *arg = eat_arg(&argc, &argv);

    if (SAME(arg, "--help") || SAME(arg, "-h")) {
      assert(0 && "unimplemented");
    } else if (SAME(arg, "--selent") || SAME(arg, "-s")) {
      assert(0 && "unimplemented");
    } else {
      if (argc == 0) {
        fprintf(stderr, "ERR: missing build command");
        exit(EXIT_FAILURE);
      }
      strncpy(filepath, arg, FILEPATH_CAP);
      strncpy(build_cmd, eat_arg(&argc, &argv), BUILD_CMD_CAP);
    }
  }

  while (1) {
    sleep(1);
    try_build(filepath, build_cmd);
  }

  return 0;
}
