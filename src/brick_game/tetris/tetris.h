#ifndef TETRIS_H
#define TETRIS_H

#include "backend.h"
#include <locale.h>
#include <ncurses.h>
#include <time.h>

extern GameState_t game_state;

void game_loop(GameState_t *game_state);

#endif