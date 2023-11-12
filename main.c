#include <curses.h>
#include <string.h>
#include <stdlib.h>

struct Player {
    int x;
    int y;
};

struct Ball {
    int x;
    int y;
    int direction_horizontal;
    int direction_vertical;
};

const int PLAYER_WIDTH = 30;
const char* PLAYER_TEXT = "##############################";
const int PLAYER_HEIGHT = 3;

const int WINDOW_HEIGHT = 20;
const int WINDOW_WIDTH = 80;

const int BRICK_WIDTH = 4;
const int BRICKS_PER_ROW = (WINDOW_WIDTH - 2) / BRICK_WIDTH;
const int BRICKS_HEIGHT = WINDOW_HEIGHT - 3;

#define BRICKS_TOTAL 57

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
int main() {

    initscr();
    keypad(stdscr, true);
    cbreak();
    noecho();
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_RED);
        init_pair(2, COLOR_GREEN, COLOR_GREEN);
        init_pair(3, COLOR_BLUE, COLOR_BLUE);
        init_pair(4, COLOR_WHITE, COLOR_WHITE);
    }

    WINDOW* window = newwin(WINDOW_HEIGHT, WINDOW_WIDTH, 0, 0);
    keypad(window, true);
    wtimeout(window, 0);
    leaveok(window, true);
    refresh();

    int key_pressed = 0;
    bool bricks[BRICKS_TOTAL] = {};
    int broken_bricks = 0;
    bool game_over = false;
    bool player_won = false;

    struct Player player = {(WINDOW_WIDTH - PLAYER_WIDTH) / 2, PLAYER_HEIGHT};
    struct Ball ball = {WINDOW_WIDTH / 2 -5, 5, 1, -1};

    while (true) {
        box(window, 0, 0);
        if (has_colors()) {
            wattron(window, COLOR_PAIR(4));
        }
        mvwprintw(window, WINDOW_HEIGHT - player.y, player.x, PLAYER_TEXT);
        if (has_colors()) {
            wattroff(window, COLOR_PAIR(4));
        }
        mvwprintw(window, WINDOW_HEIGHT - ball.y, ball.x, "o");
        mvprintw(WINDOW_HEIGHT, 2, "Arrow Keys - Left/Right, Q - quit");
        for (int i = 0; i < BRICKS_TOTAL; i++) {
            if (!bricks[i]) {
                if (has_colors()) {
                    wattron(window, COLOR_PAIR(i / (BRICKS_TOTAL / 3) + 1));
                }
                mvwprintw(window, WINDOW_HEIGHT - BRICKS_HEIGHT + (i / BRICKS_PER_ROW), 2 + (i % BRICKS_PER_ROW) * BRICK_WIDTH, "%c%c%c ", 'A' + i, 'A' + i, 'A' + i);
                if (has_colors()) {
                    wattroff(window, COLOR_PAIR(i / (BRICKS_TOTAL / 3) + 1));
                }
            }
        }

        if (game_over) {
            if (player_won) {
                mvwprintw(window, WINDOW_HEIGHT / 2 - 1, WINDOW_WIDTH / 2 - 4, "YOU WON");
            } else {
                mvwprintw(window, WINDOW_HEIGHT / 2 - 1, WINDOW_WIDTH / 2 - 5, "GAME OVER");
            }
            mvwprintw(window, WINDOW_HEIGHT / 2, WINDOW_WIDTH / 2 - 19, "Press ENTER to restart and Q to quit.");
        } else {
            ball.x += ball.direction_horizontal;
            ball.y += ball.direction_vertical;

            if (ball.x <= 2) {
                ball.direction_horizontal = 1;
            } else if (ball.x >= WINDOW_WIDTH - 2) {
                ball.direction_horizontal = -1;
            }
            if (ball.y <= 0) {
                game_over = true;
            } else if (ball.x >= player.x && ball.x <= player.x + PLAYER_WIDTH && ball.y == PLAYER_HEIGHT + 1) {
                int relative_pos = (ball.x - player.x) * 9 / PLAYER_WIDTH;
                if (relative_pos < 3) {
                    ball.direction_horizontal = -2;
                } else if (relative_pos < 6) {
                    ball.direction_horizontal = abs(ball.direction_horizontal) / ball.direction_horizontal;
                } else {
                    ball.direction_horizontal = 2;
                }
                ball.direction_vertical = 1;
            } else if (ball.y >= WINDOW_HEIGHT - 1) {
                ball.direction_vertical = -1;
            } else if (ball.direction_vertical == 1) {
                int brick = -(ball.y + 1 - BRICKS_HEIGHT) * BRICKS_PER_ROW + (ball.x - 2) / BRICK_WIDTH;
                if (brick < BRICKS_TOTAL && brick >= 0 && !bricks[brick]) {
                    bricks[brick] = true;
                    broken_bricks++;
                    ball.direction_vertical = -1;
                }
            } else if (ball.direction_vertical == -1) {
                int brick = -(ball.y - 1 - BRICKS_HEIGHT) * BRICKS_PER_ROW + (ball.x - 2) / BRICK_WIDTH;
                if (brick < BRICKS_TOTAL && brick >= 0 && !bricks[brick]) {
                    bricks[brick] = true;
                    broken_bricks++;
                    ball.direction_vertical = 1;
                }
            } else if (ball.direction_horizontal == 1) {
                int brick = -(ball.y + BRICKS_HEIGHT) * BRICKS_PER_ROW + (ball.x - 1) / BRICK_WIDTH;
                if (brick < BRICKS_TOTAL && brick >= 0 && !bricks[brick]) {
                    bricks[brick] = true;
                    broken_bricks++;
                    ball.direction_horizontal = -1;
                }
            } else if (ball.direction_horizontal == -1) {
                int brick = -(ball.y + BRICKS_HEIGHT) * BRICKS_PER_ROW + (ball.x - 3) / BRICK_WIDTH;
                if (brick < BRICKS_TOTAL && brick >= 0 && !bricks[brick]) {
                    bricks[brick] = true;
                    broken_bricks++;
                    ball.direction_horizontal = 1;
                }
            } else {
                int brick = -(ball.y + BRICKS_HEIGHT) * BRICKS_PER_ROW + (ball.x - 2) / BRICK_WIDTH;
                if (brick < BRICKS_TOTAL && brick >= 0 && !bricks[brick]) {
                    bricks[brick] = true;
                    broken_bricks++;
                    ball.direction_horizontal *= -1;
                }
            }
            if (broken_bricks == BRICKS_TOTAL) {
                game_over = true;
                player_won = true;
            }
        }

        switch (key_pressed) {
            case 'q':
                goto end;
            case KEY_LEFT:
                if (!game_over) {
                    player.x -= 2;
                    if (player.x <= 1) {
                        player.x = 1;
                    }
                }
                break;
            case KEY_RIGHT:
                if (!game_over) {
                    player.x += 2;
                    if (player.x >= WINDOW_WIDTH - PLAYER_WIDTH - 1) {
                        player.x = WINDOW_WIDTH - PLAYER_WIDTH - 1;
                    }
                }
                break;
            case 10:
                if (game_over) {
                    player.x = (WINDOW_WIDTH - PLAYER_WIDTH) / 2;
                    player.y = PLAYER_HEIGHT;
                    ball.x = 5;
                    ball.y = 5;
                    ball.direction_horizontal = 1;
                    ball.direction_vertical = 1;

                    memset(bricks, 0, sizeof(bricks));

                    broken_bricks = 0;

                    game_over = false;
                    player_won = false;
                }
                break;
            default:
                break;
        }

        wrefresh(window);
        refresh();
        wgetch(window);
        napms(100);
        key_pressed = wgetch(window);

        werase(window);
    }
    end:

    endwin();
    return 0;
}
#pragma clang diagnostic pop