
#include <fcntl.h>
#include <sys/stat.h>
#include "fileutil.h"

int xwrite(char *filename, long offset, char *arraybyte, int length)
{
    int nHandle ;
    int nRet ;
    int nMode ;
    int nPermission;

    if( length <= 0 )
        return 1 ;
    nMode = O_RDWR|O_SYNC|O_CREAT ;
    nPermission = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    nHandle = open( filename, nMode, nPermission ) ;
    lseek( nHandle, offset, SEEK_SET) ;
    nRet = write ( nHandle, arraybyte, length ) ;
    close( nHandle );
    if (nRet == length) {
        return 1 ;
    } else {
        return 0 ;
    }
}