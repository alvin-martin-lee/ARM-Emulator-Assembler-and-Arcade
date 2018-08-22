cmake_minimum_required(VERSION 3.10)
project(doc_fighter C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})


add_executable(doc_fighter
        game/data/levels.h
        game/main.c
        game/item.c
        game/item.h
        game/game.c
        game/game.h
        game/input.c
        game/input.h
        game/collision.c
        game/collision.h
        game/block.c
        game/block.h
        game/input.c
        game/input.h
        game/projectile.c
        game/projectile.h
        game/mario.c
        game/mario.h
        game/linkedlist.c
        game/linkedlist.h
        game/goomba.c
        game/goomba.h
        )
target_link_libraries(doc_fighter ${SDL2_LIBRARY})
target_link_libraries(doc_fighter m)
