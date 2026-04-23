#ifdef __ANDROID__
#include "save.h"
#include "platform/android.c"
#elif __linux__
#include "platform/linux.c"
#endif
