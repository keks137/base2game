#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
char *GetAppStoragePath() {
    const char *xdg_data_home = getenv("XDG_DATA_HOME");
    const char *home = getenv("HOME");
    char *path = malloc(256);

    if (xdg_data_home) {
        snprintf(path, 256, "%s/my_app", xdg_data_home);
    } else {
        snprintf(path, 256, "%s/.local/share/my_app", home);
    }

    // Create directory if it doesn't exist
    mkdir(path, 0700); // 0700 = user-only permissions
    return path;
}
