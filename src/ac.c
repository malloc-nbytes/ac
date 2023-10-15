#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#define SAME(x, y) strcmp(x, y) == 0

#define BUILD_CMD_CAP 64
#define FILEPATH_CAP 64

void try_build(char *build_cmd)
{
  FILE *fp = popen(build_cmd, "w");
  if (!fp) {
    fprintf(stderr, "ERR: could not run the build command %s. Reason: %s\n", build_cmd, strerror(errno));
    exit(EXIT_FAILURE);
  }

  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    printf("%s\n", buffer);
  }

  pclose(fp);
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

  struct stat file_info;
  time_t prev_time = 0;
  while (1) {
    if (stat(filepath, &file_info) == 0) {
      time_t last_modified = file_info.st_mtime;
      FILE *fp = fopen(filepath, "r");

      if (fp) {
        fscanf(fp, "%ld", &prev_time);
        fclose(fp);
      }

      if (last_modified > prev_time) {
        printf("[INFO] Detected modification. Building...\n");
        try_build(build_cmd);
        prev_time = file_info.st_mtime;
      }
    } else {
      fprintf(stderr, "ERR: stat failed for filepath: %s. Reason: %s\n", filepath, strerror(errno));
      exit(EXIT_FAILURE);
    }

    sleep(1);
  }

  return 0;
}
