#include "fsm.h"

void (*fsm[NUM_STATES][NUM_SIGNALS])() = {
    [STATE_START] = {start, start, start, start, start, start, start, start,
                     start, exit_game, NULL},
    [STATE_SPAWN] = {start, spawn, spawn, spawn, spawn, pause_game, spawn,
                     spawn, exit_game, spawn, spawn},
    [STATE_MOVING] = {start, rotate, move_d, move_r, move_l, pause_game, start,
                      move_d, hard_drop, exit_game, shift},
    [STATE_SHIFTING] = {start, shift, shift, shift, shift, shift, shift, shift,
                        shift, shift},
    [STATE_ATTACHING] = {start, process_attach, process_attach, process_attach,
                         process_attach, process_attach, process_attach,
                         process_attach, process_attach, process_attach,
                         process_attach},
    [STATE_PAUSE] = {start, NULL, NULL, NULL, NULL, pause_game, NULL, NULL,
                     NULL, exit_game, NULL},
    [STATE_GAMEOVER] = {start, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                        exit_game, NULL},
    [STATE_EXIT] = {exit_game, exit_game, exit_game, exit_game, exit_game,
                    exit_game, exit_game, exit_game, exit_game, exit_game,
                    exit_game}};

// есть состояния в котором находится игра, это по сути что происходит в игре в
// моменты - закончился таймер, фигура двигается и прочее action - это какие
// кнопки польлзователь нажал сигналы - это то что посылается в систему от
// нажатия кнопки

#ifndef UNIT_TESTING
void sigact(GameState_t *game_state, Tetris_signal signal) {
  void (*func)() = fsm[game_state->state][signal];
  if (func) {
    func(game_state);
  }
}
#endif

// Функции переходов состояний
void spawn(GameState_t *game_state) {
  game_state->current_piece.x = FIGURE_SPAWN_POS_X;
  game_state->current_piece.y = FIGURE_SPAWN_POS_Y;
  game_state->current_piece.cur_piece_type =
      game_state->current_piece.next_piece_type;
  memcpy(game_state->current_piece.cur_piece,
         TETROMINO_SHAPES[game_state->current_piece.cur_piece_type],
         sizeof(int[4][4]));
  game_state->current_piece.next_piece_type = get_rand_piece();

  if (!move_piece(&game_state->game_info, &game_state->current_piece, 0, 1)) {
    game_state->state = STATE_GAMEOVER;
  } else {
    game_state->state = STATE_MOVING;
  }
}

void rotate(GameState_t *game_state) {
  if (rotate_piece(game_state)) game_state->state = STATE_MOVING;
}

void move_d(GameState_t *game_state) {
  if (!move_piece(&(game_state->game_info), &(game_state->current_piece), 0,
                  1)) {
    game_state->state = STATE_ATTACHING;
  }
}

void move_r(GameState_t *game_state) {
  move_piece(&(game_state->game_info), &(game_state->current_piece), 1, 0);
}

void move_l(GameState_t *game_state) {
  move_piece(&(game_state->game_info), &(game_state->current_piece), -1, 0);
}

void shift(GameState_t *game_state) {
  if (!move_piece(&(game_state->game_info), &(game_state->current_piece), 0,
                  1)) {
    game_state->state = STATE_ATTACHING;
  }
}

void hard_drop(GameState_t *game_state) {
  while (
      move_piece(&(game_state->game_info), &(game_state->current_piece), 0, 1))
    ;
  game_state->state = STATE_ATTACHING;
}

void process_attach(GameState_t *game_state) {
  attach_piece_to_field(game_state);
  int completed_lines = clear_completed_lines(game_state);
  update_score(game_state, completed_lines);
  game_state->state = STATE_SPAWN;
}

void pause_game(GameState_t *game_state) {
  if (game_state->state != STATE_PAUSE)
    game_state->state = STATE_PAUSE;
  else
    game_state->state = STATE_MOVING;
}

void start(GameState_t *game_state) {
  restart_game(game_state);
  game_state->state = STATE_SPAWN;
}

void exit_game(GameState_t *game_state) { game_state->state = STATE_EXIT; }