//
// Created by aaron on 6/5/18.
//

#ifndef DOC_FIGHTER_PROJECTILE_H
#define DOC_FIGHTER_PROJECTILE_H

#include "game.h"
#include "mario.h"

typedef struct Projectile {
  // should these be encapsulated in a Vector2D class?
  // should these be in floating point (even though the final rendering coordinates are int)?
  double x, y;
  double vx, vy; // horizontal/vertical velocity
  double duration;
  bool gravity;
  // Other things:
  //  - Core game object struct
  // (?) Gravity (not needed for now)
  // (?) Sprite - encapsulated with a struct?
  // (?) Duration (in frames) before it self-destructs
  // (?) Target coordinates, instead of just flying indefinitely based on vx and vy?
  // CoreObject cobj;
} Projectile;

Projectile *Projectile_create(double x, double y);
void Projectile_destroy(Projectile *projectile);
void Projectile_setVelocity(Projectile *projectile, double velX, double velY);
void Projectile_update(struct Projectile *projectile, int level[LEVEL_H][LEVEL_W], List *projectileList);
void Projectile_onCollision(Projectile *projectile, Mario *mario);// (not sure about this) do something if the projectile hits another object
void Projectile_setTarget(Projectile *projectile, double targetX, double targetY, double velocity);

// may add other functions later (e.g. set the velocity given a set of target coordinates)

#endif //DOC_FIGHTER_PROJECTILE_H
