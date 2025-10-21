#include "save.h"
#include "asserts.h"
#include "deps/raymob/raymob.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <android/log.h>

#define LOG_TAG "MyNativeTag"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ...

static pthread_mutex_t storageMutex = PTHREAD_MUTEX_INITIALIZER;
static char *highScoreFileName = "storage.data";
static char *reloadFileName = "state.data";

static char highScoreFile[256];
static char reloadFile[256];
static bool storageInitialized = false;
int badGlobalHighscoreVarPlsRemoveLater = 0;

bool initFile(char *fileName)
{
	FILE *file =
		fopen(fileName, "ab+");
	if (file) {
		fclose(file);
		LOGD("Storage initialized at: %s", fileName);
		return true;
	} else {
		LOGE("Failed to create storage file: %s", fileName);
		return false;
	}
}

bool InitStorage()
{
	if (!storageInitialized) {
		const char *basePath = GetAppStoragePath();
		if (!basePath) {
			LOGE("Storage path unavailable");
			return false;
		}

		mkdir(basePath, 0755);

		snprintf(highScoreFile, sizeof(highScoreFile), "%s/%s", basePath, highScoreFileName);
		snprintf(reloadFile, sizeof(reloadFile), "%s/%s", basePath, reloadFileName);

		if (!initFile(highScoreFile))
			return false;
		if (!initFile(reloadFile))
			return false;
	}

	storageInitialized = true;
	return true;
}

bool BadSaveVal(int value)
{
	FILE *file_ptr = fopen(highScoreFile, "wb");

	if (file_ptr == NULL) {
		perror("Error opening file for writing");
		return false;
	}

	size_t elements_written = fwrite(&value, sizeof(int), 1, file_ptr);

	if (elements_written != 1) {
		perror("Error writing integer in binary");
		fclose(file_ptr);
		return false;
	}

	fclose(file_ptr);

	return true;
}

int BadReadVal()
{
	const uint vals_to_read = 1;
	int val = -1;
	FILE *file_ptr = fopen(highScoreFile, "rb");
	if (file_ptr == NULL) {
		perror("Error opening file for reading in binary");

		LOGD("Error opening file for reading in binary: %s", highScoreFile);
		return -1;
	}
	size_t elements_read = fread(&val, sizeof(int), vals_to_read, file_ptr);

	if (elements_read != vals_to_read) {
		perror("Error reading integer in binary");
	}
	fclose(file_ptr);
	LOGD("Val: %i\n", val);
	return val;
}

bool SaveHighScore(int value)
{
	return BadSaveVal(value);
}

int LoadHighScore()
{
	return BadReadVal();
}

bool saveForReload(game_state_t *state)
{
	ReloadData reloadData = { 0 };
	reloadData.version = RELOADDATAVERSION;
	reloadData.state = *state;
	FILE *file = fopen(reloadFile, "wb");
	if (!file)
		return false;

	bool success = true;
	success &= fwrite(&reloadData.version, sizeof(reloadData.version), 1, file) == 1;
	success &= fwrite(&reloadData.state.valid, sizeof(reloadData.state.valid), 1, file) == 1;
	success &= fwrite(&reloadData.state.grid_cols, sizeof(reloadData.state.grid_cols), 1, file) == 1;
	success &= fwrite(&reloadData.state.grid_rows, sizeof(reloadData.state.grid_rows), 1, file) == 1;
	success &= fwrite(&reloadData.state.gameGrid, sizeof(reloadData.state.gameGrid), 1, file) == 1;

	fclose(file);
	return success;
}

bool loadReload(game_state_t *state)
{
	ReloadData reloadData = { 0 };
	FILE *file = fopen(reloadFile, "rb");
	if (!file)
		return false;

	bool success = true;
	success &= fread(&reloadData.version, sizeof(reloadData.version), 1, file) == 1;
	assert(reloadData.version == RELOADDATAVERSION);
	success &= fread(&reloadData.state.grid_cols, sizeof(reloadData.state.grid_cols), 1, file) == 1;
	assert(reloadData.state.grid_cols == GRID_COLS);
	success &= fread(&reloadData.state.grid_rows, sizeof(reloadData.state.grid_rows), 1, file) == 1;
	assert(reloadData.state.grid_rows == GRID_ROWS);
	success &= fread(&reloadData.state.gameGrid, sizeof(reloadData.state.gameGrid), 1, file) == 1;

	fclose(file);

	if (!success) {
		return false;
	}

	*state = reloadData.state;
	return true;
}
