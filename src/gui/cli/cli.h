#ifndef CLI_H
#define CLI_H

#ifndef CLI_MOCK
#include <ncurses.h>
#endif

#include <locale.h>
#include <string.h>

#include "../../defines.h"
#include "../../brick_game/tetris/entities.h"
#include "../../brick_game/tetris/backend.h"

// Определения размеров
#define GAME_HEIGHT 22
#define INFO_WIDTH 20
#define INFO_HEIGHT 22

// Размеры игрового поля (без границ)
#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20

// Цветовые пары
#define COLOR_I_PIECE 1 // Голубой
#define COLOR_O_PIECE 2 // Желтый
#define COLOR_T_PIECE 3 // Фиолетовый
#define COLOR_S_PIECE 4 // Зеленый
#define COLOR_Z_PIECE 5 // Красный
#define COLOR_J_PIECE 6 // Синий
#define COLOR_L_PIECE 7 // Оранжевый
#define COLOR_BORDER 8  // Белый для границ
#define COLOR_TEXT 9    // Белый для текста

#define GAME_WINDOW_WIDTH 24
#define GAME_WINDOW_POS_X 0
#define GAME_WINDOW_POS_Y 0

#define INFO_WINDOW_WIDTH 20
#define INFO_WINDOW_POS_X (GAME_WINDOW_POS_X + GAME_WINDOW_WIDTH + 2)
#define INFO_WINDOW_POS_Y 0

#define PIECE_WINDOW_WIDTH INFO_WINDOW_WIDTH - 2
#define PIECE_WINDOW_HEIGHT 6
#define PIECE_WINDOW_POS_X (INFO_WINDOW_POS_X + 1)
#define PIECE_WINDOW_POS_Y 1

#define MESSAGE_WINDOW_WIDTH (GAME_WINDOW_WIDTH + INFO_WIDTH - 6)
#define MESSAGE_WINDOW_HEIGHT 6
#define MESSAGE_WINDOW_POS_X (GAME_WINDOW_WIDTH / 2 - 2)
#define MESSAGE_WINDOW_POS_Y (GAME_HEIGHT / 2 - 4)

#define NEXT_POS_Y 2
#define SCORE_POS_Y 8
#define CONTROLS_POS_Y 14
#define INFO_TEXT_POS_X 2

// Символы для отображения
#define FILLED_CELL '#'
#define EMPTY_CELL ' '

// Основные функции инициализации и очистки
int init_cli(void);
void cleanup_cli(void);

// Функции для работы с цветами
void _init_colors(void);

// Внутренние функции для инициализации окон
int _init_game_window(void);
int _init_info_window(void);
int _init_piece_window(void);

// Внутренние функции для отрисовки и очистки
void _draw_window_border(WINDOW *window, const char *title);
void _clear_window(WINDOW *window, int start_y, int start_x, int height, int width);

void render_all(GameState_t *game_state);
void render_next_piece_window(Piece_type piece_type);
void render_info_window(GameInfo_t game_info);
void render_game_window(GameState_t *game_state);

// Проверка размера терминала
int check_terminal_size(void);

void show_start_info();
void show_message(const char *message);
void show_gameover_info();

#endif