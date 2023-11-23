#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "platform.h"
#include "mine_state.h"
#include "build_settings.h"
#include "game_state.h"
#include "math.h"

mine_state board[BOARD_SIZE][BOARD_SIZE] = { { MS_FREE } };
game_state state = GS_PLAYING;
int has_clicked;

int rand_range(int x, int y)
{
    return x + rand() % (y - x);
}

int get_neighbour_mines(int x, int y) {
    int mines = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if(i == 0 && j == 0)
                continue;

            if(x + i < 0 || x + i >= BOARD_SIZE)
                continue;
            if(y + j < 0 || y + j >= BOARD_SIZE)
                continue;

            int cx = x + i;
            int cy = y + j;

            mine_state mine_state = board[cx][cy];

            if(mine_state == MS_MINE || mine_state == MS_FLAGGED_POSITIVE || mine_state == MS_CHECKED_MINE)
                mines++;
        }
    }
    return mines;
}

void first_click_relocations(int x, int y) {
    printf("Performing relocations!\n");
    if(board[x][y] == MS_MINE) {
        printf("Click would've been a mine!\n");
        board[x][y] = MS_FREE;
        while (1) {
            int mx = rand_range(0, BOARD_SIZE);
            int my = rand_range(0, BOARD_SIZE);

            if(board[mx][my] != MS_MINE && mx != x && my != y) {
                printf("Relocation from %i, %i to %i, %i\n", x, y, mx, my);
                board[mx][my] = MS_MINE;
                break;
            }
        }
    }

    if(get_neighbour_mines(x, y) != 0) {
        // If there's a neighbouring mine on the first click, relocate 'em all.
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if(i == 0 && j == 0)
                    continue;

                if(x + i < 0 || x + i >= BOARD_SIZE)
                    continue;
                if(y + j < 0 || y + j >= BOARD_SIZE)
                    continue;

                int cx = x + i;
                int cy = y + j;

                if(board[cx][cy] != MS_MINE)
                    continue;
                board[cx][cy] = MS_FREE;

                while(1) {
                    int mx = rand_range(0, BOARD_SIZE);
                    int my = rand_range(0, BOARD_SIZE);

                    if(board[mx][my] != MS_MINE && mx != x && my != y && mx != cx && my != cy) {
                        board[mx][my] = MS_MINE;
                        break;
                    }
                }
            }
        }
    }
}

void generate_field() {

    for (int x = 0; x < BOARD_SIZE; ++x) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            board[x][y] = MS_FREE;
        }
    }

    int mines = 0;
    while (mines < FIELD_MINES) {
        int x = rand_range(0, BOARD_SIZE);
        int y = rand_range(0, BOARD_SIZE);

        if(board[x][y] != MS_MINE) {
            mines++;
            board[x][y] = MS_MINE;
        }
    }
}

void clear_neighbours(int x, int y) {

    if(get_neighbour_mines(x, y))
        return;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if(i == 0 && j == 0)
                continue;

            if(x + i < 0 || x + i >= BOARD_SIZE)
                continue;
            if(y + j < 0 || y + j >= BOARD_SIZE)
                continue;

            int cx = x + i;
            int cy = y + j;

            if(board[cx][cy] == MS_FREE) {
                board[cx][cy] = MS_CHECKED_FREE;
                if(get_neighbour_mines(cx, cy) == 0)
                    clear_neighbours(cx, cy);
            }
        }
    }
}

int main() {

    srand(time(NULL));
    platform_init();

    texture_t unchecked = platform_load_texture("assets/tile_unchecked.png");
    texture_t flagged = platform_load_texture("assets/flagged.png");
    texture_t mine = platform_load_texture("assets/mine.png");
    texture_t flagged_false = platform_load_texture("assets/flagged-false.png");

    texture_t checked[9] = {
            platform_load_texture("assets/checked/0.png"),
            platform_load_texture("assets/checked/1.png"),
            platform_load_texture("assets/checked/2.png"),
            platform_load_texture("assets/checked/3.png"),
            platform_load_texture("assets/checked/4.png"),
            platform_load_texture("assets/checked/5.png"),
            platform_load_texture("assets/checked/6.png"),
            platform_load_texture("assets/checked/7.png"),
            platform_load_texture("assets/checked/8.png")
    };

    generate_field();

    while(!render_should_close()) {
        platform_begin_frame();

        for (int x = 0; x < BOARD_SIZE; ++x) {
            for (int y = 0; y < BOARD_SIZE; ++y) {

                texture_t texture = NULL;

                mine_state mine_state = board[x][y];

                if(mine_state == MS_FREE)
                    texture = unchecked;
                else if(mine_state == MS_MINE && state != GS_LOST)
                    texture = unchecked;
                else if(mine_state == MS_MINE && state == GS_LOST)
                    texture = mine;
                else if(mine_state == MS_CHECKED_FREE)
                    texture = checked[get_neighbour_mines(x, y)];
                else if(mine_state == MS_CHECKED_MINE)
                    texture = mine;
                else if(mine_state == MS_FLAGGED_POSITIVE)
                    texture = flagged;
                else if(mine_state == MS_FLAGGED_NEGATIVE && state != GS_LOST)
                    texture = flagged;
                else if(mine_state == MS_FLAGGED_NEGATIVE && state == GS_LOST)
                    texture = flagged_false;

                if(texture != NULL)
                   platform_draw_texture(texture, x * TILE_SIZE, y * TILE_SIZE);
            }
        }

        int cursor_x = (int)floorf(platform_cursor_x() / TILE_SIZE);
        int cursor_y = (int)floorf(platform_cursor_y() / TILE_SIZE);

        if(state == GS_PLAYING) {
            if (platform_mb_left()) {
                if (!has_clicked) {
                    first_click_relocations(cursor_x, cursor_y);
                    has_clicked = 1;
                }
                mine_state mine_state = board[cursor_x][cursor_y];
                if (mine_state == MS_FREE) {
                    board[cursor_x][cursor_y] = MS_CHECKED_FREE;
                    clear_neighbours(cursor_x, cursor_y);
                } else if (mine_state == MS_MINE) {
                    board[cursor_x][cursor_y] = MS_CHECKED_MINE;
                    state = GS_LOST;
                } else if (mine_state == MS_FLAGGED_POSITIVE) {
                    board[cursor_x][cursor_y] = MS_MINE;
                } else if (mine_state == MS_FLAGGED_NEGATIVE) {
                    board[cursor_x][cursor_y] = MS_FREE;
                }
            }

            if (platform_mb_right()) {
                if (board[cursor_x][cursor_y] == MS_MINE) {
                    board[cursor_x][cursor_y] = MS_FLAGGED_POSITIVE;
                }
                if (board[cursor_x][cursor_y] == MS_FREE) {
                    board[cursor_x][cursor_y] = MS_FLAGGED_NEGATIVE;
                }
            }
        }
        else if(state == GS_LOST) {
            if(platform_mb_left()) {
                generate_field();
                has_clicked = 0;
                state = GS_PLAYING;
            }
        }

        platform_end_frame();
    }

    platform_close();

}
