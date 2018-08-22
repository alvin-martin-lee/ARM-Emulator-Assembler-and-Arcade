To run the game, install SDL2 then run:

gcc main.c game.c block.c collision.c goomba.c input.c item.c linkedlist.c mario.c projectile.c -o game -I./include -L./include -lSDL2main -lSDL2 && ./game