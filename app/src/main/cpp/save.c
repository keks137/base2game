#include "save.h"
#include "asserts.h"
#include "platform/storagePath.h"
#include "sound.h"
#include <raylib.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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
		TraceLog(LOG_INFO, "Storage initialized at: %s", fileName);
		return true;
	} else {
		TraceLog(LOG_ERROR, "Failed to create storage file: %s", fileName);
		return false;
	}
}

bool InitStorage()
{
	if (!storageInitialized) {
		const char *basePath = GetAppStoragePath();
		if (!basePath) {
			TraceLog(LOG_ERROR, "Storage path unavailable");
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

		TraceLog(LOG_ERROR, "Error opening file for reading in binary: %s", highScoreFile);
		return -1;
	}
	size_t elements_read = fread(&val, sizeof(int), vals_to_read, file_ptr);

	if (elements_read != vals_to_read) {
		perror("Error reading integer in binary");
	}
	fclose(file_ptr);
	TraceLog(LOG_INFO, "Val: %i\n", val);
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
	PlaySfxPb(SFX_MERGE, 0.1);
	ReloadData reloadData = { 0 };
	reloadData.version = RELOADDATAVERSION;
	reloadData.state = *state;
	FILE *file = fopen(reloadFile, "wb");
	if (!file) {
		TraceLog(LOG_ERROR, "couldn't open reload file: %s", reloadFile);
		return false;
	}
	TraceLog(LOG_INFO, "reloadData grid_cols: %zu", reloadData.state.grid_cols);
	bool success = true;
	success &= fwrite(&reloadData.version, sizeof(reloadData.version), 1, file) == 1;
	success &= fwrite(&reloadData.state.valid, sizeof(reloadData.state.valid), 1, file) == 1;
	success &= fwrite(&reloadData.state.grid_cols, sizeof(reloadData.state.grid_cols), 1, file) == 1;
	success &= fwrite(&reloadData.state.grid_rows, sizeof(reloadData.state.grid_rows), 1, file) == 1;
	success &= fwrite(&reloadData.state.gameGrid, sizeof(reloadData.state.gameGrid), 1, file) == 1;

	fclose(file);
	TraceLog(LOG_INFO, "saveForReload success: %zu", success);
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
	if (reloadData.version != RELOADDATAVERSION) {
		TraceLog(LOG_ERROR, "RELOADDATAVERSION mismatch");
		success = false;
	}
	success &= fread(&reloadData.state.valid, sizeof(reloadData.state.valid), 1, file) == 1;
	if (!reloadData.state.valid) {
		TraceLog(LOG_ERROR, "Loaded state invalid");
	}

	success &= fread(&reloadData.state.grid_cols, sizeof(reloadData.state.grid_cols), 1, file) == 1;
	if (reloadData.state.grid_cols != state->grid_cols) {
		TraceLog(LOG_ERROR, "GRID_COLS mismatch");
		success = false;
	}
	success &= fread(&reloadData.state.grid_rows, sizeof(reloadData.state.grid_rows), 1, file) == 1;
	if (reloadData.state.grid_rows != state->grid_rows) {
		TraceLog(LOG_ERROR, "GRID_ROWS mismatch, expected %zu, got %zu", state->grid_rows, reloadData.state.grid_rows);
		success = false;
	}
	success &= fread(&reloadData.state.gameGrid, sizeof(reloadData.state.gameGrid), 1, file) == 1;

	fclose(file);

	TraceLog(LOG_INFO, "loadReload success: %zu", success);
	if (!success) {
		return false;
	}

	*state = reloadData.state;
	return true;
}
