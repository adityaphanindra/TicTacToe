#ifndef GRID_H
#define GRID_H

#include <cstdint>
#include <vector>

#include "constants.h"

class Grid
{
public:
	Grid();
    bool operator==(const Grid & other) const;
	void reset();
	Move value(int8_t row, int8_t col) const;
	bool set_value(int8_t row, int8_t col);
	bool set_value(int8_t row, int8_t col, Move move);
	std::vector<MovePosition> valid_move_positions() const;
	Move next_player() const;
	GameState game_state() const;
	bool has_game_ended() const;
	bool can_win_in_one_move(MovePosition & position) const;
    std::vector<MovePosition> winning_moves() const;
    size_t rank() const;
    void print_grid() const;
private:
	bool _has_game_ended() const;
	GameState _game_state() const;
	bool _get_completed_row(std::vector<MovePosition> & winning_moves) const;
	bool _get_completed_col(std::vector<MovePosition> & winning_moves) const;
	bool _get_completed_diag(std::vector<MovePosition> & winning_moves) const;

	Move _grid[NUM_ROWS][NUM_COLS];
};

#endif // GRID_H