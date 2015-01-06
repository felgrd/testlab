#include <ftw.h>

#ifndef __UTILS_H
#define __UTILS_H

#define DIRECTORY		"/var/testlab"
#define COMMAND_LENGTH	50

void close_all_fds(int keep);

int project_delete(long long int release);

int delete_item(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);

#endif
