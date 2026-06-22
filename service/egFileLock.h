#pragma once

#include "../metainfo/egCoreTypes.h"

int  lockFile(const std::string& fileNameStr, bool writeLock = true);
void unlockFile(int fileDescriptor);
