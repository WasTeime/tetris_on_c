#ifndef ENTETIES_H
#define ENTETIES_H

#include <ncurses.h>

#define PIECE_COUNT 7

typedef enum
{
    STATE_START,
    STATE_SPAWN,
    STATE_MOVING,
    STATE_SHIFTING,
    STATE_ATTACHING,
    STATE_PAUSE,
    STATE_GAMEOVER,
    STATE_EXIT,
    NUM_STATES
} Tetris_state;

typedef enum
{
    SIG_START,
    SIG_ROTATE,
    SIG_MOVE_DOWN,
    SIG_MOVE_RIGHT,
    SIG_MOVE_LEFT,
    SIG_PAUSE,
    SIG_RESET,
    SIG_SOFT_DROP,
    SIG_FAST_DROP,
    SIG_EXIT,
    SIG_NOSIG,
    NUM_SIGNALS
} Tetris_signal;

typedef enum
{
    Start = 0,
    Pause,
    Terminate,
    Left,
    Right,
    Up,
    Down,
    Action,
    No_action,
} UserAction_t;

typedef struct
{
    int **field;
    int **next;
    int score;
    int high_score;
    int level;
    int speed;
} GameInfo_t;

typedef enum
{
    PIECE_I = 0,
    PIECE_O,
    PIECE_T,
    PIECE_S,
    PIECE_Z,
    PIECE_J,
    PIECE_L,
} Piece_type;

static const int TETROMINO_SHAPES[PIECE_COUNT][4][4] = {
    // PIECE_I - Линия
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}},

    // PIECE_O - Квадрат
    {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}},

    // PIECE_T - T-образная
    {
        {0, 0, 0, 0},
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}},

    // PIECE_S - S-образная
    {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0}},

    // PIECE_Z - Z-образная
    {
        {0, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}},

    // PIECE_J - J-образная
    {
        {0, 0, 0, 0},
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}},

    // PIECE_L - L-образная
    {
        {0, 0, 0, 0},
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}},
};

typedef struct
{
    int x;
    int y;
    int cur_piece[4][4];
    Piece_type cur_piece_type;
    Piece_type next_piece_type;
} Piece;

typedef struct
{
    GameInfo_t game_info;
    Tetris_state state;
    Piece current_piece;
} GameState_t;

#endif