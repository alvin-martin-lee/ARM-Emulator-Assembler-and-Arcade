//
// Created by Matthew Pull on 06/06/2018.
//

#include "input.h"

void getInputs(Game* game) {
  const Uint8* keystates;
  Sint16 jx, jy;

  switch (INPUT_MODE) {
    case KEYBOARD:
      keystates = SDL_GetKeyboardState(NULL);

      game->input.up = keystates[SDL_SCANCODE_UP];
      game->input.down = keystates[SDL_SCANCODE_DOWN];
      game->input.left = keystates[SDL_SCANCODE_LEFT];
      game->input.right = keystates[SDL_SCANCODE_RIGHT];
      game->input.a = keystates[SDL_SCANCODE_SPACE];
      game->input.b = keystates[SDL_SCANCODE_RETURN];
      break;

    case JOYSTICK:
      jx = SDL_JoystickGetAxis(game->joy1, 0);
      jy = SDL_JoystickGetAxis(game->joy1, 1);
      printf("Joystick x: %i, y: %i\n", jx, jy);
      game->input.up = SDL_JoystickGetButton(game->joy1, 2);
      game->input.down = jx < -AXIS_THRESHOLD;
      game->input.left = jy < -AXIS_THRESHOLD;
      game->input.right = (jy > AXIS_THRESHOLD);
      game->input.a = SDL_JoystickGetButton(game->joy1, 0);
      game->input.b = SDL_JoystickGetButton(game->joy1, 1);

    default:
      break;
  }
}