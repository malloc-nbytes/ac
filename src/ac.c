#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>

#define BUILD_CMD_CAP 64
#define FILEPATH_CAP 64
#define SILENT  1 << 0
#define VERBOSE 1 << 1
#define RUN     1 << 2

#define ERR(msg, ...)                                   \
  do {                                                  \
    fprintf(stderr, "ERR: " msg "\n", ##__VA_ARGS__);   \
    exit(EXIT_FAILURE);                                 \
  } while (0)

#define INFO(msg, ...)                               \
  do {                                               \
    printf("[INFO] " msg "\n", ##__VA_ARGS__);       \
  } while (0)

#define LOG(msg, ...)                                \
  do {                                               \
    if ((FLAGS & VERBOSE) != 0) {                    \
      printf("[LOG] " msg "\n", ##__VA_ARGS__);      \
    }                                                \
  } while (0)

static uint16_t FLAGS = 0x00;
const float delay = .7f;
static size_t buildnum = 1;

void try_build(char *build_cmd, char *run_filepath)
{
  printf("--------------------\n");
  INFO("Build %ld", buildnum++);

  FILE *fp = NULL;

  // Redirect output of `build_cmd` to /dev/null if
  // SILENT flag is set.
  if ((FLAGS & SILENT) == 1) {
    LOG("Redirecting output > /dev/null");
    char redirect[256];
    snprintf(redirect, sizeof(redirect), "%s > /dev/null 2>&1", build_cmd);
    fp = popen(redirect, "r");
  } else {
    fp = popen(build_cmd, "r");
  }

  if (!fp) {
    ERR("could not run the build command %s because of: %s\n",
        build_cmd, strerror(errno));
  }

  // `pclose()` returns the exit status.
  int exit_status = pclose(fp);
  INFO("command exited with exit code: %d", exit_status);

  // TODO: make this check the file for
  // last modification.
  if ((FLAGS & RUN) != 0 && exit_status == 0) {
    fp = popen(run_filepath, "r");
    if (!fp) {
      fprintf(stderr, "could not run: %s because of: %s",
          run_filepath, strerror(errno));
    }

    // Print the output of the program.
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    exit_status = pclose(fp);
    INFO("%s exited with code: %d", run_filepath, exit_status);
  }

  printf("--------------------\n");
}

void usage(const char *prog_name)
{
  fprintf(stderr, "Usage: %s [OPTION]... --build <build command> [WATCH FILES]...\n", prog_name);
  fprintf(stderr, "Active Compilation -- will check for updates in [WATCH FILES].\n");
  fprintf(stderr, "                      When there is a change, it will attempt to run <build command>.\n\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  --help              display this message\n");
  fprintf(stderr, "  --build <build cmd> the build command (required)\n");
  fprintf(stderr, "  --run <filepath>    run the program whenever it compiles\n");
  fprintf(stderr, "  --silent            only show exit code and status message\n");
  fprintf(stderr, "  --verbose           show extra information\n");
  exit(EXIT_FAILURE);
}

// Easy way to go through the `argv`
const char *expect(int *argc, char ***argv, const char *expected)
{
  if (argc == 0) {
    ERR("expected %s, but `argc` = 0", expected);
  }
  (*argc)--;
  return *(*argv)++;
}

void build_loop(char **watch_files, size_t watch_files_len, char *build_cmd, char *run_filepath)
{
  struct stat file_info;
  time_t prev_time = 0;
  int idx = 0;

  while (1) {
    const char *filepath = watch_files[idx];

    // Make sure we have actually read the file.
    if (stat(filepath, &file_info) == 0) {
      time_t last_modified = file_info.st_mtime;
      FILE *fp = fopen(filepath, "r");

      if (fp) {
        fscanf(fp, "%ld", &prev_time);
        fclose(fp);
      }
      else {
        ERR("could not open filepath: %s for reason: %s",
            filepath, strerror(errno));
      }

      // The file has been updated. Try to build.
      if (last_modified > prev_time) {
        LOG("`last_modified: %ld :: `prev_time`: %ld", last_modified, prev_time);
        try_build(build_cmd, run_filepath);
        prev_time = file_info.st_mtime;
      }
    }
    else {
      ERR("ERR: stat failed for filepath: %s. Reason: %s\n",
          filepath, strerror(errno));
    }

    idx = (idx+1)%watch_files_len;

    sleep(delay);
  }
}

int main(int argc, char **argv)
{
  const char *prog_name = expect(&argc, &argv, "program name");

  if (argc == 0) {
    usage(prog_name);
  }

  char build_cmd[BUILD_CMD_CAP] = {0};
  char run_filepath[FILEPATH_CAP] = {0};
  char *watch_files[FILEPATH_CAP] = {0};
  size_t watch_files_len = 0;

  while (argc != 0) {
    const char *arg = expect(&argc, &argv, "flag, filepath, or build command");

    if (strlen(arg) > 2 && arg[0] == '-' && arg[1] == '-') {
      if (strcmp(arg, "--help") == 0) {
        usage(prog_name);
      }
      else if (strcmp(arg, "--silent") == 0) {
        FLAGS |= SILENT;
        LOG("silent mode set");
      }
      else if (strcmp(arg, "--verbose") == 0) {
        FLAGS |= VERBOSE;
        LOG("verbose mode set");
      }
      else if (strcmp(arg, "--run") == 0) {
        FLAGS |= RUN;
        LOG("run mode set");
        const char *try_run_filepath = expect(&argc, &argv, "run filepath");
        strncpy(run_filepath, try_run_filepath, FILEPATH_CAP);
        LOG("run filepath: %s", run_filepath);
      }
      else if (strcmp(arg, "--build") == 0) {
        const char *try_build_cmd = expect(&argc, &argv, "build command");
        strncpy(build_cmd, try_build_cmd, BUILD_CMD_CAP);
        LOG("build command: %s", build_cmd);
      }
      else {
        ERR("ERR: unknown flag %s", arg);
      }
      LOG("flag: %s", arg);
    }
    else {
      watch_files[watch_files_len++] = strdup(arg);
      LOG("added watch file: %s", watch_files[watch_files_len - 1]);
    }
  }

  build_loop(watch_files, watch_files_len, build_cmd, run_filepath);

  return 0;
}
