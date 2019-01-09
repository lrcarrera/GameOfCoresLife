/*
 * Defines some game actions for Conway's Game of Life.
 *
 * @author: Johnathan Davis
 */

#ifndef LIFE_ACTIONS_H
#define LIFE_ACTIONS_H

#include <stdint.h>

#include "types.h"


void 
life_write(char *output_filename, int32_t board_h, int32_t board_w, Board board, int iterations);

void
populate_board(int32_t board_h, int32_t board_w, Board board);

void
zero_board(int32_t board_h, int32_t board_w, Board board);

void
zero_board_region(int32_t board_h1, int32_t board_w1,
                  int32_t board_h2, int32_t board_w2,
                  Board board);

void
init_board_rects(BoardRect board_rects);

size_t
get_neighbor_count(int32_t board_h, int32_t board_w, int32_t row, int32_t col,
                   Board board);

Cell *
get_cell_by_coord(int32_t board_h, int32_t board_w, int32_t x, int32_t y,
                  Board board);

void
advance_all_cells(int32_t board_h, int32_t board_w,
                  Board board_in, Board board_out);

void
advance_cell(int32_t board_h, int32_t board_w, int32_t row, int32_t col,
             Board board_in, Board board_out);

void
toggle_cells_from_clicks(int32_t board_h, int32_t board_w,
                         Board board_clicks, Board board);

#endif /* LIFE_ACTIONS_H */

