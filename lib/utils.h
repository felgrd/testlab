#include <ftw.h>

#ifndef __UTILS_H
#define __UTILS_H

#define DIRECTORY		"/var/testlab"
#define COMMAND_LENGTH	50

/// In this state checkout source code from repository
#define STATE_CHECKOUT     1
/// In this state compile source code
#define STATE_COMPILE      2
/// In this state test invidiual function of the router
#define STATE_TESTROUTER   3
/// In this state delete binnary of firmware
#define STATE_CLEAN        4

void close_all_fds(int keep);

int project_delete(long long int release);

int delete_item(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);

#endif
