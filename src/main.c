#include <stdlib.h>
#include <time.h>

#include "brick_game/tetris/tetris.h"
#include "gui/cli/cli.h"

GameState_t game_state = {0};

int main() {
  srand(time(NULL));

  if (init_game_state(&game_state) != SUCCESS || init_cli() != SUCCESS) {
    return ERROR;
  }

  game_loop(&game_state);

  // Очистка памяти
  cleanup_game_state(&game_state);
  cleanup_cli();

  return SUCCESS;
}