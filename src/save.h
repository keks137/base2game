#ifndef INCLUDE_SRC_SAVE_H_
#define INCLUDE_SRC_SAVE_H_

#include <stdint.h>
extern char storageDataFile[256];
#include "stdbool.h"
#include "state.h"

#define RELOADDATAVERSION 1

typedef struct {
	uint8_t version;
	game_state_t state;
	// Tile *tileBuf;
} ReloadData;

typedef struct {
	char *fullName;
} StorageFile;

bool SaveHighScore(int value);
int LoadHighScore();
bool InitStorage();
bool loadReload(game_state_t *state);
bool saveForReload(game_state_t *state);

#endif // INCLUDE_SRC_SAVE_H_
