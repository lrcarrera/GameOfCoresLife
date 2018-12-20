/*
 * Conway's Game of Life.
 *
 * An excercise in SDL and C.
 *
 * @author: Johnathan Davis
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDL.h"
#include "actions.h"
#include "macros.h"
#include "render.h"
#include "types.h"


/* Use two boards to maintain a backbuffer. */
static Board board_active;
static Board board_backbuffer;

/* Track clicks */
static Board board_clicks;

/* SDL's perspective of board. */
static BoardRect board_rects;

static int32_t tick_interval = TICK_RATE_INITIAL_MS;

static uint32_t board_w = BOARD_W_INIT;
static uint32_t board_h = BOARD_H_INIT;

int
main(int argc, char *argv[])
{
    int iterations=0;
    /* Initialize SDL. */
    SDL_Window *win = NULL;
    SDL_Renderer *ren = NULL;
    if (sdl_init(WINDOW_TITLE, WINDOW_W_INIT, WINDOW_H_INIT, &win, &ren))
        return 1;

    srand(time(NULL));

    /* Initialize the board. */
    board_active = calloc(BOARD_W_MAX * BOARD_H_MAX,
                          sizeof(**board_active));
    board_backbuffer = calloc(BOARD_W_MAX * BOARD_H_MAX,
                              sizeof(**board_backbuffer));
    board_clicks = calloc(BOARD_W_MAX * BOARD_H_MAX, sizeof(**board_clicks));
    board_rects = calloc(BOARD_W_MAX * BOARD_H_MAX, sizeof(**board_rects));
    populate_board(board_h, board_w, board_active);
    init_board_rects(board_rects);

    life_write("./LifeGameInit", board_h, board_w, board_active, iterations);

    /* Track the last time the game state advanced. */
    int32_t ticks_last_step = 0;

    int run = 1; /* keep at 1 to keep running the game at tick_interval */
    int quit = 0; /* set to 1 to exit the game loop */
    while (!quit) {
        int32_t ticks_start = SDL_GetTicks();

        /* Unless changed by resizing, use the current board dimensions for the
         * next iteration. */
        uint32_t board_w_next = board_w;
        uint32_t board_h_next = board_h;

        int step = 0; /* set to 1 if the game should advance 1 tick */
        int restart = 0; /* set to 1 if the board should be reset */
        int clear = 0; /* set to 1 if the board should be cleared */
        int clicked = 0; /* set to 1 if a click happens */
        zero_board(board_h, board_w, board_clicks);

        /*
         * Handle events.
         */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_q) {
                        life_write("./LifeGameEnd", board_h, board_w, board_active, iterations);
                        quit = 1;
                    } else if (e.key.keysym.sym == SDLK_SPACE) {
                        run = !run;
                    } else if (e.key.keysym.sym == SDLK_s) {
                        run = 0;
                        step = 1;
                    } else if (e.key.keysym.sym == SDLK_LEFTBRACKET) {
                        tick_interval += TICK_RATE_STEP;
                        if (tick_interval > INT_MAX - TICK_RATE_STEP)
                            tick_interval = INT_MAX - TICK_RATE_STEP;
                    } else if (e.key.keysym.sym == SDLK_RIGHTBRACKET) {
                        tick_interval -= TICK_RATE_STEP;
                        if (tick_interval < 0)
                            tick_interval = 0;
                    } else if (e.key.keysym.sym == SDLK_r) {
                        restart = 1;
                        run = 1;
                        tick_interval = TICK_RATE_INITIAL_MS;
                    } else if (e.key.keysym.sym == SDLK_c) {
                        clear = 1;
                        run = 0;
                        step = 1;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        int32_t x = e.button.x;
                        int32_t y = e.button.y;
                        Cell *cell = get_cell_by_coord(board_h, board_w, x, y,
                                                       board_clicks);

                        /* Flip this cell's clicked/unclicked state. */
                        *cell = !*cell;
                        clicked = 1;
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        uint32_t win_w_next = e.window.data1;
                        uint32_t win_h_next = e.window.data2;
                        board_w_next = px_to_cell_length(win_w_next);
                        if (board_w_next > BOARD_W_MAX)
                            board_w_next = BOARD_W_MAX;
                        board_h_next = px_to_cell_length(win_h_next);
                        if (board_h_next > BOARD_H_MAX)
                            board_h_next = BOARD_H_MAX;
                    }
                    break;
                default:
                    break;
            }
        }

        if (board_h_next < board_h || board_w_next < board_w) {
            /* Clear cropped regions of board. */
            zero_board_region(board_h, board_w, board_h_next, board_w_next,
                              board_active);
            zero_board_region(board_h, board_w, board_h_next, board_w_next,
                              board_backbuffer);
            zero_board_region(board_h, board_w, board_h_next, board_w_next,
                              board_clicks);
        }

        /*
         * Game logic.
         */
        int user_provoked_tick = (step || restart || clear);
        int next_tick_due = (SDL_GetTicks() - ticks_last_step >= tick_interval);
        if (user_provoked_tick || next_tick_due) {
            if (restart)
                populate_board(board_h_next, board_w_next, board_active);
            else if (clear)
                zero_board(board_h_next, board_w_next, board_active);

            if (run || step) {
                /* Write the updated life/death statuses to the backbuffer. */
                advance_all_cells(board_h_next, board_w_next,
                                  board_active, board_backbuffer);

                /* Make the backbuffer active. */
                swap(Board, board_active, board_backbuffer);
                iterations++;
                if (!(iterations%1))
                    life_write("./LifeGame", board_h, board_w, board_active, iterations);
            }

            ticks_last_step = SDL_GetTicks();
        }

        if (clicked) {
            toggle_cells_from_clicks(board_h_next, board_w_next,
                                     board_clicks, board_active);
        }

        /*
         * Render.
         */
        render_blank(ren);

        render_cells(ren, board_h_next, board_w_next,
                     board_rects, board_active);

        SDL_RenderPresent(ren);

        /* Update current board dimensions. */
        board_h = board_h_next;
        board_w = board_w_next;

        /* Cap the frame rate. */
        int32_t ticks_ellapsed = SDL_GetTicks() - ticks_start;
        if (ticks_ellapsed < TICKS_PER_FRAME)
            SDL_Delay(TICKS_PER_FRAME - ticks_ellapsed);
    }

    printf("Exiting...\n");
    sdl_teardown(win, ren, NULL);
    return 0;
}

