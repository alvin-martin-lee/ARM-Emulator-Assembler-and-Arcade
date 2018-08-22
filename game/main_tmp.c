/* Stephen's Tutorials (http://stephenmeier.net/)
gcc main.c -o game -I./include -L./include -lSDL2main -lSDL2
running on gcc 4.8.1, SDL 2.0.1
*/
//----------------------------------------------------------------------

#include "game.h"
#include "collision.h"
#include "data/levels.h"
#include "input.h"

//----------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

Game game = {
    SDL_FALSE,
    {
        SCREEN_SCALE * SCREEN_W,
        SCREEN_SCALE * SCREEN_H,
        SCREEN_NAME,
        NULL, NULL
    },
    {0, NULL},
    {0, NULL},
    game_init,
    game_quit,
    NULL,
    {false, false, false, false, false, false}
};


void loadGraphics() {
  SDL_Surface* surface_overworld = SDL_LoadBMP("images/overworld.bmp");
  game.overworld.n = (unsigned) ((surface_overworld->w/17)*(surface_overworld->h/17)+1);
  game.overworld.sprites = (SDL_Surface**) malloc(sizeof(SDL_Surface*)*game.overworld.n);

  SDL_Surface* surface_mario = SDL_LoadBMP("images/mario-2-debug.bmp");
  game.mario.n = (unsigned) ((surface_mario->w/17)*(surface_mario->h/17)+1);
  game.mario.sprites = (SDL_Surface**) malloc(sizeof(SDL_Surface*)*game.mario.n);

  int x, y;
  SDL_Rect rect_overworld = {0, 0, TILE_SIZE, TILE_SIZE};
  int overworld_padding = 1;
  for(int i = 0; i < game.overworld.n; i++){
    game.overworld.sprites[i] = SDL_CreateRGBSurface(0, TILE_SIZE, TILE_SIZE, IMG_BIT_DEPTH, 0, 0, 0, 0);
    SDL_SetColorKey(game.overworld.sprites[i], 1, 0xA2D8FF);
    SDL_FillRect(game.overworld.sprites[i], 0, 0XA2D8FF);
    if(i != 0) {
      x = (i - 1) % (surface_overworld->w/(TILE_SIZE + overworld_padding));
      y = (i - x) / (surface_overworld->w/(TILE_SIZE + overworld_padding));
      rect_overworld.x = x * (TILE_SIZE + overworld_padding);
      rect_overworld.y = y * (TILE_SIZE + overworld_padding);
      SDL_BlitSurface(surface_overworld, &rect_overworld, game.overworld.sprites[i], NULL);
    }
  }

  SDL_Rect rect_mario = {0, 0, MARIO_GFX_W, MARIO_GFX_H};
  for (int i = 0; i < game.mario.n; i++) {
    game.mario.sprites[i] = SDL_CreateRGBSurface(0, MARIO_GFX_W, MARIO_GFX_H, IMG_BIT_DEPTH, 0, 0, 0, 0);
    SDL_SetColorKey(game.mario.sprites[i], 1, 0xFF00FF);
    SDL_FillRect(game.mario.sprites[i], 0, 0xFF00FF);
    x = i % (surface_mario->w/MARIO_GFX_W);
    y = (i - x) / (surface_mario->w/MARIO_GFX_W);
    rect_mario.x = x * MARIO_GFX_W;
    rect_mario.y = y * MARIO_GFX_H;
    SDL_BlitSurface(surface_mario, &rect_mario, game.mario.sprites[i], NULL);
  }

  SDL_FreeSurface(surface_overworld);
  SDL_FreeSurface(surface_mario);
}


//----------------------------------------------------------------------
void game_init(void) {
  if(SDL_Init(SDL_INIT_EVERYTHING)!=0) {
    printf("SDL error -> %s\n", SDL_GetError());
    exit(1);
  }

  game.screen.window = SDL_CreateWindow(
      game.screen.name,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      game.screen.w, game.screen.h, 0
  );
  game.screen.renderer = SDL_CreateRenderer(
      game.screen.window, -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
  );

  loadGraphics();

  switch (INPUT_MODE) {
    case JOYSTICK:
      if (SDL_NumJoysticks() > 0) {
        // Open joystick
        game.joy1 = SDL_JoystickOpen(0);

        if (game.joy1) {
          printf("Opened joystick\n");
          printf("Name: %s\n", SDL_JoystickNameForIndex(0));
          printf("Number of Axes: %d\n", SDL_JoystickNumAxes(game.joy1));
          printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(game.joy1));
          printf("Number of Balls: %d\n", SDL_JoystickNumBalls(game.joy1));
        } else {
          printf("Couldn't open joystick\n");
          exit(1);
        }
      } else {
        printf("No joystick connected\n");
        exit(1);
      }
      break;

    case KEYBOARD:
      // No initialization needed
      break;

    default:
      printf("Unknown input source\n");
      exit(1);
  }
  SDL_InitSubSystem(SDL_INIT_JOYSTICK);
  game.running = SDL_TRUE;
}

//----------------------------------------------------------------------
void game_quit(void) {
  for(int i = 0; i < game.overworld.n; i++) {
    SDL_FreeSurface(game.overworld.sprites[i]);
  }
  free(game.overworld.sprites);
  game.overworld.sprites = NULL;
  
  for(int i = 0; i < game.mario.n; i++) {
    SDL_FreeSurface(game.mario.sprites[i]);
  }
  free(game.mario.sprites);
  game.mario.sprites = NULL;

  SDL_DestroyRenderer(game.screen.renderer);
  game.screen.renderer = NULL;

  SDL_DestroyWindow(game.screen.window);
  game.screen.window = NULL;

  // Close if opened
  if (SDL_JoystickGetAttached(game.joy1)) {
    SDL_JoystickClose(game.joy1);
  }

  SDL_Quit();
}

CoreObject *Mario_init() {
  SDL_Point startPos = {TILE_SIZE * 5, TILE_SIZE * 10};
  SDL_Rect size = {0, 0, MARIO_GFX_W * SCREEN_SCALE, MARIO_GFX_H * SCREEN_SCALE};
  SDL_Rect hitbox = {0, 0, MARIO_W, MARIO_H};
  SDL_Rect padding = {0, 0, MARIO_PADDING_W, MARIO_PADDING_H};
  CoreObject *marioCore = CoreObject_create(
      startPos,
      LEFT,
      MOVE_SPEED,
      size,
      hitbox,
      padding
  );
  return marioCore;
}

//----------------------------------------------------------------------
int main(int argc, char* argv[]) {

  game.init();

  DirectionH hdir = RIGHT;// 0 = left, 1 = right
  DirectionV vdir = NEUTRAL;// -1 = down, 0 = neutral, 1 = up
  int crouching = 0;
  int animation_offset = 0;
  int animation_counter = 0;
  int x = TILE_SIZE * 5;
  int y = TILE_SIZE * 10;

  CoreObject *marioCore = Mario_init();
  marioCore->size.x = (x - MARIO_PADDING_W) * SCREEN_SCALE;
  marioCore->size.y = (y - MARIO_PADDING_H) * SCREEN_SCALE;
  marioCore->hitbox.x = x;
  marioCore->hitbox.y = y;
  // free(marioCore);

  int scroll = 0;

  SDL_Surface* background_surface = SDL_LoadBMP("images/background.bmp");
  SDL_Texture* background_texture = SDL_CreateTextureFromSurface(game.screen.renderer, background_surface);

  SDL_Rect rect_overworld = {0, 0, TILE_SIZE * SCREEN_SCALE, TILE_SIZE * SCREEN_SCALE};
  SDL_Texture* textures_overworld[game.overworld.n];
  for (int i = 0; i < game.overworld.n; i++) {
    textures_overworld[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.overworld.sprites[i]);
  }

  SDL_Texture* textures_mario[game.mario.n];
  for (int i = 0; i < game.mario.n; i++) {
    textures_mario[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.mario.sprites[i]);
  }

  SDL_Event event;
  while(game.running) {
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT: {
          game.running = SDL_FALSE;
        } break;

        case SDL_KEYDOWN: {
          switch(event.key.keysym.sym) {
            case SDLK_ESCAPE:
              game.running = SDL_FALSE;
              break;
          }
        }
      }
    }

    getInputs(&game);

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    // printf("-----------------------------------\n");

    int left_head_tile = level1[y / TILE_SIZE][(x + scroll - MOVE_SPEED) / TILE_SIZE];
    // int left_foot_tile = level1[y / TILE_SIZE + 1][(x + scroll - MOVE_SPEED) / TILE_SIZE];

    int right_head_tile = level1[y / TILE_SIZE][(x + scroll + MARIO_W + MOVE_SPEED - 1) / TILE_SIZE];
    // int right_foot_tile = level1[y / TILE_SIZE + 1][(x + scroll + MARIO_W + MOVE_SPEED - 1) / TILE_SIZE];

    // int base_tile_left = level1[(y + MARIO_H - 1) / TILE_SIZE][(x + scroll + MARIO_W / 2 - 1) / TILE_SIZE];
    // int base_tile_right = level1[(y + MARIO_H - 1) / TILE_SIZE][(x + scroll + MARIO_W / 2) / TILE_SIZE];
    // int base_tile_offset = (x + scroll + MARIO_W / 2) % TILE_SIZE;

    // int bottom_tile = level1[(y + MARIO_H) / TILE_SIZE][(x + scroll + MARIO_W / 2) / TILE_SIZE];
    // int bottom_tile_offset = (TILE_SIZE - (y + MARIO_H) % TILE_SIZE) % TILE_SIZE;

    bool g = isOnGround(&marioCore->hitbox, level1);
    if (game.input.down && g) {
      crouching = 1;
      animation_counter = 0;
      animation_offset = 5;
    }

    if (!game.input.down) {
      crouching = 0;
    }

    int nearestPlatformBelowMario = -1;
    if (isOnGround(&marioCore->hitbox, level1)) {
      nearestPlatformBelowMario = marioCore->hitbox.y + marioCore->hitbox.h + 1;
    }

    if (nearestPlatformBelowMario != -1 && game.input.up && (marioCore->hitbox.y > nearestPlatformBelowMario - MAX_JUMP_HEIGHT) && vdir != -1) {
      vdir = 1;
      animation_offset = 3;
      y -= MOVE_SPEED;
    }

    if (!isOnGround(&marioCore->hitbox, level1)) {
      vdir = -1;
      y += MOVE_SPEED;

      if (isOnGround(&marioCore->hitbox, level1)) {
        vdir = 0;
        animation_offset = 0;
      }
    }

    // DEBUG ONLY
    if (keystates[SDL_SCANCODE_W]) {
      y -= 2;
    }

    // DEBUG ONLY
    if (keystates[SDL_SCANCODE_S]) {
      y += 1;
    }

    // DEBUG ONLY
    if (keystates[SDL_SCANCODE_R]) {
      y = TILE_SIZE * 10;
    }

    if (game.input.left) {
      hdir = LEFT;

      if (crouching == 0) {

        if (left_head_tile == 0) {
          // Move mario or scroll screen
          if (x > TILE_SIZE * SCROLL_BUFFER) {
            // Mario is within main area
            x -= MOVE_SPEED;
            /*
            if (base_tile_right == 8) {
              y -= MOVE_SPEED;
            }
             */
          } else if (scroll > 0) {
            // Background still within screen
            scroll -= MOVE_SPEED;
            /*
            if (base_tile_right == 8) {
              y -= MOVE_SPEED;
            }
             */
          } else if (x >= MOVE_SPEED) {
            // Mario is still within the screen
            x -= MOVE_SPEED;
            /*
            if (base_tile_right == 8) {
              y -= MOVE_SPEED;
            }
             */
          }
        }

        if (animation_counter >= 3) {
          animation_counter = 0;
          if (animation_offset >= 2) {
            animation_offset = 1;
          } else {
            animation_offset += 1;
          }
        } else {
          animation_counter += 1;
        }
      }
    } else if (game.input.right) {
      hdir= RIGHT;

      if (crouching == 0) {
        if (right_head_tile == 0) {
          // Move mario or scroll screen
          if (x < SCREEN_W - (TILE_SIZE * SCROLL_BUFFER + MARIO_W)) {
            // Mario is within main area
            x += MOVE_SPEED;
            /*
            if (base_tile_left == 7) {
              y -= MOVE_SPEED;
            }*/
          } else if (scroll < LEVEL_W * TILE_SIZE - SCREEN_W) {
            // Background still within screen
            scroll += MOVE_SPEED;
            /*
            if (base_tile_left == 7) {
              y -= MOVE_SPEED;
            }
             */
          } else if (x <= SCREEN_W - MARIO_W - MOVE_SPEED) {
            x += MOVE_SPEED;
            /*
            if (base_tile_left == 7) {
              y -= MOVE_SPEED;
            }
             */
          }
        }

        if (animation_counter >= 3) {
          animation_counter = 0;
          if (animation_offset >= 2) {
            animation_offset = 1;
          } else {
            animation_offset += 1;
          }
        } else {
          animation_counter += 1;
        }
      }
    } else {
      if (crouching == 0) {
        animation_counter = 0;
        animation_offset = 0;
      }
    }

    /*
    if (bottom_tile == 0) {
      y += 1;
    } else if (bottom_tile == 7) {
      if (base_tile_offset < bottom_tile_offset || bottom_tile_offset == 0) {
        y += 1;
      }
    } else if (bottom_tile == 8) {
      if (base_tile_offset >= TILE_SIZE - bottom_tile_offset || bottom_tile_offset == 0) {
        y += 1;
      }
    }
     */

    SDL_RenderClear(game.screen.renderer);

    SDL_RenderCopy(game.screen.renderer, background_texture, NULL, NULL);

    rect_overworld.x = scroll; rect_overworld.y = 0;

//    printf("Current scroll position: %i\n", scroll);
//    printf("Current Mario x: %i\n", x);
//    printf("Current Mario y: %i\n", y);
//    printf("Left head tile: %i\n", left_head_tile);
//    printf("Left foot tile: %i\n", left_foot_tile);
//    printf("Right head tile: %i\n", right_head_tile);
//    printf("Right foot tile: %i\n", right_foot_tile);
//    printf("Base tile left: %i\n", base_tile_left);
//    printf("Base tile right: %i\n", base_tile_right);
//    printf("Base tile offset: %i\n", base_tile_offset);
//    printf("Bottom tile: %i\n", bottom_tile);
//    printf("Bottom tile offset: %i\n", bottom_tile_offset);

//    printf("Input 'UP': %i\n", game.input.up);
//    printf("Input 'DOWN': %i\n", game.input.down);
//    printf("Input 'LEFT': %i\n", game.input.left);
//    printf("Input 'RIGHT': %i\n", game.input.right);
//    printf("Input 'A': %i\n", game.input.a);
//    printf("Input 'B': %i\n", game.input.b);

    if (INPUT_MODE == JOYSTICK) {
      Sint16 jx, jy;
      jx = SDL_JoystickGetAxis(game.joy1, 0);
      jy = SDL_JoystickGetAxis(game.joy1, 1);
      printf("Joystick x: %i, y: %i\n", jx, jy);
    }

    for (int i = 0; i < LEVEL_H; i++) {
      for (int j = 0; j < LEVEL_W; j++) {
        SDL_RenderCopy(game.screen.renderer, textures_overworld[level1[i][j]], NULL, &rect_overworld);
        rect_overworld.x += TILE_SIZE * SCREEN_SCALE;
      }
      rect_overworld.x = -scroll * SCREEN_SCALE;
      rect_overworld.y += TILE_SIZE * SCREEN_SCALE;
    }

    marioCore->size.x = (x - MARIO_PADDING_W) * SCREEN_SCALE;
    marioCore->size.y = (y - MARIO_PADDING_H) * SCREEN_SCALE;
    marioCore->hitbox.x = x;
    marioCore->hitbox.y = y;
    int animation_startIdx = hdir == 0 ? 6 : 0;
    SDL_RenderCopy(game.screen.renderer, textures_mario[animation_startIdx + animation_offset], NULL, &marioCore->size);

    // Draw debug rectangles
    SDL_SetRenderDrawColor(game.screen.renderer, 0, 255, 0, 0);
    SDL_RenderDrawRect(game.screen.renderer, &marioCore->size);

    SDL_Rect tmp = marioCore->hitbox;
    tmp.x *= SCREEN_SCALE;
    tmp.y *= SCREEN_SCALE;
    tmp.w *= SCREEN_SCALE;
    tmp.h *= SCREEN_SCALE;

    SDL_SetRenderDrawColor(game.screen.renderer, 0, 0, 255, 0);
    SDL_RenderDrawRect(game.screen.renderer, &tmp);

    SDL_SetRenderDrawColor(game.screen.renderer, 0, 0, 0, 0);
    SDL_RenderDrawPoint(game.screen.renderer, x, y);

    SDL_RenderPresent(game.screen.renderer);
  }

  for (int i = 0; i < game.overworld.n; i++) {
    SDL_DestroyTexture(textures_overworld[i]);
  }

  for (int i = 0; i < game.mario.n; i++) {
    SDL_DestroyTexture(textures_mario[i]);
  }

  free(marioCore);
  game.quit();

  return 0;
}
