//
// Created by aaron on 6/5/18.
//
#include <math.h>
#include <stdlib.h>
#include "projectile.h"
#include "collision.h"
#include "game.h"

struct Projectile *Projectile_create(double x, double y) {
  struct Projectile *proj = malloc(sizeof(struct Projectile));
  proj->x = x;
  proj->y = y;
  proj->gravity = false;
  proj->vx = proj->vy = 0;
  return proj;
}

void Projectile_destroy(struct Projectile* projectile) {
  free(projectile);
}

void Projectile_update(struct Projectile *projectile, int level[LEVEL_H][LEVEL_W], List *projectileList) {
  SDL_Rect rect = {(int) projectile->x, (int) projectile->y, TILE_SIZE, TILE_SIZE};
  if (!isWithinLevel(&rect) ||
      collideHorizontal(&rect, projectile->vx > 0 ? RIGHT : LEFT, level)) {
    Projectile_destroy(projectile);
    List_remove(projectileList, projectile);
    return;
  }
  projectile->x += projectile->vx;
  projectile->y += projectile->gravity ? GRAVITY : projectile->vy;

  // TODO: logic for collision with another character
  // - other enemy? ignore (fly through)
  // - mario? deactivate powerup / shrink mario / kill mario...
  // (special case: for bullet bill, if mario jumps on it, destroy the projectile)
}

void Projectile_setVelocity(struct Projectile *projectile, double velX, double velY) {
  projectile->vx = velX;
  projectile->vy = velY;
}

void Projectile_onCollision(Projectile *projectile, Mario *mario) {
  Mario_takeDamage(mario);
  Projectile_destroy(projectile);
}

void Projectile_setTarget(struct Projectile *projectile, double targetX, double targetY, double velocity) {
  // assert targetX and targetY are valid coordinates within the level
  double dx = targetX - projectile->x;
  double dy = targetY - projectile->y;
  double theta = atan2(dy, dx);
  projectile->vx = velocity * cos(theta);
  projectile->vy = velocity * sin(theta);
}
