#ifndef INCLUDE_PLATFORM_STORAGEPATH_H_
#define INCLUDE_PLATFORM_STORAGEPATH_H_

#ifdef __linux__

#include "linux/getAppStoragePath.h"

#endif // linux
#ifdef __ANDROID__
#include "raymob.h"
#endif // android

#endif // INCLUDE_PLATFORM_STORAGEPATH_H_
