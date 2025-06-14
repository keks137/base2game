#ifndef INCLUDE_CPP_STATE_H_
#define INCLUDE_CPP_STATE_H_

#include "tile.h"
#include "constants.h"
#include <inttypes.h>
typedef struct game_state_t {
    uint16_t grid_cols;
    uint16_t grid_rows;
    Tile gameGrid[GRID_COLS][GRID_ROWS];
} game_state_t;

static game_state_t g_gameState = { 0 };


#endif  // INCLUDE_CPP_STATE_H_
