#include <fcntl.h>
#include <unistd.h>

#include "egFileLock.h"

int writeLockFile(const std::string& fileNameStr) {
    const char* filename = fileNameStr.c_str();
    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        // EG_LOG_STUB << "cant open lock file: " << fileNameStr << FN;
        return -1;
    }

    struct flock fl; // Define the lock we want to test for
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;    // Test if we can get an exclusive (write), lock lock.l_type = F_UNLCK; to unlock
    // fl.l_whence = SEEK_SET; // From start of file
    // fl.l_start = 0;         // Offset 0
    // fl.l_len = 0;           // 0 means until end of file (EOF)

    if (fcntl(fd, F_SETLK, &fl) == -1) { // F_SETLKW - wait for lock
        // EG_LOG_STUB << "cant SET fcntl: " << fileNameStr << FN;
        close(fd);
        return -1;
    }

    if (fcntl(fd, F_GETLK, &fl) == -1) { // Use F_GETLK to check for existing locks
        // EG_LOG_STUB << "cant GET fcntl: " << fileNameStr << FN;
        close(fd);
        return -1;
    }

    if (fl.l_type != F_UNLCK) { // locked
    //    std::cout << "lockFile(): no conflicting locks found. The file is available: " << fileNameStr << std::endl;
    // } else {
    //    std::cout << "lockFile(): file is locked by process: " << fl.l_pid << " " << fileNameStr; //  << std::endl;
    //    std::cout << " Lock type: " << (fl.l_type == F_RDLCK ? "Read" : "Write") << std::endl;
        close(fd);
        return -1;        
    }

    return fd; // locked by this process till close(fd)
}

void unlockFile(int fileDescriptor) {
    close(fileDescriptor);
}