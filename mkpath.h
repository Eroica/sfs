#include <string.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>

int
mkpath(char *path, mode_t mode, mode_t dir_mode);
