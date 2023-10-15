#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>

#define ERR(msg, ...)                                   \
  do {                                                  \
    fprintf(stderr, "ERR: " msg "\n", ##__VA_ARGS__);   \
    exit(EXIT_FAILURE);                                 \
  } while (0)

#define INFO(msg, ...)                               \
  do {                                               \
    printf("[INFO] " msg "\n", ##__VA_ARGS__);       \
  } while (0)

#define BUILD_CMD_CAP 64
#define FILEPATH_CAP 64
#define SILENT 1 << 0

static uint8_t FLAGS = 0x00;
const float delay = .7f;
static size_t buildnum = 1;

void try_build(char *build_cmd)
{
  printf("--------------------\n");
  INFO("Build %ld", buildnum++);

  FILE *fp = NULL;

  // Redirect output of `build_cmd` to /dev/null if
  // SILENT flag is set.
  if ((FLAGS & SILENT) == 1) {
    char redirect[256];
    snprintf(redirect, sizeof(redirect), "%s > /dev/null 2>&1", build_cmd);
    fp = popen(redirect, "r");
  } else {
    fp = popen(build_cmd, "r");
  }

  if (!fp) {
    ERR("could not run the build command %s. Reason: %s\n",
        build_cmd, strerror(errno));
  }

  int exit_status = pclose(fp);
  INFO("Command exited with exit code: %d\n", exit_status);
  printf("--------------------\n");
}

void usage(const char *prog_name)
{
  fprintf(stderr, "Usage: %s [OPTION]... [FILEPATH] [BUILD]\n", prog_name);
  fprintf(stderr, "Active Compilation -- will check for updates in [FILEPATH].\n");
  fprintf(stderr, "                      When there is a change, it will attempt to run [BUILD].\n\n");
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

    if (strlen(arg) > 2 && arg[0] == '-' && arg[1] == '-') {
      if (strcmp(arg, "--help") == 0) {
        usage(prog_name);
      }
      else if (strcmp(arg, "--silent") == 0) {
        FLAGS |= SILENT;
        INFO("silent mode set");
      }
      else {
        ERR("ERR: unknown flag %s", arg);
      }
    }
    else {
      if (argc == 0) {
        ERR("ERR: missing build command");
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
      } else {
        ERR("could not open filepath: %s for reason: %s",
            filepath, strerror(errno));
      }

      if (last_modified > prev_time) {
        try_build(build_cmd);
        prev_time = file_info.st_mtime;
      }
    } else {
      ERR("ERR: stat failed for filepath: %s. Reason: %s\n",
          filepath, strerror(errno));
    }

    sleep(delay);
  }

  return 0;
}
