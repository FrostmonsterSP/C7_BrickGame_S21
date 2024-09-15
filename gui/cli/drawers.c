/**
 * @file drawers.c
 * @brief Реализация функций отрисовки консольного интерфейса для игры "Тетрис".
 *
 * В этом файле находятся функции, отвечающие за создание и отрисовку различных
 * элементов интерфейса, включая игровое поле, информационную панель, панель
 * управления, меню и следующую фигуру. Также реализованы вспомогательные
 * функции для инициализации и завершения работы с консолью с использованием
 * библиотеки ncurses. Присутствуют функции для обновления окна меню и отрисовки
 * элементов управления.
 *
 * Основные задачи:
 * - Инициализация окон для игры, информации, меню и управления.
 * - Отрисовка текущего состояния игрового поля и статистики.
 * - Обновление меню и отображение следующей фигуры.
 * - Взаимодействие с окнами с использованием геттеров и сеттеров.
 */
#include "cli.h"

static WINDOW *GameWin = NULL;     ///< Игровое окно
static WINDOW *InfoWin = NULL;     ///< Окно информации
static WINDOW *NextWin = NULL;     ///< Окно следующей фигуры
static WINDOW *MenuWin = NULL;     ///< Окно меню
static WINDOW *ControlWin = NULL;  ///< Окно управления

void drawGameField(const GameInfo_t *game_info) {
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      drawCell(x, y, GameWin, game_info->field[y][x]);
    }
  }
  wrefresh(GameWin);
}

void drawInfoPanel(const GameInfo_t *state) {
  int curr_y = 1;
  mvwprintw(InfoWin, curr_y++, 1, "Next Piece:");
  curr_y += NP_HEIGHT;
  mvwprintw(InfoWin, curr_y++, 1, "Score: %d       ", state->score);
  mvwprintw(InfoWin, curr_y++, 1, "Level: %d       ", state->level);
  mvwprintw(InfoWin, curr_y++, 1, "Speed: %d       ", state->speed);
  curr_y++;
  mvwprintw(InfoWin, curr_y++, 1, "High Score: %d  ", state->high_score);
  wrefresh(InfoWin);
}

void drawControlPanel(void) {
  mvwprintw(ControlWin, 0, 1, "Controls:");
  mvwprintw(ControlWin, 1, 1, "  Arrow keys:  Move ");
  mvwprintw(ControlWin, 2, 1, "  Space:       Drop ");
  mvwprintw(ControlWin, 3, 1, "  S:           Start game");
  mvwprintw(ControlWin, 4, 1, "  P:           Pause");
  mvwprintw(ControlWin, 5, 1, "  Q:           Quit ");
  wrefresh(ControlWin);
}

void drawCell(int x, int y, WINDOW *window, int cell) {
  if (cell) {
    mvwaddch(window, y + 1, x * 2 + 1, '[');
    mvwaddch(window, y + 1, x * 2 + 2, ']');
  } else {
    mvwaddch(window, y + 1, x * 2 + 1, ' ');
    mvwaddch(window, y + 1, x * 2 + 2, ' ');
  }
}

void drawNextPiece(const GameInfo_t *game_info) {
  for (int y = 0; y < FIGURE_HEIGHT; y++) {
    for (int x = 0; x < FIGURE_WIDTH; x++) {
      if (game_info->next != NULL) {
        drawCell(x, y, NextWin, game_info->next[y][x]);
      } else {
        drawCell(x, y, NextWin, 0);
      }
    }
  }
  wrefresh(NextWin);
}

/**
 * @brief Инициализация интерфейса игры.
 *
 * Настраивает и создаёт основные окна интерфейса для отображения игры.
 */
void startInterface(void) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, 1);
  curs_set(0);
  nodelay(stdscr, 1);
  GameWin = newwin(GW_HEIGHT, GW_WIDTH, GW_POS_Y, GW_POS_X);
  InfoWin = newwin(INFO_HEIGHT, INFO_WIDTH, INFO_POS_Y, INFO_POS_X);
  ControlWin = newwin(EXTRA_HEIGHT, EXTRA_WIDTH, EXTRA_POS_Y, EXTRA_POS_X);
  NextWin = derwin(InfoWin, NP_HEIGHT, NP_WIDTH, NP_POS_Y, NP_POS_X);
  box(GameWin, 0, 0);
  box(InfoWin, 0, 0);
  box(ControlWin, 0, 0);
  box(NextWin, 0, 0);
  refresh();
  wrefresh(GameWin);
  wrefresh(InfoWin);
  wrefresh(NextWin);
  drawControlPanel();
}

/**
 * @brief Завершение интерфейса игры.
 *
 * Уничтожает все окна и завершает работу библиотеки ncurses.
 */
void endInterface(void) {
  if (MenuWin != NULL) {
    wclear(MenuWin);
    delwin(MenuWin);
    MenuWin = NULL;
  }
  if (GameWin != NULL) {
    wclear(GameWin);
    delwin(GameWin);
    GameWin = NULL;
  }
  if (NextWin != NULL) {
    wclear(NextWin);
    delwin(NextWin);
    NextWin = NULL;
  }
  if (InfoWin != NULL) {
    wclear(InfoWin);
    delwin(InfoWin);
    InfoWin = NULL;
  }
  if (ControlWin != NULL) {
    wclear(ControlWin);
    delwin(ControlWin);
    ControlWin = NULL;
  }
  endwin();
}

int initMenuWin(void) {
  if (MenuWin != NULL) {
    return 1;
  }
  MenuWin = newwin(MENU_HEIGHT, MENU_WIDTH, MENU_POS_Y, MENU_POS_X);
  box(MenuWin, 0, 0);
  mvwprintw(MenuWin, 0, 2, "== TETRIS ==");
  return 0;
}

void closeMenuWin(void) {
  wclear(MenuWin);
  MenuWin = NULL;
}

void drawPlusMinus(int draw_condition, MenuItem_t current_item, int line_y) {
  int plus_x = INIT_MENU_ITEM_X + INIT_MENU_ITEM_L + 1;
  int minus_x = 1;
  if (draw_condition) {
    if (current_item > Exit && !(current_item % 2)) {
      wattron(MenuWin, A_REVERSE);
    } else {
      wattroff(MenuWin, A_REVERSE);
    }
    mvwprintw(MenuWin, line_y, plus_x, "[+]");
    if (current_item > Exit && current_item % 2) {
      wattron(MenuWin, A_REVERSE);
    } else {
      wattroff(MenuWin, A_REVERSE);
    }
    mvwprintw(MenuWin, line_y, minus_x, "[-]");
    wattron(MenuWin, A_REVERSE);
  } else {
    wattroff(MenuWin, A_REVERSE);
  }
}

void drawMenuBodyInit(MenuItem_t current_item) {
  int line_y = INIT_MENU_ITEM_Y;
  int line_x = INIT_MENU_ITEM_X;
  for (int i = line_y; i < MENU_HEIGHT - 1; i++) {
    mvwprintw(MenuWin, i, 1, "              ");
  }
  if (current_item == Play) {
    wattron(MenuWin, A_REVERSE);
  }
  mvwprintw(MenuWin, line_y++, line_x, "Play  ");
  wattroff(MenuWin, A_REVERSE);
  int draw_condition = current_item == LevelRestart ||
                       (current_item > Exit && current_item < IncSpeed);
  drawPlusMinus(draw_condition, current_item, line_y);
  mvwprintw(MenuWin, line_y++, line_x, "Level ");
  wattroff(MenuWin, A_REVERSE);
  draw_condition = current_item == Speed || current_item > DecLevel;
  drawPlusMinus(draw_condition, current_item, line_y);
  mvwprintw(MenuWin, line_y, line_x, "Speed ");
  wattroff(MenuWin, A_REVERSE);
}

void drawMenuBodyIdle(MenuItem_t current_item) {
  wattron(MenuWin, A_REVERSE);
  mvwprintw(MenuWin, 1, 1, "    Paused    ");
  wattroff(MenuWin, A_REVERSE);
  int line_y = INIT_MENU_ITEM_Y;
  int line_x = IDLE_MENU_ITEM_X;
  if (current_item == Play) {
    wattron(MenuWin, A_REVERSE);
  }
  mvwprintw(MenuWin, line_y++, line_x, " Continue ");
  wattroff(MenuWin, A_REVERSE);
  if (current_item == LevelRestart) {
    wattron(MenuWin, A_REVERSE);
  }
  mvwprintw(MenuWin, line_y++, line_x, " Restart ");
  wattroff(MenuWin, A_REVERSE);
}

void drawMenuFooter(MenuItem_t current_item) {
  if (current_item == Exit) {
    wattron(MenuWin, A_REVERSE);
  }
  mvwprintw(MenuWin, MENU_FOOTER_Y, INIT_MENU_ITEM_X, "Exit ");
  wattroff(MenuWin, A_REVERSE);
  wrefresh(MenuWin);
}

WINDOW *getGameWin(void) { return GameWin; }
WINDOW *getInfoWin(void) { return InfoWin; }
WINDOW *getNextWin(void) { return NextWin; }
WINDOW *getControlWin(void) { return ControlWin; }
WINDOW *getMenuWin(void) { return MenuWin; }

void setGameWin(WINDOW *game_win) { GameWin = game_win; }
void setInfoWin(WINDOW *info_win) { InfoWin = info_win; }
void setNextWin(WINDOW *next_win) { NextWin = next_win; }
void setControlWin(WINDOW *control_win) { ControlWin = control_win; }
void setMenuWin(WINDOW *menu_win) { MenuWin = menu_win; }
