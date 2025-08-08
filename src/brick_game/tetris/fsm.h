#ifndef FSM_H
#define FSM_H

#include "entities.h"
#include "backend.h"
#include <ncurses.h>

void sigact(GameState_t *game_state, Tetris_signal signal);
void spawn(GameState_t *game_state);
void rotate(GameState_t *game_state);
void move_d(GameState_t *game_state);
void move_r(GameState_t *game_state);
void move_l(GameState_t *game_state);
void hard_drop(GameState_t *game_state);
void shift(GameState_t *game_state);
void process_attach(GameState_t *game_state);
void pause_game(GameState_t *game_state);
void start(GameState_t *game_state);
void exit_game(GameState_t *game_state);

#endif