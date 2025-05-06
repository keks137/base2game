#ifndef INCLUDE_SRC_SAVE_H_
#define INCLUDE_SRC_SAVE_H_

#include "stdbool.h"

typedef enum {
    STORAGE_POSITION_SCORE = 0,
    STORAGE_POSITION_HISCORE = 1
} StorageData;

bool SaveStorageValue(unsigned int position, int value);
int LoadStorageValue(unsigned int position);

bool SaveHighScore(int value);
int LoadHighScore();
bool InitStorage();

#endif // INCLUDE_SRC_SAVE_H_
