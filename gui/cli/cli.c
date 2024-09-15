/**
 * @file cli.c
 * @brief Реализация основных функций консольного интерфейса для игры "Тетрис".
 *
 * В этом файле содержатся функции, обеспечивающие взаимодействие пользователя с
 * игрой через консольный интерфейс. Реализованы обработка ввода от
 * пользователя, отображение меню, обновление состояния игры и динамическое
 * отображение информации об игровом процессе.
 *
 * Основные задачи:
 * - Управление состояниями игры и взаимодействие с меню.
 * - Обработка пользовательских команд (движения, пауза, выход).
 * - Отрисовка элементов интерфейса, включая игровое поле и панели информации.
 * - Вызовы для обновления текущего состояния игры и пользовательского ввода.
 *
 * Функции включают обработку клавиш, управление пунктами меню, отрисовку
 * элементов интерфейса и динамическое обновление информации на экране в
 * зависимости от состояния игры.
 */
#include "cli.h"

/**
 * @brief Основной интерфейс для управления состоянием игры.
 *
 * Выполняет обновление текущего состояния игры и взаимодействие с пользователем
 * через меню.
 * @retval 1 - игра продолжается
 * @retval 0 - игра завершена
 */
int interface(void) {
  static MenuItem_t current_item = Play;  ///< Текущий пункт меню.
  static int init_flag = 0;  ///< Флаг инициализации интерфейса.
  const GameInfo_t *game_info =
      updateCurrentState();  ///< Обновление информации о текущем состоянии
                             ///< игры.
  int running = 1;           ///< Флаг работы программы.
  drawInfoPanel(game_info);
  UserAction_t user_action = -1;  ///< Действие пользователя.
  if (init_flag) user_action = getKey();
  init_flag = 1;
  if (game_info->state == StateInit || game_info->state == StateIdle) {
    int confirm = 0;
    if (initMenuWin()) {
      confirm = menuKeyHandler(game_info->state == StateIdle, user_action,
                               &current_item);
    }
    if (user_action == Terminate || (confirm && current_item == Exit)) {
      running = 0;
    } else if (user_action == Start || (confirm && current_item == Play)) {
      user_action = game_info->state == StateIdle ? Pause : Start;
      nodelay(stdscr, 1);
    } else if (confirm) {
      user_action = menuUserInputMod(current_item);
      if (game_info->state == StateIdle && current_item == LevelRestart) {
        user_action = Start;
      }
    } else {
      user_action = -1;
      drawMenus(game_info->state, current_item);
    }
  } else {
    if (getMenuWin() != NULL) {
      closeMenuWin();
      current_item = Play;
    }
    drawNextPiece(game_info);
    drawGameField(game_info);
  }
  userInput(user_action);
  napms(TICK_TIME);
  return running;
}

void drawMenus(GameState_t state, MenuItem_t current_item) {
  if (state == StateInit) {
    drawMenuBodyInit(current_item);
  } else {
    drawMenuBodyIdle(current_item);
  }
  drawMenuFooter(current_item);
}

UserAction_t menuUserInputMod(MenuItem_t current_item) {
  UserAction_t user_action = -1;
  switch (current_item) {
    case IncLevel:
      user_action = Right;
      break;
    case DecLevel:
      user_action = Left;
      break;
    case IncSpeed:
      user_action = Up;
      break;
    case DecSpeed:
      user_action = Down;
      break;
    default:
      break;
  }
  return user_action;
}

UserAction_t getKey(void) {
  UserAction_t action = -1;
  switch (getch()) {
    case KEY_LEFT:
      action = Left;
      break;
    case KEY_RIGHT:
      action = Right;
      break;
    case KEY_UP:
      action = Up;
      break;
    case KEY_DOWN:
      action = Down;
      break;
    case 's':
    case 'S':
      action = Start;
      break;
    case KEY_ENTER:
    case ENTR_KEY:
    case ' ':
      action = Action;
      break;
    case 'p':
    case 'P':
      action = Pause;
      break;
    case 'q':
    case 'Q':
      action = Terminate;
      break;
    default:
      break;
  }
  flushinp();
  return action;
}

/**
 * @brief Обработка нажатия клавиши со стрелкой вверх в меню
 *
 * Если выбран пункт "Выход", то выбирается пункт "Играть".
 * Если выбран пункт "Увеличить уровень" или "Уменьшить уровень",
 * то выбирается пункт "Играть".
 * Если выбран пункт "Увеличить скорость" или "Уменьшить скорость",
 * то выбирается пункт "Перезапуск".
 * Если выбран любой другой пункт, то выбирается предыдущий пункт.
 */
static inline void menuKeyUpHandler(MenuItem_t *current_item) {
  if ((*current_item) <= Exit && (*current_item != Play)) {
    (*current_item)--;
  } else if (*current_item == IncSpeed || *current_item == DecSpeed) {
    (*current_item) = LevelRestart;
  } else if (*current_item == IncLevel || *current_item == DecLevel) {
    (*current_item) = Play;
  } else {
    (*current_item) = Exit;
  }
}

/**
 * @brief Обработка нажатия клавиши со стрелкой вниз в меню
 *
 * Если выбран пункт "Играть", то выбирается пункт "Выход".
 * Если выбран пункт "Выход", то выбирается пункт "Увеличить уровень" или
 * "Уменьшить уровень".
 * Если выбран пункт "Увеличить уровень" или "Уменьшить уровень",
 * то выбирается пункт "Увеличить скорость" или "Уменьшить скорость".
 * Если выбран любой другой пункт, то выбирается следующий пункт.
 */
static inline void menuKeyDownHandler(MenuItem_t *current_item) {
  if ((*current_item) < Exit) {
    (*current_item)++;
  } else if (*current_item == IncSpeed || *current_item == DecSpeed) {
    (*current_item) = Exit;
  } else if (*current_item == IncLevel || *current_item == DecLevel) {
    (*current_item) = Speed;
  } else {
    (*current_item) = Play;
  }
}

/**
 * @brief Обработка нажатия клавиши со стрелкой влево в меню
 *
 * Если выбран пункт "Увеличить уровень" или "Перезапуск",
 * то выбирается пункт "Уменьшить уровень".
 * Если выбран пункт "Увеличить скорость" или "Скорость",
 * то выбирается пункт "Уменьшить скорость".
 */
static inline void menuKeyLeftHandler(MenuItem_t *current_item) {
  if (*current_item == IncLevel || *current_item == LevelRestart) {
    *current_item = DecLevel;
  } else if (*current_item == IncSpeed || *current_item == Speed) {
    *current_item = DecSpeed;
  }
}

/**
 * @brief Обработка нажатия клавиши со стрелкой вправо в меню
 *
 * Если выбран пункт "Уменьшить уровень" или "Перезапуск",
 * то выбирается пункт "Увеличить уровень".
 * Если выбран пункт "Уменьшить скорость" или "Скорость",
 * то выбирается пункт "Увеличить скорость".
 */
static inline void menuKeyRightHandler(MenuItem_t *current_item) {
  if (*current_item == DecLevel || *current_item == LevelRestart) {
    *current_item = IncLevel;
  } else if (*current_item == DecSpeed || *current_item == Speed) {
    *current_item = IncSpeed;
  }
}

int menuKeyHandler(int is_idle, UserAction_t action, MenuItem_t *current_item) {
  int chosen_action = 0;
  switch (action) {
    case Action:
      chosen_action = 1;
      break;
    case Up:
      menuKeyUpHandler(current_item);
      break;
    case Down:
      menuKeyDownHandler(current_item);
      break;
    case Left:
      menuKeyLeftHandler(current_item);
      break;
    case Right:
      menuKeyRightHandler(current_item);
      break;
    default:
      break;
  }
  if (is_idle) {
    if ((*current_item) == Speed) {
      if (action == Up) {
        (*current_item) = LevelRestart;
      } else {
        (*current_item) = Exit;
      }
    }
  }
  return chosen_action;
}
