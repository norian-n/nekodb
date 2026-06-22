#include <fcntl.h>
#include <unistd.h>

#include "egFileLock.h"

int lockFile(const std::string& fileNameStr, bool writeLock) {
    int lockFileDescriptor = open(fileNameStr.c_str(), O_RDWR | O_CREAT, 0666); // file descriptor
    if (lockFileDescriptor == -1) {
        // EG_LOG_STUB << "cant open lock file: " << fileNameStr << FN;
        return -1;
    }

    struct flock fileLock;        // file lock struct
    memset(&fileLock, 0, sizeof(fileLock));
    fileLock.l_type = writeLock ? F_WRLCK : F_RDLCK;    // write lock, fl.l_type == F_RDLCK for read lock, lock.l_type = F_UNLCK to unlock

    if ( fcntl(lockFileDescriptor, F_SETLK, &fileLock) == -1 ) { // F_SETLKW - wait for lock
        // EG_LOG_STUB << "cant SET fcntl: " << fileNameStr << FN;
        close(lockFileDescriptor);
        return -1;
    }
    return lockFileDescriptor; // locked by this process till close(fd)
}

void unlockFile(int fileDescriptor) {
    close(fileDescriptor);
}