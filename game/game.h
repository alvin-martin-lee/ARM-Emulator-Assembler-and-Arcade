//----------------------------------------------------------------------
#ifndef _GAME_H_
#define _GAME_H_

#define SDL_MAIN_HANDLED

//----------------------------------------------------------------------
#define SCREEN_W 320
#define SCREEN_H 256
#define SCREEN_NAME "Super MariOh Baby!"
#define SCREEN_SCALE 4

#define MARIO_MOVE_SPEED 2
#define LEVEL_W 267
#define LEVEL_H 16
#define TILE_SIZE 16        // px size of each tile sprite
#define MARIO_GFX_W 32      // px size of each mario sprite
#define MARIO_GFX_H 48      // px size of each mario sprite
#define MARIO_W 16          // hit box width
#define MARIO_H 32          // hit box height
#define MARIO_PADDING_W ((MARIO_GFX_W - MARIO_W) / 2)
#define MARIO_PADDING_H ((MARIO_GFX_H - MARIO_H) / 2)
#define IMG_BIT_DEPTH 24
#define SCROLL_BUFFER 10
#define GRAVITY 4

#define MAX_JUMP_HEIGHT 10

#define GOOMBA_MOVE_SPEED 1
#define GOOMBA_GFX_W 16
#define GOOMBA_GFX_H 16
#define GOOMBA_W 16
#define GOOMBA_H 16
#define GOOMBA_PADDING_W 8
#define GOOMBA_PADDING_H 8

#define QUESTION_TILE_IDLE_ID 7
#define QUESTION_TILE_ACTIVATED_ID 8
#define BRICK_TILE_ID 22
#define BRICK_BROKEN_ID 23

#define MUSHROOM_TILE_ID 0
#define FIREFLOWER_TILE_ID 84
#define STAR_TILE_ID 211
#define ONEUP_TILE_ID 72
#define COIN_TILE_ID 0

enum {JOYSTICK, KEYBOARD} InputMode;
#define INPUT_MODE KEYBOARD

//----------------------------------------------------------------------
#include <stdbool.h>
#include "include/SDL2/SDL.h"
#include "linkedlist.h"
//----------------------------------------------------------------------

typedef enum {
  LEFT, RIGHT
} DirectionH;

typedef enum {
  DOWN = -1, NEUTRAL = 0, UP = 1
} DirectionV;

typedef struct {
  bool alive;
  DirectionH current_direction;
  SDL_Rect size;
  SDL_Rect hitbox;
  SDL_Rect padding;
} CoreObject;

// Core structure helper functions

CoreObject *CoreObject_create(DirectionH starting_direction, SDL_Rect
size, SDL_Rect hitbox, SDL_Rect padding);

void CoreObject_destroy(CoreObject *core_object);

void game_init(void);
void game_quit(void);

typedef struct {
  unsigned int n;
  SDL_Surface** sprites;
} Spritesheet;

typedef struct {
  bool up;
  bool down;
  bool left;
  bool right;
  bool a;
  bool b;
} Inputs;

typedef struct {
  // define "attributes"
  SDL_bool running;

  struct {
    unsigned int w;
    unsigned int h;
    const char* name;
    SDL_Window* window;
    SDL_Renderer* renderer;
  } screen;

  Spritesheet overworld;
  Spritesheet mario;
  Spritesheet marioBig;
  Spritesheet marioFire;
  Spritesheet items;
  Spritesheet coins;
  Spritesheet fireball;
  Spritesheet goombas;
  Spritesheet hud_num;

  // define "methods"
  void (*init)(void);
  void (*quit)(void);

  // define joystick
  SDL_Joystick* joy1;

  Inputs input;

  // List of items
  List itemList;
  //List of goombas
  List goombaList;

  unsigned int level;
} Game;


#endif
