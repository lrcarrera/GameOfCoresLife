/*
 * Defines types used for Conway's Game of Life.
 *
 * @author: Johnathan Davis
 */

#ifndef LIFE_TYPES_H
#define LIFE_TYPES_H

#include <stdint.h>

#include "SDL.h"
#include "vars.h"


typedef uint8_t Cell; /* 0 = dead; 1 = alive */

typedef Cell (*Board)[BOARD_W_MAX];

typedef SDL_Rect (*BoardRect)[BOARD_W_MAX];

typedef uint8_t Color;

#endif /* LIFE_TYPES_H */

