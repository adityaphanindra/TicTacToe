#include "grid.h"

#include <cassert>
#include <cstdint>
#include <vector>

#include "constants.h"

Grid::Grid() {
	reset();
}

bool Grid::operator==(const Grid & other) const {
	for (int8_t row = 0; row < NUM_ROWS; ++row) {
		for (int8_t col = 0; col < NUM_COLS; ++col) {
			if (_grid[row][col] != other._grid[row][col]) {
				return false;
			}
		}
	}
	return true;	
}

void Grid::reset() {
	for (int8_t row = 0; row < NUM_ROWS; ++row) {
		for (int8_t col = 0; col < NUM_COLS; ++col) {
			_grid[row][col] = Move::EMPTY;
		}
	}
}

Move Grid::value(int8_t row, int8_t col) const {
	assert(row >= 0 && row < NUM_ROWS && col >= 0 && col < NUM_COLS);

	return _grid[row][col];
}

bool Grid::set_value(int8_t row, int8_t col) {
	assert(row >= 0 && row < NUM_ROWS && col >= 0 && col < NUM_COLS);

	if (_has_game_ended()) {
		printf("Grid::set_value(): Warning! Game has ended.\n");
		return false;
	}

	if (_grid[row][col] != Move::EMPTY) {
		printf("Grid::set_value(): Warning! Cell (%d, %d) is not empty.\n", row, col);
		return false;		
	}
	_grid[row][col] = next_player();
	return true;
}

bool Grid::set_value(int8_t row, int8_t col, Move move) {
	assert(row >= 0 && row < NUM_ROWS && col >= 0 && col < NUM_COLS);

	_grid[row][col] = move;
	return true;
}

std::vector<MovePosition> Grid::valid_move_positions() const {
	std::vector<MovePosition> valid_positions;

	for (int8_t row = 0; row < NUM_ROWS; ++row) {
		for (int8_t col = 0; col < NUM_COLS; ++col) {
			if (_grid[row][col] == Move::EMPTY) {
				valid_positions.push_back(std::make_pair(row, col));
			}
		}
	}
	return valid_positions;
}

Move Grid::next_player() const {
	if (_has_game_ended()) {
		// Game Ended
		return Move::EMPTY;
	}
	int8_t num_noughts = 0;
	int8_t num_crosses = 0;
	for (int8_t row = 0; row < NUM_ROWS; ++row) {
		for (int8_t col = 0; col < NUM_COLS; ++col) {
			switch (_grid[row][col]) {
				case Move::NOUGHT:
					++num_noughts;
					break;
				case Move::CROSS:
					++num_crosses;
					break;
				case Move::EMPTY:
				default:
					break;
			}
		}
	}
	if (num_crosses + num_noughts >= NUM_ROWS * NUM_COLS) {
		// Game Ended
		return Move::EMPTY;
	}
	if (num_crosses > num_noughts) {
		return Move::NOUGHT;
	} else if (num_crosses > num_noughts) {
		return Move::CROSS;
	} else {
		return FIRST_PLAYER_MOVE;
	}
	return Move::EMPTY;
}

GameState Grid::game_state() const {
	return _game_state();
}

bool Grid::has_game_ended() const {
	return _has_game_ended();
}

bool Grid::can_win_in_one_move(MovePosition & position) const {
	position = std::make_pair(NUM_ROWS, NUM_COLS);

	size_t rank = this->rank();
	if (rank < 3) {
		return false;
	}

	for (int8_t row = 0; row < NUM_ROWS; ++row) {
		for (int8_t col = 0; col < NUM_COLS; ++col) {
			if (_grid[row][col] != Move::EMPTY) {
				continue;
			}
			Grid grid_after_move = *this;
			bool success = grid_after_move.set_value(row, col);
			if (!success) {
				continue;
			}
			if (grid_after_move._has_game_ended()) {
				position = std::make_pair(row, col);
				return true;
			}
		}
	}
	return false;
}

std::vector<MovePosition> Grid::winning_moves() const {
	std::vector<MovePosition> winning_moves;
	switch (_game_state()) {
		case GameState::ONGOING:
		case GameState::INVALID:
		case GameState::DRAW:
		default:
			return winning_moves;
		case GameState::NOUGHT_WINS:
		case GameState::CROSS_WINS:
			if (_get_completed_row(winning_moves)) {
				return winning_moves;				
			} else if (_get_completed_col(winning_moves)) {
				return winning_moves;				
			} else if (_get_completed_diag(winning_moves)) {
				return winning_moves;				
			}
			winning_moves.clear();
			return winning_moves;
	}
}

size_t Grid::rank() const {
    size_t rank = 0;
    for (size_t row = 0; row < NUM_ROWS; ++row) {
        for (size_t col = 0; col < NUM_COLS; ++col) {
            if (_grid[row][col] == Move::EMPTY) {
                continue;
            }
            ++rank;
        }
    }
    return rank;
}

bool Grid::_has_game_ended() const {
	switch (_game_state()) {
		case GameState::ONGOING:
		case GameState::INVALID:
		default:
			return false;
		case GameState::NOUGHT_WINS:
		case GameState::CROSS_WINS:
		case GameState::DRAW:
			return true;
	}
}

GameState Grid::_game_state() const {
	int16_t row_sums[NUM_ROWS];
	int16_t col_sums[NUM_COLS];
	int16_t diag_sums[NUM_DIAGS];
	int16_t empty_count = 0;

	for (int8_t row = 0; row < NUM_ROWS; ++row) {
		row_sums[row] = 0;
	}
	for (int8_t col = 0; col < NUM_COLS; ++col) {
		col_sums[col] = 0;
	}
	for (int8_t diag = 0; diag < NUM_DIAGS; ++diag) {
		diag_sums[diag] = 0;
	}

	for (int8_t row = 0; row < NUM_ROWS; ++row) {
		for (int8_t col = 0; col < NUM_COLS; ++col) {
			int8_t score = 0;
			switch(_grid[row][col]) {
				case Move::CROSS:
					score = 1;
					break;
				case Move::NOUGHT:
					score = -1;
					break;
				default:
					empty_count += 1;
					break;
			}
			row_sums[row] += score;
			col_sums[col] += score;
			if (row == col) {
				diag_sums[0] += score;
			}
			if (row + col + 1 == NUM_COLS) {
				diag_sums[1] += score;
			}
		}
	}

	for (int8_t row = 0; row < NUM_ROWS; ++row) {
		if (row_sums[row] == NUM_COLS) {
			return GameState::CROSS_WINS;
		} else if (row_sums[row] == -NUM_COLS) {
			return GameState::NOUGHT_WINS;
		}
	}

	for (int8_t col = 0; col < NUM_COLS; ++col) {
		if (col_sums[col] == NUM_ROWS) {
			return GameState::CROSS_WINS;
		} else if (col_sums[col] == -NUM_ROWS) {
			return GameState::NOUGHT_WINS;
		}
	}

	for (int8_t diag = 0; diag < NUM_DIAGS; ++diag) {
		if (diag_sums[diag] == std::min(NUM_ROWS, NUM_COLS)) {
			return GameState::CROSS_WINS;
		} else if (diag_sums[diag] == -std::min(NUM_ROWS, NUM_COLS)) {
			return GameState::NOUGHT_WINS;
		}
	}

	if (empty_count == 0) {
		return GameState::DRAW;
	}

	return GameState::ONGOING;
}

void Grid::print_grid() const {
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            printf("%s ", STR_MOVE_SYMBOL(_grid[row][col]));
        }
        printf("\n");
    }
    printf("\n");
}

bool Grid::_get_completed_row(std::vector<MovePosition> & winning_moves) const {
	winning_moves.clear();

	for (int8_t row = 0; row < NUM_ROWS; ++row) {
		winning_moves.push_back(std::make_pair(row, 0));

		for (int8_t col = 1; col < NUM_COLS; ++col) {
			if (_grid[row][col - 1] != _grid[row][col]) {
				winning_moves.clear();
				break;
			}
			winning_moves.push_back(std::make_pair(row, col));
		}

		if (!winning_moves.empty()) {
			return true;
		}
	}
	return false;
}

bool Grid::_get_completed_col(std::vector<MovePosition> & winning_moves) const {
	winning_moves.clear();

	for (int8_t col = 0; col < NUM_COLS; ++col) {
		winning_moves.push_back(std::make_pair(0, col));

		for (int8_t row = 1; row < NUM_ROWS; ++row) {
			if (_grid[row - 1][col] != _grid[row][col]) {
				winning_moves.clear();
				break;
			}
			winning_moves.push_back(std::make_pair(row, col));
		}

		if (!winning_moves.empty()) {
			return true;
		}
	}
	return false;
}

bool Grid::_get_completed_diag(std::vector<MovePosition> & winning_moves) const {
	winning_moves.clear();
	if (NUM_ROWS != NUM_COLS) {
		return false;
	}

	winning_moves.push_back(std::make_pair(0, 0));
	for (int8_t row = 1; row < NUM_ROWS; ++row) {
		if (_grid[row - 1][row - 1] != _grid[row][row]) {
			winning_moves.clear();
			break;
		}
		winning_moves.push_back(std::make_pair(row, row));
	}

	if (!winning_moves.empty()) {
		return true;
	}

	winning_moves.push_back(std::make_pair(0, NUM_COLS - 1));
	for (int8_t row = 1; row < NUM_ROWS; ++row) {
		if (_grid[row - 1][NUM_COLS - row] != _grid[row][NUM_COLS - 1 - row]) {
			winning_moves.clear();
			break;
		}
		winning_moves.push_back(std::make_pair(row, NUM_COLS - 1 - row));
	}

	if (!winning_moves.empty()) {
		return true;
	}
	return false;
}