#include "tetris.h"

#include <unistd.h>

#include "../../gui/cli/cli.h"

#ifndef UNIT_TESTING
int main() {
  srand(time(NULL));

  static GameState_t game_state = {0};

  if (init_game_state(&game_state) != SUCCESS || init_cli() != SUCCESS) {
    return ERROR;
  }

  game_loop(&game_state);

  // Очистка памяти
  cleanup_game_state(&game_state);
  cleanup_cli();

  return SUCCESS;
}
#endif

void game_loop(GameState_t *game_state) {
  static int timer = 0;
  int is_high_score_save = 0;

  while (game_state->state != STATE_EXIT) {
    UserAction_t pressed_key = convert_key_to_action(getch());

    if (pressed_key != No_action) {
      userInput(game_state, pressed_key);
    }

    if (game_state->state == STATE_GAMEOVER && !is_high_score_save) {
      save_high_score(game_state->game_info.score);
      is_high_score_save = 1;
    }

    switch (game_state->state) {
      case STATE_MOVING:
        if (++timer >= game_state->game_info.speed) {
          timer = 0;
          sigact(game_state, SIG_NOSIG);
        }
        break;

      case STATE_SPAWN:
      case STATE_ATTACHING:
      case STATE_SHIFTING:
        is_high_score_save = 0;
        sigact(game_state, SIG_NOSIG);
        timer = 0;
        break;

      default:
        timer = 0;
        break;
    }

    render_all(game_state);
    // usleep(16000);
  }
}
