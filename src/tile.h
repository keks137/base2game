#ifndef INCLUDE_CPP_TILE_H_
#define INCLUDE_CPP_TILE_H_

typedef enum Animations {
    ANIMNONE,
    ANIMSPAWNING,
} Animation;

typedef struct Anim {
    Animation current;
    float prog;
} Anim;
typedef struct Tile {
    int val;
    Anim anim;
    float scale;
} Tile;



#endif  // INCLUDE_CPP_TILE_H_
