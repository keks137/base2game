#ifndef INCLUDE_PLATFORM_STORAGEPATH_H_
#define INCLUDE_PLATFORM_STORAGEPATH_H_


#ifndef __ANDROID__

#ifdef __linux__

#include "linux/getAppStoragePath.h"

#endif // linux
#endif // not-android

#ifdef __ANDROID__
#include "raymob.h"
#endif // android

#endif // INCLUDE_PLATFORM_STORAGEPATH_H_
