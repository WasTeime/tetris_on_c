#ifndef BACKEND_H
#define BACKEND_H

#define KEY_PRESS_TO_BE_HOLD 3

#define NUM_ACTIONS 8

#define INIT_SPEED 10
#define INIT_LEVEL 1
#define INIT_SCORE 0
#define MAX_LEVEL 10

#define BOARD_ROWS 20
#define BOARD_COLS 10

#define IS_CELL_EMPTY 0
#define IS_CELL_FILL 1

#define FIGURE_SPAWN_POS_X BOARD_COLS / 2
#define FIGURE_SPAWN_POS_Y -2
#define FIGURE_BOX_SIZE 4

#define WINDOW_ROWS 20
#define WINDOW_COLS 20

#define COLLISION_PIECE -1
#define COLLISION_SIDE_WALL -2
#define COLLISION_BOTTOM_WALL -3

#include "../../defines.h"
#include "helpers.h"
#include "fsm.h"
#include "entities.h"
#include <stdlib.h>
#include <string.h>

extern int piece_bag[7];
extern int bag_index;

void shuffle_bag();
int get_rand_piece();

void save_high_score(int score);
int load_high_score();

int init_game_info(GameInfo_t *stats);
int init_game_state(GameState_t *game_state);
void restart_game(GameState_t *game_state);
void cleanup_game_state(GameState_t *game_state);

void attach_piece_to_field(GameState_t *game_state);
int clear_completed_lines(GameState_t *game_state);
void update_score(GameState_t *game_state, int lines_cleared);
int rotate_piece(GameState_t *game_state);

int is_no_collision(GameInfo_t *game_info, Piece *piece, int offset_x, int offset_y);
int move_piece(GameInfo_t *game_info, Piece *piece, int offset_x, int offset_y);

void userInput(GameState_t *game_state, UserAction_t action);
UserAction_t convert_key_to_action(int key);
#endif