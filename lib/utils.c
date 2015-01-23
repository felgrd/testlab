#define _XOPEN_SOURCE 500
#include <stdio.h>			// snprintf
#include <stdlib.h>			// atoi
#include <sys/stat.h>		// mkdir
#include <unistd.h>			// execl, pipe
#include <fcntl.h> 			// open
#include <syslog.h>			// closelog
#include <limits.h>
#include "utils.h"


// zavreni vsech deskriptoru a syslogu
void close_all_fds(int keep)
{
	int	fd;
	int	max;

	max = getdtablesize();
	for (fd = keep + 1; fd < max; fd++) {
		close(fd);
	}

	closelog();
	open("/dev/null", O_RDWR | O_NOCTTY);
	dup(0);
	dup(0);
}

int project_delete(long long int release){
	char		command[PATH_MAX];
	int 		result;

	// Sestaveni adresare s docasnym souboru
	result = snprintf(command, sizeof(command), "%s/project/%lld", \
	DIRECTORY, release);
	if(result < 3){
		return 0;
	}

	// Smazani daneho adresare
	result = nftw(command, delete_item, 200, FTW_DEPTH);

	return result == 0;
}

int delete_item(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
	int			result;

	// Smazani prazdneho adresare nebo smazani souboru
	if(tflag == FTW_DP){
		result = rmdir(fpath);
	}else if(tflag == FTW_F){
		result = unlink(fpath);
	}

	return result == -1;
}
