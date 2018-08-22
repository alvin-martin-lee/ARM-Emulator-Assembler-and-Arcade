//
// Created by Christopher Huang on 6/6/2018.
//
#include <stdio.h>
#include <stdbool.h>
#include "include/SDL2/SDL.h"
#include "game.h"

// Core structure helper functions

CoreObject *CoreObject_create(DirectionH starting_direction, SDL_Rect
size, SDL_Rect hitbox, SDL_Rect padding) {
  CoreObject *core_object = malloc(sizeof(CoreObject));
  core_object->alive = true;
  core_object->current_direction = starting_direction;
  core_object->size = size;
  core_object->hitbox = hitbox;
  core_object->padding.w = TILE_SIZE;
  core_object->padding.h = TILE_SIZE;
  return core_object;
}

void CoreObject_destroy(CoreObject *core_object) {
  free(core_object);
}
