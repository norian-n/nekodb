#pragma once

#include "../metainfo/egCoreTypes.h"

int writeLockFile(const std::string& fileNameStr);
void unlockFile(int fileDescriptor);
