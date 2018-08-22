//
// Created by aaron on 6/5/18.
//
#ifndef DOC_FIGHTER_COLLISION_H
#define DOC_FIGHTER_COLLISION_H

#include <stdbool.h>

#define EMPTY_TILE 0

enum CollisionDirection {COLL_NONE = 0, COLL_TOP = 1, COLL_BOTTOM = 2, COLL_LEFT = 4, COLL_RIGHT = 8};

int collideRects(SDL_Rect *a, SDL_Rect *b);
int collideTile(SDL_Rect *rect, int tileRowIdx, int tileColIdx);
void marioCollideBrick(SDL_Rect *marioRect, int **level);

// TODO: resolve all collisions within the game on each frame (depends on the how the game state is implemented)
// Strategies to detect collisions (example scenario: does Mario collide with any enemy?0
// - brute force O(n^2): compare with every single other enemy in the level
// - uniform grid: partition the screen into grids, so that you only need to compare mario's position with
//                 the enemies within the same grid (every time mario/an enemy moves, the grid has to be updated)
// - more complex data structures (spatial hashmap, quadtree)
void *getCollisionPairs(/* Game state, containing the level array and all Game objects */);
void resolveCollisions(/* List of colliding objects */);

bool isOnGround(SDL_Rect *rect, int level[LEVEL_H][LEVEL_W]); // check if mario is standing on the ground
bool isWithinLevel(SDL_Rect *rect);
bool collideTop(SDL_Rect *rect, int level[LEVEL_H][LEVEL_W]);
bool collideHorizontal(SDL_Rect *rect, DirectionH dir, int level[LEVEL_H][LEVEL_W]);
#endif //DOC_FIGHTER_COLLISION_H
