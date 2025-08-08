#include "cli.h"
#ifndef CLI_MOCK
#include <ncurses.h>
#endif

// Глобальные окна
WINDOW *game_window = NULL;
WINDOW *info_window = NULL;
WINDOW *piece_window = NULL;

// Определяем цвета для разных типов тетромино
#define COLOR_I_PIECE 1  // Голубой
#define COLOR_O_PIECE 2  // Желтый
#define COLOR_T_PIECE 3  // Фиолетовый
#define COLOR_S_PIECE 4  // Зеленый
#define COLOR_Z_PIECE 5  // Красный
#define COLOR_J_PIECE 6  // Синий
#define COLOR_L_PIECE 7  // Оранжевый
#define COLOR_BORDER 8
#define COLOR_TEXT 9

void _init_colors(void) {
  if (has_colors()) {
    start_color();

    init_pair(COLOR_I_PIECE, COLOR_BLACK, COLOR_CYAN);
    init_pair(COLOR_O_PIECE, COLOR_BLACK, COLOR_YELLOW);
    init_pair(COLOR_T_PIECE, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(COLOR_S_PIECE, COLOR_BLACK, COLOR_GREEN);
    init_pair(COLOR_Z_PIECE, COLOR_BLACK, COLOR_RED);
    init_pair(COLOR_J_PIECE, COLOR_BLACK, COLOR_BLUE);
    init_pair(COLOR_L_PIECE, COLOR_BLACK, COLOR_WHITE);

    init_pair(COLOR_BORDER, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_TEXT, COLOR_WHITE, COLOR_BLACK);
  }
}

// Функция для получения цвета по типу тетромино
int get_piece_color(Piece_type piece_type) {
  switch (piece_type) {
    case PIECE_I:
      return COLOR_I_PIECE;
    case PIECE_O:
      return COLOR_O_PIECE;
    case PIECE_T:
      return COLOR_T_PIECE;
    case PIECE_S:
      return COLOR_S_PIECE;
    case PIECE_Z:
      return COLOR_Z_PIECE;
    case PIECE_J:
      return COLOR_J_PIECE;
    case PIECE_L:
      return COLOR_L_PIECE;
    default:
      return COLOR_TEXT;
  }
}

// Функция для получения цвета из значения поля
int get_field_color(int field_value) {
  if (field_value >= 1 && field_value <= 7) {
    return get_piece_color(field_value - 1);
  } else if (field_value == IS_CELL_FILL) {
    return COLOR_TEXT;
  }
  return COLOR_TEXT;
}

void _draw_window_border(WINDOW *window, const char *title) {
  wattron(window, COLOR_PAIR(COLOR_BORDER));

  box(window, 0, 0);

  if (title != NULL && strlen(title) > 0) {
    mvwprintw(window, 0, 2, " %s ", title);
  }

  wattroff(window, COLOR_PAIR(COLOR_BORDER));
}

void _clear_window(WINDOW *window, int start_y, int start_x, int height,
                   int width) {
  for (int y = start_y; y < start_y + height; y++) {
    for (int x = start_x; x < start_x + width; x++) {
      mvwaddch(window, y, x, EMPTY_CELL);
    }
  }
}

int _init_game_window(void) {
  game_window = newwin(GAME_HEIGHT, GAME_WINDOW_WIDTH, GAME_WINDOW_POS_Y,
                       GAME_WINDOW_POS_X);
  if (!game_window) {
    endwin();
    printf("Failed to create game window!\n");
    return ERROR;
  }

  return SUCCESS;
}

int _init_info_window(void) {
  info_window =
      newwin(INFO_HEIGHT, INFO_WIDTH, INFO_WINDOW_POS_Y, INFO_WINDOW_POS_X);
  if (!info_window) {
    endwin();
    printf("Failed to create info window!\n");
    return ERROR;
  }

  return SUCCESS;
}

int _init_piece_window(void) {
  piece_window = newwin(PIECE_WINDOW_HEIGHT, PIECE_WINDOW_WIDTH,
                        PIECE_WINDOW_POS_Y, PIECE_WINDOW_POS_X);
  if (!piece_window) {
    endwin();
    printf("Failed to create piece window!\n");
    return ERROR;
  }
  return SUCCESS;
}

int check_terminal_size() {
  int terminal_size_y, terminal_size_x;
  getmaxyx(stdscr, terminal_size_y, terminal_size_x);

  if (terminal_size_y < 25 || terminal_size_x < 45) {
    endwin();
    printf("Terminal too small! Need at least 45x25 characters.\n");
    printf("Current size: %dx%d\n", terminal_size_x, terminal_size_y);
    return ERROR;
  }

  return SUCCESS;
}

int init_cli(void) {
  if (!initscr()) {
    printf("Failed to initialize NCurses!\n");
    return ERROR;
  }

  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  cbreak();
  timeout(50);

  if (check_terminal_size() == ERROR) {
    endwin();
    return ERROR;
  }

  _init_colors();

  if (_init_game_window() == ERROR || _init_info_window() == ERROR ||
      _init_piece_window() == ERROR) {
    cleanup_cli();
    return ERROR;
  }

  return SUCCESS;
}

void cleanup_cli(void) {
  if (game_window) {
    delwin(game_window);
    game_window = NULL;
  }

  if (info_window) {
    delwin(info_window);
    info_window = NULL;
  }

  if (piece_window) {
    delwin(piece_window);
    piece_window = NULL;
  }

  endwin();
}

// Функция отрисовки цветного блока
void draw_colored_block(WINDOW *window, int y, int x, int piece_type_or_value) {
  int color_pair = get_field_color(piece_type_or_value);

  wattron(window, COLOR_PAIR(color_pair));
  mvwaddch(window, y, x * 2, ' ');  // Левая половина блока
  mvwaddch(window, y, x * 2 + 1, ' ');  // Правая половина блока
  wattroff(window, COLOR_PAIR(color_pair));
}

void render_game_window(GameState_t *game_state) {
  werase(game_window);

  _draw_window_border(game_window, "TETRIS");

  for (int y = 0; y < BOARD_ROWS; y++) {
    for (int x = 0; x < BOARD_COLS; x++) {
      if (game_state->game_info.field[y][x] == IS_CELL_FILL ||
          game_state->game_info.field[y][x] > 0) {
        draw_colored_block(game_window, y + 1, x + 1,
                           game_state->game_info.field[y][x]);
      } else {
        mvwaddch(game_window, y + 1, (x + 1) * 2, EMPTY_CELL);
        mvwaddch(game_window, y + 1, (x + 1) * 2 + 1, EMPTY_CELL);
      }
    }
  }

  int current_piece_color =
      get_piece_color(game_state->current_piece.cur_piece_type);

  for (int i = 0; i < FIGURE_BOX_SIZE; i++) {
    for (int j = 0; j < FIGURE_BOX_SIZE; j++) {
      if (game_state->current_piece.cur_piece[i][j] == 1) {
        int field_x = game_state->current_piece.x + j;
        int field_y = game_state->current_piece.y + i;

        if (field_x >= 0 && field_x < BOARD_COLS && field_y >= 0 &&
            field_y < BOARD_ROWS) {
          wattron(game_window, COLOR_PAIR(current_piece_color));
          mvwaddch(game_window, field_y + 1, (field_x + 1) * 2,
                   ' ');  // Левая половина
          mvwaddch(game_window, field_y + 1, (field_x + 1) * 2 + 1,
                   ' ');  // Правая половина
          wattroff(game_window, COLOR_PAIR(current_piece_color));
        }
      }
    }
  }

  wnoutrefresh(game_window);
}

void render_info_window(GameInfo_t game_info) {
  werase(info_window);
  _draw_window_border(info_window, "INFO");

  wattron(info_window, COLOR_PAIR(COLOR_TEXT));

  mvwprintw(info_window, SCORE_POS_Y, INFO_TEXT_POS_X, "Score:");
  mvwprintw(info_window, SCORE_POS_Y, INFO_TEXT_POS_X + 7, "%8d",
            game_info.score);

  mvwprintw(info_window, SCORE_POS_Y + 1, INFO_TEXT_POS_X, "High:");
  mvwprintw(info_window, SCORE_POS_Y + 1, INFO_TEXT_POS_X + 7, "%8d",
            game_info.high_score);

  mvwprintw(info_window, SCORE_POS_Y + 2, INFO_TEXT_POS_X, "Level:");
  mvwprintw(info_window, SCORE_POS_Y + 2, INFO_TEXT_POS_X + 7, "%8d",
            game_info.level);

  mvwprintw(info_window, CONTROLS_POS_Y, INFO_TEXT_POS_X, "Controls:");
  mvwprintw(info_window, CONTROLS_POS_Y + 1, INFO_TEXT_POS_X,
            "Left/Right - move");
  mvwprintw(info_window, CONTROLS_POS_Y + 2, INFO_TEXT_POS_X, "Down - down");
  mvwprintw(info_window, CONTROLS_POS_Y + 3, INFO_TEXT_POS_X, "Up - rotate");
  mvwprintw(info_window, CONTROLS_POS_Y + 4, INFO_TEXT_POS_X, "ESC - quit");
  mvwprintw(info_window, CONTROLS_POS_Y + 5, INFO_TEXT_POS_X, "P - pause");

  wattroff(info_window, COLOR_PAIR(COLOR_TEXT));
  wnoutrefresh(info_window);
}

void render_next_piece_window(Piece_type piece_type) {
  // 1. Очищаем окно
  werase(piece_window);

  // 2. Рисуем рамку
  _draw_window_border(piece_window, "NEXT");

  // 3. Рисуем следующую фигуру с соответствующим цветом
  int next_piece_color = get_piece_color(piece_type);

  wattron(piece_window, COLOR_PAIR(next_piece_color));

  for (int y = 0; y < FIGURE_BOX_SIZE; y++) {
    for (int x = 0; x < FIGURE_BOX_SIZE; x++) {
      if (TETROMINO_SHAPES[piece_type][y][x] != 0) {
        mvwaddch(piece_window, y + 1, (x + 2) * 2,
                 ' ');  // Левая половина блока
        mvwaddch(piece_window, y + 1, (x + 2) * 2 + 1,
                 ' ');  // Правая половина блока
      }
    }
  }

  wattroff(piece_window, COLOR_PAIR(next_piece_color));

  // 4. подготавливаем окно к обновлению
  wnoutrefresh(piece_window);
}

void show_message(const char *message) {
  wattron(game_window, COLOR_PAIR(COLOR_TEXT));
  mvwprintw(game_window, GAME_HEIGHT / 2 - 2, 10, "%s", message);
  wattroff(game_window, COLOR_PAIR(COLOR_TEXT));

  wnoutrefresh(game_window);
}

void show_start_info() {
  werase(game_window);
  _draw_window_border(game_window, NULL);

  wattron(game_window, COLOR_PAIR(COLOR_TEXT) | A_BOLD);
  mvwprintw(game_window, GAME_HEIGHT / 2 - 5, GAME_WINDOW_WIDTH / 2 - 4,
            "Controls:");
  wattroff(game_window, COLOR_PAIR(COLOR_TEXT) | A_BOLD);

  wattron(game_window, COLOR_PAIR(COLOR_TEXT));
  mvwprintw(game_window, GAME_HEIGHT / 2 - 4, 4, "Left/Right - move");
  mvwprintw(game_window, GAME_HEIGHT / 2 - 3, 4, "Down - down");
  mvwprintw(game_window, GAME_HEIGHT / 2 - 2, 4, "Up - rotate");
  mvwprintw(game_window, GAME_HEIGHT / 2 - 1, 4, "ESC - quit");
  mvwprintw(game_window, GAME_HEIGHT / 2, 4, "P - pause");

  mvwprintw(game_window, GAME_HEIGHT / 2 + 2, 4, "Press any");
  mvwprintw(game_window, GAME_HEIGHT / 2 + 3, 4, "control button...");
  wattroff(game_window, COLOR_PAIR(COLOR_TEXT));

  wnoutrefresh(game_window);
}

void show_gameover_info() {
  werase(game_window);
  _draw_window_border(game_window, NULL);

  wattron(game_window, COLOR_PAIR(COLOR_TEXT) | A_BOLD);
  mvwprintw(game_window, GAME_HEIGHT / 2 - 5, GAME_WINDOW_WIDTH / 2 - 4,
            "GAME OVER");
  wattroff(game_window, COLOR_PAIR(COLOR_TEXT) | A_BOLD);

  wattron(game_window, COLOR_PAIR(COLOR_TEXT));
  mvwprintw(game_window, GAME_HEIGHT / 2 - 3, 4, "ESC - quit");
  mvwprintw(game_window, GAME_HEIGHT / 2 - 2, 4, "ENTER - reset");
  wattroff(game_window, COLOR_PAIR(COLOR_TEXT));

  wnoutrefresh(game_window);
}

// Главная функция рендеринга
#ifndef UNIT_TESTING
void render_all(GameState_t *game_state) {
  static int prev_state = STATE_START;
  if (prev_state == STATE_START && game_state->state != STATE_START) {
    clear();
    refresh();
  }
  prev_state = game_state->state;

  if (game_state->state == STATE_START) {
    show_start_info();
  } else if (game_state->state == STATE_GAMEOVER) {
    show_gameover_info(game_state->state);
  } else {
    render_game_window(game_state);
    render_info_window(game_state->game_info);
    render_next_piece_window(game_state->current_piece.next_piece_type);

    if (game_state->state == STATE_PAUSE) {
      show_message("PAUSE");
    }
  }

  // обновляем весь физический экран!
  doupdate();
}
#endif