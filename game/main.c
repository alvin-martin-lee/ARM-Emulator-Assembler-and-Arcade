#include "game.h"
#include "collision.h"
#include "data/levels.h"
#include "input.h"
#include "block.h"
#include "mario.h"
#include "projectile.h"
#include "goomba.h"

//----------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    {0, NULL},
    {0, NULL},
    {0, NULL},
    {0, NULL},
    {0, NULL},
    {0, NULL},
    {0, NULL},
    game_init,
    game_quit,
    NULL,
    {false, false, false, false, false, false},
    NULL,
    NULL,
    NULL,
    NULL,
    0
};

void loadSpritesheet(int width, int height, Spritesheet *spritesheet, Uint32 colourKey, int padding, char path[]) {

  SDL_Surface* surface = SDL_LoadBMP(path);
  spritesheet->n = (unsigned) ((surface->w/(width+padding))*(surface->h/(height+padding))+1);
  spritesheet->sprites = (SDL_Surface **) malloc(sizeof(SDL_Surface*) * spritesheet->n);

  int x, y;
  SDL_Rect rect = {0, 0, width, height};
  for (int i = 0; i < spritesheet->n; i++) {
    spritesheet->sprites[i] = SDL_CreateRGBSurface(0, width, height, IMG_BIT_DEPTH, 0, 0, 0, 0);
    SDL_SetColorKey(spritesheet->sprites[i], 1, colourKey);
    SDL_FillRect(spritesheet->sprites[i], 0, colourKey);
    x = i % (surface->w / width);
    y = (i - x) / (surface->w / width);
    rect.x = x * (width + padding);
    rect.y = y * (height + padding);
    SDL_BlitSurface(surface, &rect, spritesheet->sprites[i], NULL);
  }

  SDL_FreeSurface(surface);
}

void destroySpritesheet(Spritesheet *spritesheet) {
  for(int i = 0; i < spritesheet->n; i++) {
    SDL_FreeSurface(spritesheet->sprites[i]);
  }
  free(spritesheet->sprites);
}

void loadGraphics() {

  loadSpritesheet(MARIO_GFX_W, MARIO_GFX_H, &game.mario, 0xFF00FF, 0, "images/tony_1.bmp");
  loadSpritesheet(MARIO_GFX_W, MARIO_GFX_H, &game.marioBig, 0xFF00FF, 0, "images/tony_2b.bmp");
  loadSpritesheet(MARIO_GFX_W, MARIO_GFX_H, &game.marioFire, 0xFF00FF, 0, "images/tony_3.bmp");
  loadSpritesheet(TILE_SIZE, TILE_SIZE, &game.items, 0xa2d8ff, 1, "images/items.bmp");
  loadSpritesheet(TILE_SIZE, TILE_SIZE, &game.coins, 0xa2d8ff, 1, "images/coins.bmp");
  loadSpritesheet(TILE_SIZE, TILE_SIZE, &game.fireball, 0x0, 0, "images/fireball2.bmp");
  loadSpritesheet(GOOMBA_GFX_W, GOOMBA_GFX_H, &game.goombas, 0xffffff, 0, "images/mobs.bmp");

  // can't use loadSpritesheet because of "if (i != 0)" part
  SDL_Surface* surface_overworld = SDL_LoadBMP("images/tiles.bmp");
  game.overworld.n = (unsigned) ((surface_overworld->w/17)*(surface_overworld->h/17)+1);
  game.overworld.sprites = (SDL_Surface**) malloc(sizeof(SDL_Surface*)*game.overworld.n);
  int x, y;
  SDL_Rect rect_overworld = {0, 0, TILE_SIZE, TILE_SIZE};
  int overworld_padding = 1;
  for(int i = 0; i < game.overworld.n; i++){
    game.overworld.sprites[i] = SDL_CreateRGBSurface(0, TILE_SIZE, TILE_SIZE, IMG_BIT_DEPTH, 0, 0, 0, 0);
    SDL_SetColorKey(game.overworld.sprites[i], 1, 0xFF00FF);
    SDL_FillRect(game.overworld.sprites[i], 0, 0xFF00FF);
    if(i != 0) {
      x = (i - 1) % (surface_overworld->w/(TILE_SIZE + overworld_padding));
      y = (i - x) / (surface_overworld->w/(TILE_SIZE + overworld_padding));
      rect_overworld.x = x * (TILE_SIZE + overworld_padding);
      rect_overworld.y = y * (TILE_SIZE + overworld_padding);
      SDL_BlitSurface(surface_overworld, &rect_overworld, game.overworld.sprites[i], NULL);
    }
  }
  SDL_FreeSurface(surface_overworld);

  SDL_Surface* surface_hud = SDL_LoadBMP("images/hud.bmp");
  game.hud_num.n = 10;
  game.hud_num.sprites = (SDL_Surface**) malloc(sizeof(SDL_Surface*)*game.hud_num.n);
  SDL_Rect rect_hud_num = {0, 113, 8, 10};
  int hud_num_padding = 1;
  for (int i = 0; i < game.hud_num.n; i++) {
    game.hud_num.sprites[i] = SDL_CreateRGBSurface(0, 8, 10, IMG_BIT_DEPTH, 0, 0, 0, 0);
    SDL_SetColorKey(game.hud_num.sprites[i], 1, 0x7fa9c6);
    SDL_FillRect(game.hud_num.sprites[i], 0, 0x7fa9c6);
    rect_hud_num.x = i * (8 + hud_num_padding);
    SDL_BlitSurface(surface_hud, &rect_hud_num, game.hud_num.sprites[i], NULL);
  }
  SDL_FreeSurface(surface_hud);
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

  srand(12); //time(NULL));

  // Initialize game object lists
  List_init(&game.itemList);
  List_init(&game.goombaList);

  game.running = SDL_TRUE;
}

//----------------------------------------------------------------------
void game_quit(void) {
  destroySpritesheet(&game.overworld);
  destroySpritesheet(&game.mario);
  destroySpritesheet(&game.marioBig);
  destroySpritesheet(&game.marioFire);
  destroySpritesheet(&game.items);
  destroySpritesheet(&game.goombas);

  SDL_DestroyRenderer(game.screen.renderer);
  SDL_DestroyWindow(game.screen.window);

  // Close if opened
  if (SDL_JoystickGetAttached(game.joy1)) {
    SDL_JoystickClose(game.joy1);
  }
  SDL_Quit();
}

CoreObject *Mario_init() {
  SDL_Rect size = {0, 0, MARIO_GFX_W * SCREEN_SCALE, MARIO_GFX_H * SCREEN_SCALE};
  SDL_Rect hitbox = {TILE_SIZE * 5, TILE_SIZE * 10, MARIO_W, MARIO_H};
  SDL_Rect padding = {0, 0, MARIO_PADDING_W, MARIO_PADDING_H};
  CoreObject *marioCore = CoreObject_create(
      LEFT,
      size,
      hitbox,
      padding
  );
  return marioCore;
}

/** Create a 2D array of pointers to Blocks */
Block ***createBlock2DArray() {
  Block ***array = malloc(sizeof(Block **) * LEVEL_H);
  for (int i = 0; i < LEVEL_H; i++) {
    array[i] = malloc(sizeof(Block *) * LEVEL_W);
    for (int j = 0; j < LEVEL_W; j++) {
      array[i][j] = NULL;
    }
  }
  return array;
}

Block ***createLevel1Blocks() {
  Block ***array = createBlock2DArray();
  for (int i = 0; i < LEVEL_H; i++) {
    for (int j = 0; j < LEVEL_W; j++) {
      if (level1[i][j] == BRICK_TILE_ID) {
        array[i][j] = Block_create(BLOCK_TYPE_BRICK, i, j);
      }
      else if (level1[i][j] == QUESTION_TILE_IDLE_ID) {
        array[i][j] = Block_create(BLOCK_TYPE_QUESTION, i, j);
      }
    }
  }
  return array;
}

void destroyLevel1Blocks(Block ***blocks) {
  for (int i = 0; i < LEVEL_H; i++) {
    for (int j = 0; j < LEVEL_W; j++) {
      Block_destroy(blocks[i][j]);
    }
    free(blocks[i]);
  }
  free(blocks);
}

//----------------------------------------------------------------------
int main(int argc, char* argv[]) {

  game.init();

  DirectionH hdir = RIGHT;
  DirectionV vdir = NEUTRAL;
  int crouching = 0;
  int animation_offset = 0;
  int animation_counter = 0;
  int goomba_animation_offset = 0;
  int goomba_animation_counter = 0;
  int x = TILE_SIZE * 9;
  int y = TILE_SIZE * 12;
  bool goomba_stepped_on = false;

  Block ***blockArray = createLevel1Blocks();

  // keep track of jump state
  int jumpHeight = 0;
  // needed to prevent mario from continuously bouncing when the UP key is held down
  const int max_jump_height = 5 * TILE_SIZE;

  // prevent continuous fireball shooting
  bool prevShootFireball = false;
  int currentFrame = 0;

  bool prevMarioBig = false;

  // create player
  Mario *mario = Mario_create();
  CoreObject *marioCore = Mario_init();
  marioCore->size.x = (x - MARIO_PADDING_W) * SCREEN_SCALE;
  marioCore->size.y = (y - MARIO_PADDING_H) * SCREEN_SCALE;
  marioCore->hitbox.x = x;
  marioCore->hitbox.y = y;
  mario->core = marioCore;

  // add goombas to list
  List_insert_front(&game.goombaList, Goomba_create(25, 11));

  // x offset for scrolling (x = pixel on screen, x + offset = actual x-coordinate within level)
  int scroll = 4 * TILE_SIZE;

  SDL_Surface* background_surface = SDL_LoadBMP("images/back_1.bmp");
  SDL_Texture* background_texture = SDL_CreateTextureFromSurface(game.screen.renderer, background_surface);

  SDL_Surface* splash_1_surface = SDL_LoadBMP("images/splash_1.bmp");
  SDL_Surface* splash_2_surface = SDL_LoadBMP("images/splash_2.bmp");
  SDL_Texture* splash_1_texture = SDL_CreateTextureFromSurface(game.screen.renderer, splash_1_surface);
  SDL_Texture* splash_2_texture = SDL_CreateTextureFromSurface(game.screen.renderer, splash_2_surface);
  
  SDL_Surface* victory_surface = SDL_LoadBMP("images/victory.bmp");
  SDL_Texture* victory_texture = SDL_CreateTextureFromSurface(game.screen.renderer, victory_surface);
  
  SDL_Surface* defeat_surface = SDL_LoadBMP("images/defeat.bmp");
  SDL_Texture* defeat_texture = SDL_CreateTextureFromSurface(game.screen.renderer, defeat_surface);

  SDL_Surface* hud_tony_surface = SDL_LoadBMP("images/hud_tony.bmp");
  SDL_SetColorKey(hud_tony_surface, 1, 0x7fa9c6);
  SDL_Texture* hud_tony_texture = SDL_CreateTextureFromSurface(game.screen.renderer, hud_tony_surface);

  SDL_Surface* hud_lives_surface = SDL_LoadBMP("images/hud_lives.bmp");
  SDL_SetColorKey(hud_lives_surface, 1, 0x7fa9c6);
  SDL_Texture* hud_lives_texture = SDL_CreateTextureFromSurface(game.screen.renderer, hud_lives_surface);

  SDL_Rect rect_overworld = {0, 0, TILE_SIZE * SCREEN_SCALE, TILE_SIZE * SCREEN_SCALE};
  SDL_Texture* textures_overworld[game.overworld.n];
  for (int i = 0; i < game.overworld.n; i++) {
    textures_overworld[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.overworld.sprites[i]);
  }

  SDL_Texture* textures_mario[game.mario.n];
  for (int i = 0; i < game.mario.n; i++) {
    textures_mario[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.mario.sprites[i]);
  }

  SDL_Texture* textures_mario_big[game.marioBig.n];
  for (int i = 0; i < game.marioBig.n; i++) {
    textures_mario_big[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.marioBig.sprites[i]);
  }

  SDL_Texture* textures_mario_fire[game.marioFire.n];
  for (int i = 0; i < game.marioFire.n; i++) {
    textures_mario_fire[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.marioFire.sprites[i]);
  }

  SDL_Texture* textures_item[game.items.n];
  for (int i = 0; i < game.items.n; i++) {
    textures_item[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.items.sprites[i]);
  }

  SDL_Texture* textures_coins[game.coins.n];
  for (int i = 0; i < game.coins.n; i++) {
    textures_coins[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.coins.sprites[i]);
  }

  SDL_Texture* texture_fireball[game.fireball.n];
  for (int i = 0; i < game.fireball.n; i++) {
    texture_fireball[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.fireball.sprites[i]);
  }

  SDL_Texture* textures_goombas[game.goombas.n];
  for (int i = 0; i < game.goombas.n; i++) {
    textures_goombas[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.goombas.sprites[i]);
  }

  SDL_Texture* textures_hud_num[game.hud_num.n];
  for (int i = 0; i < game.hud_num.n; i++) {
    textures_hud_num[i] = SDL_CreateTextureFromSurface(game.screen.renderer, game.hud_num.sprites[i]);
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

    SDL_RenderClear(game.screen.renderer);

    switch (game.level) {
      case 0:
        currentFrame = (currentFrame + 1) % 30;

        if (game.input.b) {
          printf("Game input");
          game.level = 1;
        }

        if (currentFrame >= 15) {
          SDL_RenderCopy(game.screen.renderer, splash_2_texture, NULL, NULL);
        } else {
          SDL_RenderCopy(game.screen.renderer, splash_1_texture, NULL, NULL);
        }

        break;
      case 2:
        currentFrame++;

        if (currentFrame > 400) {
          game.level = 0;
        }

        SDL_RenderCopy(game.screen.renderer, victory_texture, NULL, NULL);
        break;
      case 3:
        currentFrame++;

        if (currentFrame > 400) {
          game.level = 0;
        }

        SDL_RenderCopy(game.screen.renderer, defeat_texture, NULL, NULL);
        break;
      case 1:
        currentFrame = (currentFrame + 1) % 60;

        for (ListIter gb = List_begin(&game.goombaList);
             gb != List_end(&game.goombaList);
             gb = List_iter_next(gb)) {
          goomba_stepped_on |= Goomba_stepped_on(mario, List_iter_value(gb));
        }
        // DEBUG ONLY
        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_SPACE]) {
          MarioState state = Mario_getState(mario);
          if (!prevShootFireball && state.fire) {
            Mario_shootFireball(mario, hdir);
            prevShootFireball = true;
          }
        } else {
          prevShootFireball = false;
        }

        x = marioCore->hitbox.x;
        y = marioCore->hitbox.y;
        SDL_Rect marioNextRect = {x + scroll, y, marioCore->hitbox.w, marioCore->hitbox.h};
        if (isOnGround(&marioNextRect, level1)) {
          jumpHeight = 0;
          if (game.input.down) {
            vdir = NEUTRAL;
            crouching = true;
            animation_counter = 0;
            animation_offset = Mario_getState(mario).small ? 0 : 5; // left/right crouch sprite
          }
          else if (game.input.up || goomba_stepped_on) {
            vdir = UP;
          }
          else {
            vdir = NEUTRAL;
            crouching = false;
          }
        }
        else {
          // apply gravity
          y += GRAVITY;
          animation_offset = 3;
        }

        if (collideTop(&marioNextRect, level1)) {
          vdir = DOWN;
          jumpHeight = 0;
          goomba_stepped_on = false;
        }

        // jump logic
        if (game.input.up || goomba_stepped_on) {
          if (vdir == UP) {
            if (jumpHeight < MAX_JUMP_HEIGHT * TILE_SIZE) {
              const int JUMP_SPEED = 8;
              jumpHeight += JUMP_SPEED;
              y -= JUMP_SPEED;
              if (jumpHeight > MAX_JUMP_HEIGHT * TILE_SIZE) {
                jumpHeight = MAX_JUMP_HEIGHT * TILE_SIZE;
              }
            }
            else {
              // fall and reset jump height
              vdir = DOWN;
              jumpHeight = 0;
              goomba_stepped_on = false;
            }
          }
        }
        else {
          // fall and reset jump height
          vdir = DOWN;
          jumpHeight = 0;
        }

        // block logic
        if (marioNextRect.y >= 0 && collideTop(&marioNextRect, level1)) {
          if (isWithinLevel(&marioNextRect)) {
            int top = marioNextRect.y / TILE_SIZE;
            int lft = marioNextRect.x / TILE_SIZE;
            int rgt = (marioNextRect.x + marioNextRect.w) / TILE_SIZE;

            // arithmetic to check if mario's head overlaps at least half of the block's' width to break/activate it
            int dxl = marioNextRect.x - lft * TILE_SIZE;
            int dxr = (rgt + 1) * TILE_SIZE - (marioNextRect.x + marioNextRect.w);
            if (dxl < TILE_SIZE / 2) {
              Mario_onCollideBlock(mario, top, lft, level1, blockArray, &game.itemList);
            }
            if (dxr < TILE_SIZE / 2) {
              Mario_onCollideBlock(mario, top, rgt, level1, blockArray, &game.itemList);
            }
          }
        }

        if (game.input.left) {
          hdir = LEFT;
          marioNextRect.x -= MARIO_MOVE_SPEED;

          if (crouching == 0) {
            if (marioNextRect.y < 0 || !collideHorizontal(&marioNextRect, hdir, level1)) {
              // Move mario or scroll screen
              if (x > TILE_SIZE * SCROLL_BUFFER) {
                // Mario is within main area
                x -= MARIO_MOVE_SPEED;
              } else if (scroll > 0) {
                // Background still within screen
                scroll -= MARIO_MOVE_SPEED;
              } else if (x >= MARIO_MOVE_SPEED) {
                // Mario is still within the screen
                x -= MARIO_MOVE_SPEED;
              }
            }
          }
        } else if (game.input.right) {
          hdir= RIGHT;
          marioNextRect.x += MARIO_MOVE_SPEED;
          if (crouching == 0) {
            if (marioNextRect.y < 0 || !collideHorizontal(&marioNextRect, hdir, level1)) {
              // Move mario or scroll screen
              if (x < SCREEN_W - (TILE_SIZE * SCROLL_BUFFER + MARIO_W)) {
                // Mario is within main area
                x += MARIO_MOVE_SPEED;
              } else if (scroll < LEVEL_W * TILE_SIZE - SCREEN_W) {
                // Background still within screen
                scroll += MARIO_MOVE_SPEED;
              } else if (x <= SCREEN_W - MARIO_W - MARIO_MOVE_SPEED) {
                x += MARIO_MOVE_SPEED;
              }
            } else if (!collideTop(&marioNextRect, level1)){
              int rgtPx = (marioNextRect.x + marioNextRect.w) / TILE_SIZE * TILE_SIZE;
              int overlap = marioNextRect.x + marioNextRect.w - rgtPx;
              x = x + MARIO_MOVE_SPEED - overlap;
            }
          }
        }

        // mario collecting items
        for (ListIter it = List_begin(&game.itemList);
             it != List_end(&game.itemList);
             it = List_iter_next(it)) {
          Item *item = (Item *) List_iter_value(it);
          if (item->type == ITEM_COIN) {
            // coin: gets collected when mario jumps under the block,
            //       it is only here for the animation
            if (item->animCountdown <= 0) {
              List_remove(&game.itemList, item);
            } else {
              if (item->animCountdown >= COIN_ANIM_MAX / 2) {
                item->hitbox.y -= 4;
              } else {
                item->hitbox.y += 4;
              }
              item->animCountdown--;
            }
          }
          else if (item->type != ITEM_COIN && collideRects(&marioNextRect, &item->hitbox)) {
            Mario_onCollectItem(mario, *item);
            List_remove(&game.itemList, item);
          }
        }

        // handle animation
        if (isOnGround(&marioNextRect, level1)) {
          if (game.input.left || game.input.right) {
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
          else {
            animation_counter = 0;
            animation_offset = (!Mario_getState(mario).small && crouching) ? 0 : 0;
          }
        }
        else {
          animation_counter = 0;
          animation_offset = 3;
        }

        // goomba animations
        for (ListIter gb = List_begin(&game.goombaList);
             gb != List_end(&game.goombaList);
             gb = List_iter_next(gb)) {
          Goomba_update(List_iter_value(gb), level1);
          if (goomba_animation_counter >= 3) {
            goomba_animation_counter = 0;
            if (goomba_animation_offset == 1) {
              goomba_animation_offset--;
            } else if (goomba_animation_offset == 0) {
              goomba_animation_offset++;
            } else if (goomba_animation_offset == 2) {
              List_remove(&game.goombaList, gb);
              Goomba_destroy(List_iter_value(gb));
            }
          } else {
            goomba_animation_counter++;
          }
          if (Goomba_stepped_on(mario, List_iter_value(gb))) {
            goomba_animation_offset = 2;
          }
        }

        SDL_RenderCopy(game.screen.renderer, background_texture, NULL, NULL);

        rect_overworld.x = scroll;
        rect_overworld.y = 0;

//    printf("Current scroll position: %i\n", scroll);
    printf("Current Mario x: %i\n", marioCore->hitbox.x);
    printf("Current Mario y: %i\n", marioCore->hitbox.y);
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
        printf("Score: %i\n", mario->points);
        printf("Lives: %i\n", mario->lives);

        if (INPUT_MODE == JOYSTICK) {
          Sint16 jx, jy;
          jx = SDL_JoystickGetAxis(game.joy1, 0);
          jy = SDL_JoystickGetAxis(game.joy1, 1);
          printf("Joystick x: %i, y: %i\n", jx, jy);
        }

        for (int i = 0; i < LEVEL_H; i++) {
          for (int j = 0; j < LEVEL_W; j++) {
            Block *block = blockArray[i][j];
            if (block != NULL) {
              if (block->type == BLOCK_TYPE_BRICK) {
                if (block->state == BLOCK_STATE_ACTIVATED) {
                  block->breakCountdown--;
                }
                if (block->breakCountdown >= 0) {
                  int brickTextureIdx = block->state == BLOCK_STATE_IDLE ? BRICK_TILE_ID : BRICK_BROKEN_ID;
                  SDL_RenderCopy(game.screen.renderer, textures_overworld[brickTextureIdx], NULL, &rect_overworld);
                }
              } else if (block->type == BLOCK_TYPE_QUESTION) {
                int questionTextureIdx = block->state == BLOCK_STATE_IDLE ? QUESTION_TILE_IDLE_ID : QUESTION_TILE_ACTIVATED_ID;
                SDL_RenderCopy(game.screen.renderer, textures_overworld[questionTextureIdx], NULL, &rect_overworld);
              }
            } else {
              SDL_RenderCopy(game.screen.renderer, textures_overworld[level1[i][j]], NULL, &rect_overworld);
            }
            rect_overworld.x += TILE_SIZE * SCREEN_SCALE;
          }
          rect_overworld.x = -scroll * SCREEN_SCALE;
          rect_overworld.y += TILE_SIZE * SCREEN_SCALE;
        }
        MarioState marioState =  Mario_getState(mario);
        marioCore->size.x = (x - MARIO_PADDING_W);
        marioCore->size.y = (y - MARIO_PADDING_H);
        marioCore->hitbox.x = x;
        marioCore->hitbox.y = y;
        if (!marioState.big && !marioState.fire) {
          marioCore->size.y -= MARIO_H / 2;
          marioCore->hitbox.h = MARIO_H / 2;
        } else {
          if (!prevMarioBig) {
            marioCore->hitbox.y -= MARIO_H / 2;
            marioCore->size.y -= MARIO_H / 2;
          }
          marioCore->hitbox.h = MARIO_H;
        }
        marioCore->size.x *= SCREEN_SCALE;
        marioCore->size.y *= SCREEN_SCALE;
        prevMarioBig = marioState.big || marioState.fire;

        int animation_startIdx = hdir == 0 ? 6 : 0;
        SDL_Texture **marioTextureArray = textures_mario;
        if (marioState.fire) {
          marioTextureArray = textures_mario_fire;
        }
        else if (marioState.big) {
          marioTextureArray = textures_mario_big;
        }
        Mario_draw(mario, game.screen.renderer, marioTextureArray[animation_startIdx + animation_offset]);

        // draw projectiles
        for (ListIter it = List_begin(&mario->projectiles);
             it != List_end(&mario->projectiles);
             it = List_iter_next(it)) {
          Projectile *proj = (Projectile *) List_iter_value(it);
          SDL_Rect r = {
              (int) proj->x * SCREEN_SCALE,
              (int) proj->y * SCREEN_SCALE,
              TILE_SIZE * SCREEN_SCALE,
              TILE_SIZE * SCREEN_SCALE};
          SDL_RenderCopy(game.screen.renderer, texture_fireball[(currentFrame / 4) % 4], NULL, &r);
          Projectile_update(proj, level1, &mario->projectiles);
        }
        // Draw Goombas
        SDL_Texture **goombaTextureArray = textures_goombas;
        for (ListIter gb = List_begin(&game.goombaList);
             gb != List_end(&game.goombaList);
             gb = List_iter_next(gb)) {
          //Goomba_draw(List_iter_value(gb), game.screen.renderer, goombaTextureArray[goomba_animation_offset]);
        }

        // Draw items
        for (ListIter it = List_begin(&game.itemList);
             it != List_end(&game.itemList);
             it = List_iter_next(it)) {
          Item *item = (Item *) List_iter_value(it);
          SDL_Rect itemDrawRect = item->hitbox;
          itemDrawRect.x = (itemDrawRect.x - scroll) * SCREEN_SCALE;
          itemDrawRect.y *= SCREEN_SCALE;
          itemDrawRect.w *= SCREEN_SCALE;
          itemDrawRect.h *= SCREEN_SCALE;
          SDL_Texture *itemTexture = NULL;
          switch(item->type) {
            case ITEM_MUSHROOM:
              itemTexture = textures_item[MUSHROOM_TILE_ID];
              break;
            case ITEM_FIREFLOWER:
              itemTexture = textures_item[FIREFLOWER_TILE_ID];
              break;
            case ITEM_STAR:
              itemTexture = textures_item[STAR_TILE_ID];
              break;
            case ITEM_ONEUP:
              itemTexture = textures_item[ONEUP_TILE_ID];
              break;
            case ITEM_COIN:
              itemTexture = textures_coins[COIN_TILE_ID];
              break;
            default:
              break;
          }
          if (itemTexture != NULL) {
            SDL_RenderCopy(game.screen.renderer, itemTexture, NULL, &itemDrawRect);
          }
        }

        SDL_Rect htt = {TILE_SIZE, TILE_SIZE, 35 * SCREEN_SCALE, 10 * SCREEN_SCALE};
        SDL_RenderCopy(game.screen.renderer, hud_tony_texture, NULL, &htt);

        SDL_Rect hms = {TILE_SIZE, TILE_SIZE + 11 * SCREEN_SCALE, 8 * SCREEN_SCALE, 10 * SCREEN_SCALE};
        int score = mario->points;
        for (int i = 7; i >= 0; i--) {
          hms.x = TILE_SIZE + 9 * i * SCREEN_SCALE;
          int numeral = score % 10;
          score /= 10;
          SDL_RenderCopy(game.screen.renderer, textures_hud_num[numeral], NULL, &hms);
        }

        SDL_Rect hlt = {SCREEN_W * SCREEN_SCALE - TILE_SIZE - 44 * SCREEN_SCALE, TILE_SIZE, 44 * SCREEN_SCALE, 10 * SCREEN_SCALE};
        SDL_RenderCopy(game.screen.renderer, hud_lives_texture, NULL, &hlt);

        hms.x = SCREEN_W * SCREEN_SCALE - TILE_SIZE - hms.w;
        SDL_RenderCopy(game.screen.renderer, textures_hud_num[mario->lives], NULL, &hms);

        if (marioCore->hitbox.y > SCREEN_H) {
          mario->lives--;

          hdir = RIGHT;
          vdir = NEUTRAL;
          crouching = 0;
          animation_offset = 0;
          animation_counter = 0;
          goomba_animation_offset = 0;
          goomba_animation_counter = 0;
          x = TILE_SIZE * 9;
          y = TILE_SIZE * 12;
          goomba_stepped_on = false;
          jumpHeight = 0;
          prevShootFireball = false;
          currentFrame = 0;
          prevMarioBig = false;
          marioCore->hitbox.x = x;
          marioCore->hitbox.y = y;
          scroll = 4 * TILE_SIZE;

          if (mario->lives <= 0) {
            mario->lives = MAX_LIVES;
            game.level = 3;
          }
        } else if (marioCore->hitbox.y > 180 && marioCore->hitbox.x == 208) {
          hdir = RIGHT;
          vdir = NEUTRAL;
          crouching = 0;
          animation_offset = 0;
          animation_counter = 0;
          goomba_animation_offset = 0;
          goomba_animation_counter = 0;
          x = TILE_SIZE * 9;
          y = TILE_SIZE * 12;
          goomba_stepped_on = false;
          jumpHeight = 0;
          prevShootFireball = false;
          currentFrame = 0;
          prevMarioBig = false;
          marioCore->hitbox.x = x;
          marioCore->hitbox.y = y;
          scroll = 4 * TILE_SIZE;
          mario->lives = MAX_LIVES;
          game.level = 2;
        }

        break;
    }

    SDL_RenderPresent(game.screen.renderer);
  }

  for (int i = 0; i < game.overworld.n; i++) {
    SDL_DestroyTexture(textures_overworld[i]);
  }

  for (int i = 0; i < game.mario.n; i++) {
    SDL_DestroyTexture(textures_mario[i]);
  }

  for (int i = 0; i < game.marioFire.n; i++) {
    SDL_DestroyTexture(textures_mario_fire[i]);
  }

  for (int i = 0; i < game.mario.n; i++) {
    SDL_DestroyTexture(textures_item[i]);
  }

  for (int i = 0; i < game.goombas.n; i++) {
    SDL_DestroyTexture(textures_goombas[i]);
  }

  destroyLevel1Blocks(blockArray);
  Mario_destroy(mario);
  List_destroy(&game.itemList);
  List_destroy(&game.goombaList);
  game.quit();

  return 0;
}
