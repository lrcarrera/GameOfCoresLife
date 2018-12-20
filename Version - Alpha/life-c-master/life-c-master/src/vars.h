/*
 * Global settings for Conway's Game of Life.
 *
 * @author: Johnathan Davis
 */

#ifndef LIFE_VARS_H
#define LIFE_VARS_H

#include "macros.h"


#define WINDOW_TITLE "Conway's Game of Life (Q to quit)"
/*
#define BOARD_W_INIT 140
#define BOARD_H_INIT 80
*/
#define BOARD_W_INIT 10
#define BOARD_H_INIT 10

#define BOARD_W_MAX 2000
#define BOARD_H_MAX 2000
#define CELL_SIZE 5
#define GRID_SIZE 1
#define COLOR_GRID {0, 0, 0}
/*
#define COLOR_ALIVE_A {255, 20, 0}
#define COLOR_ALIVE_B {255, 170, 0}
*/
#define COLOR_ALIVE_A {66, 244, 155}
#define COLOR_ALIVE_B {4, 0, 255}
#define COLOR_DEAD {20, 20, 20}
#define WINDOW_W_INIT cell_length_to_px(BOARD_W_INIT)
#define WINDOW_H_INIT cell_length_to_px(BOARD_H_INIT)
#define ALIVE 1
#define DEAD 0
#define LUCK_LIFE_START 15 /* out of 100 */
#define FPS_MAX 60
#define TICKS_PER_FRAME 1000.0 / FPS_MAX
#define TICK_RATE_INITIAL_MS 30
#define TICK_RATE_STEP 10
#define FPS_SAMPLE_COUNT 10

#endif /* LIFE_VARS_H */

