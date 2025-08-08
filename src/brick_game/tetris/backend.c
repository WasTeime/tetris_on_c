#include "backend.h"

int piece_bag[7];
int bag_index = 0;

void shuffle_bag() {
  for (int i = 0; i < PIECE_COUNT; i++) {
    piece_bag[i] = i;
  }

  for (int i = 6; i > 0; --i) {
    int rand_i = rand() % (i + 1);
    int temp = piece_bag[i];
    piece_bag[i] = piece_bag[rand_i];
    piece_bag[rand_i] = temp;
  }

  bag_index = 0;
}

int get_rand_piece() {
  if (bag_index >= PIECE_COUNT) {
    shuffle_bag();
  }
  return piece_bag[bag_index++];
}

void save_high_score(int score) {
  int old_high_score = load_high_score();

  if (score <= old_high_score) {
    return;
  }

  FILE *file = fopen("high_score.txt", "w");
  if (file != NULL) {
    fprintf(file, "%d", score);
    fclose(file);
  }
}

int load_high_score() {
  int score = 0;
  FILE *file = fopen("high_score.txt", "r");

  if (file == NULL) {
    return 0;
  }

  if (fscanf(file, "%d", &score) != 1) {
    score = 0;
  }

  fclose(file);
  return score;
}

int init_game_info(GameInfo_t *stats) {
  stats->field = allocate_2d_array(BOARD_ROWS, BOARD_COLS);
  if (!stats->field) return ERROR;

  stats->next = allocate_2d_array(FIGURE_BOX_SIZE, FIGURE_BOX_SIZE);
  if (!stats->next) return ERROR;

  stats->high_score = load_high_score();
  stats->level = INIT_LEVEL;
  stats->score = INIT_SCORE;
  stats->speed = INIT_SPEED;

  return SUCCESS;
}

int init_game_state(GameState_t *game_state) {
  if (init_game_info(&game_state->game_info) != SUCCESS) {
    return ERROR;
  }

  shuffle_bag();

  int random_piece_type = get_rand_piece();

  memcpy(game_state->current_piece.cur_piece,
         TETROMINO_SHAPES[random_piece_type], sizeof(int[4][4]));
  game_state->current_piece.cur_piece_type = random_piece_type;
  game_state->current_piece.next_piece_type = get_rand_piece();
  game_state->current_piece.x = FIGURE_SPAWN_POS_X;
  game_state->current_piece.y = FIGURE_SPAWN_POS_Y;

  game_state->state = STATE_START;

  return SUCCESS;
}

void restart_game(GameState_t *game_state) {
  cleanup_game_state(game_state);

  init_game_state(game_state);

  game_state->state = STATE_SPAWN;
}

void cleanup_game_state(GameState_t *game_state) {
  free_2d_array(game_state->game_info.field, BOARD_ROWS);
  free_2d_array(game_state->game_info.next, FIGURE_BOX_SIZE);
  game_state->game_info.field = NULL;
  game_state->game_info.next = NULL;
}

void attach_piece_to_field(GameState_t *game_state) {
  for (int i = 0; i < FIGURE_BOX_SIZE; i++) {
    for (int j = 0; j < FIGURE_BOX_SIZE; j++) {
      if (game_state->current_piece.cur_piece[i][j] == 1) {
        int field_x = game_state->current_piece.x + j;
        int field_y = game_state->current_piece.y + i;

        game_state->game_info.field[field_y][field_x] = 1;
      }
    }
  }
}

int clear_completed_lines(GameState_t *game_state) {
  int lines_cleared = 0;

  // Проверяем строки снизу вверх
  for (int y = BOARD_ROWS - 1; y >= 0; y--) {
    int is_full = 1;
    for (int x = 0; x < BOARD_COLS; x++) {
      if (game_state->game_info.field[y][x] == 0) {
        is_full = 0;
        break;
      }
    }

    if (is_full) {
      lines_cleared++;
      // Сдвигаем все строки выше на одну вниз
      for (int above_y = y; above_y > 0; above_y--) {
        for (int x = 0; x < BOARD_COLS; x++) {
          game_state->game_info.field[above_y][x] =
              game_state->game_info.field[above_y - 1][x];
        }
      }

      for (int x = 0; x < BOARD_COLS; x++) {
        game_state->game_info.field[0][x] = 0;
      }

      // компенсируем y--
      y++;
    }
  }

  return lines_cleared;
}

void update_score(GameState_t *game_state, int lines_cleared) {
  if (lines_cleared > 0) {
    int score_increment = 0;
    switch (lines_cleared) {
      case 1:
        score_increment = 100;
        break;
      case 2:
        score_increment = 300;
        break;
      case 3:
        score_increment = 700;
        break;
      case 4:
        score_increment = 1500;
        break;
    }
    game_state->game_info.score += score_increment;
    game_state->game_info.level =
        (game_state->game_info.score / 600 + 1 > MAX_LEVEL)
            ? MAX_LEVEL
            : game_state->game_info.score / 600 + 1;

    int new_speed = INIT_SPEED - game_state->game_info.level;
    game_state->game_info.speed = new_speed < 2 ? 2 : new_speed;
  }
}

int rotate_piece(GameState_t *game_state) {
  // Создаём временную матрицу для повёрнутой фигуры
  Piece temp_piece = game_state->current_piece;

  // Поворачиваем фигуру на 90 градусов по часовой стрелке
  for (int i = 0; i < FIGURE_BOX_SIZE; i++) {
    for (int j = 0; j < FIGURE_BOX_SIZE; j++) {
      temp_piece.cur_piece[j][FIGURE_BOX_SIZE - 1 - i] =
          game_state->current_piece.cur_piece[i][j];
    }
  }

  if (is_no_collision(&game_state->game_info, &temp_piece, 0, 0) == TRUE) {
    game_state->current_piece = temp_piece;
    return TRUE;
  }

  return FALSE;
}

void userInput(GameState_t *game_state, UserAction_t action) {
  Tetris_signal signal = SIG_NOSIG;

  switch (action) {
    case Start:
      signal = SIG_START;
      break;
    case Pause:
      signal = SIG_PAUSE;
      break;
    case Terminate:
      signal = SIG_EXIT;
      break;
    case Left:
      signal = SIG_MOVE_LEFT;
      break;
    case Right:
      signal = SIG_MOVE_RIGHT;
      break;
    case Up:
      signal = SIG_ROTATE;
      break;
    case Down:
      signal = SIG_MOVE_DOWN;
      break;
    case Action:
      signal = SIG_FAST_DROP;
      break;
    case No_action:
    default:
      return;
  }

  sigact(game_state, signal);
}

UserAction_t convert_key_to_action(int key) {
  switch (key) {
    case 10:
      return Start;
    case 'p':
      return Pause;
    case 27:
      return Terminate;  // ESC
    case KEY_LEFT:
      return Left;
    case KEY_RIGHT:
      return Right;
    case KEY_UP:
      return Up;
    case KEY_DOWN:
      return Down;
    case 32:
      return Action;  // Space
    case ERR:
    default:
      return No_action;
  }
}

int is_no_collision(GameInfo_t *game_info, Piece *piece, int offset_x,
                    int offset_y) {
  for (int i = 0; i < FIGURE_BOX_SIZE; i++) {
    for (int j = 0; j < FIGURE_BOX_SIZE; j++) {
      if (piece->cur_piece[i][j] == 1) {
        int field_x = piece->x + j + offset_x;
        int field_y = piece->y + i + offset_y;

        if (field_x < 0 || field_x >= BOARD_COLS) {
          return COLLISION_SIDE_WALL;
        } else if (field_y >= BOARD_ROWS) {
          return COLLISION_BOTTOM_WALL;
        } else if (field_y >= 0 && game_info->field[field_y][field_x] == 1) {
          return COLLISION_PIECE;
        }
      }
    }
  }

  return TRUE;
}

int move_piece(GameInfo_t *game_info, Piece *piece, int offset_x,
               int offset_y) {
  int collision = is_no_collision(game_info, piece, offset_x, offset_y);

  if (collision == TRUE) {
    piece->x += offset_x;
    piece->y += offset_y;
    return TRUE;
  } else if (collision == COLLISION_SIDE_WALL) {
    piece->y += offset_y;
    return TRUE;
  } else
    return FALSE;
}