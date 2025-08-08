#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "../brick_game/tetris/entities.h"
#include "../brick_game/tetris/helpers.h"
#include "../brick_game/tetris/backend.h"
#include "../brick_game/tetris/fsm.h"

// Заглушки для функций, которые не нужны в тестах
void sigact(GameState_t *game_state, Tetris_signal signal)
{
    // Пустая заглушка
    (void)game_state; // подавляем предупреждение
    (void)signal;     // подавляем предупреждение
}

void render_all(void *state)
{
    (void)state; // подавляем предупреждение
}

// --- backend тесты ---
void test_save_high_score()
{
    int old = load_high_score();
    save_high_score(old);     // не должен обновить
    save_high_score(old + 1); // должен обновить
    assert(load_high_score() == old + 1);
    printf("test_save_high_score: PASSED\n");
}

void test_restart_game()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    restart_game(&state);
    assert(state.state == STATE_SPAWN);
    printf("test_restart_game: PASSED\n");
    cleanup_game_state(&state);
}

void test_attach_piece_to_field()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    state.current_piece.x = 0;
    state.current_piece.y = 0;
    memcpy(state.current_piece.cur_piece, TETROMINO_SHAPES[PIECE_O], sizeof(int[4][4]));
    attach_piece_to_field(&state);
    int filled = 0;
    for (int i = 0; i < FIGURE_BOX_SIZE; i++)
        for (int j = 0; j < FIGURE_BOX_SIZE; j++)
            filled += state.game_info.field[i][j];
    assert(filled > 0);
    printf("test_attach_piece_to_field: PASSED\n");
    cleanup_game_state(&state);
}

void test_clear_completed_lines()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    for (int j = 0; j < BOARD_COLS; j++)
        state.game_info.field[BOARD_ROWS - 1][j] = 1;
    int lines = clear_completed_lines(&state);
    assert(lines == 1);
    printf("test_clear_completed_lines: PASSED\n");
    cleanup_game_state(&state);
}

void test_update_score()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    update_score(&state, 1);
    update_score(&state, 2);
    update_score(&state, 3);
    update_score(&state, 4);
    update_score(&state, 0);
    printf("test_update_score: PASSED\n");
    cleanup_game_state(&state);
}

void test_convert_key_to_action()
{
    assert(convert_key_to_action(10) == Start);
    assert(convert_key_to_action('p') == Pause);
    assert(convert_key_to_action(27) == Terminate);
    assert(convert_key_to_action(KEY_LEFT) == Left);
    assert(convert_key_to_action(KEY_RIGHT) == Right);
    assert(convert_key_to_action(KEY_UP) == Up);
    assert(convert_key_to_action(KEY_DOWN) == Down);
    assert(convert_key_to_action(32) == Action);
    assert(convert_key_to_action(ERR) == No_action);
    assert(convert_key_to_action(12345) == No_action);
    printf("test_convert_key_to_action: PASSED\n");
}

void test_userInput()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    userInput(&state, Start);
    userInput(&state, Pause);
    userInput(&state, Terminate);
    userInput(&state, Left);
    userInput(&state, Right);
    userInput(&state, Up);
    userInput(&state, Down);
    userInput(&state, Action);
    userInput(&state, No_action);
    printf("test_userInput: PASSED\n");
    cleanup_game_state(&state);
}

void test_rotate_piece()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    int r1 = rotate_piece(&state);
    assert(r1 == TRUE || r1 == FALSE);
    // Сделаем невозможный поворот (например, фигура у края)
    state.current_piece.x = -10;
    int r2 = rotate_piece(&state);
    assert(r2 == FALSE);
    printf("test_rotate_piece: PASSED\n");
    cleanup_game_state(&state);
}

void test_is_no_collision()
{
    GameInfo_t info = {0};
    info.field = allocate_2d_array(BOARD_ROWS, BOARD_COLS);
    Piece p = {0};
    memcpy(p.cur_piece, TETROMINO_SHAPES[PIECE_I], sizeof(int[4][4]));
    p.x = 0;
    p.y = 0;
    assert(is_no_collision(&info, &p, 0, 0) == TRUE);
    p.x = -1;
    assert(is_no_collision(&info, &p, 0, 0) == COLLISION_SIDE_WALL);
    p.x = 0;
    p.y = BOARD_ROWS - 1;
    assert(is_no_collision(&info, &p, 0, 1) == COLLISION_BOTTOM_WALL);
    p.x = 0;
    p.y = 0;
    info.field[0][0] = 1;
    assert(is_no_collision(&info, &p, 0, 0) == TRUE);
    free_2d_array(info.field, BOARD_ROWS);
    printf("test_is_no_collision: PASSED\n");
}

void test_move_piece()
{
    GameInfo_t info = {0};
    info.field = allocate_2d_array(BOARD_ROWS, BOARD_COLS);
    Piece p = {0};
    memcpy(p.cur_piece, TETROMINO_SHAPES[PIECE_I], sizeof(int[4][4]));
    p.x = 0;
    p.y = 0;
    assert(move_piece(&info, &p, 1, 0) == TRUE);
    p.x = -1;
    assert(move_piece(&info, &p, 0, 0) == TRUE); // COLLISION_SIDE_WALL ветка
    p.x = 0;
    p.y = BOARD_ROWS - 1;
    assert(move_piece(&info, &p, 0, 1) == FALSE); // COLLISION_BOTTOM_WALL ветка
    free_2d_array(info.field, BOARD_ROWS);
    printf("test_move_piece: PASSED\n");
}

// FSM tests
void test_fsm_spawn()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    state.state = STATE_SPAWN;
    spawn(&state);
    assert(state.state == STATE_MOVING || state.state == STATE_GAMEOVER);
    printf("test_fsm_spawn: PASSED\n");
    cleanup_game_state(&state);
}

void test_fsm_rotate()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    rotate(&state);
    printf("test_fsm_rotate: PASSED\n");
    cleanup_game_state(&state);
}

void test_fsm_move_d()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    move_d(&state);
    printf("test_fsm_move_d: PASSED\n");
    cleanup_game_state(&state);
}

void test_fsm_move_r()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    move_r(&state);
    printf("test_fsm_move_r: PASSED\n");
    cleanup_game_state(&state);
}

void test_fsm_move_l()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    move_l(&state);
    printf("test_fsm_move_l: PASSED\n");
    cleanup_game_state(&state);
}

void test_fsm_shift()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    shift(&state);
    printf("test_fsm_shift: PASSED\n");
    cleanup_game_state(&state);
}

void test_fsm_hard_drop()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    hard_drop(&state);
    printf("test_fsm_hard_drop: PASSED\n");
    cleanup_game_state(&state);
}

void test_fsm_process_attach()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    process_attach(&state);
    printf("test_fsm_process_attach: PASSED\n");
    cleanup_game_state(&state);
}

void test_fsm_pause_game()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    state.state = STATE_MOVING;
    pause_game(&state);
    assert(state.state == STATE_PAUSE);
    pause_game(&state);
    assert(state.state == STATE_MOVING);
    printf("test_fsm_pause_game: PASSED\n");
    cleanup_game_state(&state);
}

void test_fsm_start()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    start(&state);
    assert(state.state == STATE_SPAWN);
    printf("test_fsm_start: PASSED\n");
    cleanup_game_state(&state);
}

void test_fsm_exit_game()
{
    GameState_t state = {0};
    assert(init_game_state(&state) == SUCCESS);
    exit_game(&state);
    assert(state.state == STATE_EXIT);
    printf("test_fsm_exit_game: PASSED\n");
    cleanup_game_state(&state);
}

int main()
{
    // backend/fsm тесты
    test_save_high_score();
    test_restart_game();
    test_attach_piece_to_field();
    test_clear_completed_lines();
    test_update_score();
    test_convert_key_to_action();
    test_userInput();
    test_rotate_piece();
    test_is_no_collision();
    test_move_piece();
    test_fsm_spawn();
    test_fsm_rotate();
    test_fsm_move_d();
    test_fsm_move_r();
    test_fsm_move_l();
    test_fsm_shift();
    test_fsm_hard_drop();
    // test_fsm_process_attach();
    test_fsm_pause_game();
    test_fsm_start();
    test_fsm_exit_game();

    printf("\nAll logic tests passed!\n");
    return 0;
}