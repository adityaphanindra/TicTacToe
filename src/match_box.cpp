#include "match_box.h"

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>

MatchBox::MatchBox() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

MatchBox::MatchBox(const Grid grid) : _grid(grid) {
    for (size_t row = 0; row < NUM_ROWS; ++row) {
        for (size_t col = 0; col < NUM_COLS; ++col) {
            _remaining_seeds[row][col] = 0;
            if (_grid.value(row, col) == Move::EMPTY) {
                _remaining_seeds[row][col] = MAX_NUM_SEEDS;
            }
        }
    }

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

MatchBox::MatchBox(const MatchBox & other) {
    _grid = other._grid;
    for (size_t row = 0; row < NUM_ROWS; ++row) {
        for (size_t col = 0; col < NUM_COLS; ++col) {
            _remaining_seeds[row][col] = other._remaining_seeds[row][col];
        }
    }
}

void MatchBox::operator=(const MatchBox &other) {
    _grid = other._grid;
    for (size_t row = 0; row < NUM_ROWS; ++row) {
        for (size_t col = 0; col < NUM_COLS; ++col) {
            _remaining_seeds[row][col] = other._remaining_seeds[row][col];
        }
    }
}

MovePosition MatchBox::pick_random_move() {
    printf("MatchBox::pick_random_move(): MATCHBOX GRID:\n");
    _grid.print_grid();
    _print_remaining_seeds();

    int32_t total_remaining_seeds = 0;
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            total_remaining_seeds += _remaining_seeds[row][col];
        }
    }
    if (total_remaining_seeds <= 0) {
        return std::make_pair(NUM_ROWS, NUM_COLS);
    }

    int16_t random_index = static_cast<int16_t>(std::rand() % total_remaining_seeds);
    int16_t remaining_index = 0;
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            if (remaining_index + _remaining_seeds[row][col] > random_index) {
                printf("Matchbox::pick_random_move(): picked (%d, %d)\n", row, col);
                return std::make_pair(row, col);
            }
            remaining_index += _remaining_seeds[row][col];
        }
    }
    return std::make_pair(NUM_ROWS, NUM_COLS);
}

Grid MatchBox::get_grid() const {
    return _grid;
}

void MatchBox::reward_drawn_move(MovePosition move) {
    size_t row = move.first, col = move.second;
    assert(row < NUM_ROWS && col < NUM_COLS);
    assert (_grid.value(row, col) == Move::EMPTY);
    _remaining_seeds[row][col] += 1;
}

void MatchBox::reward_move(MovePosition move) {
    size_t row = move.first, col = move.second;
    assert(row < NUM_ROWS && col < NUM_COLS);
    assert (_grid.value(row, col) == Move::EMPTY);
    _remaining_seeds[row][col] += 3;
}

void MatchBox::punish_move(MovePosition move) {
    size_t row = move.first, col = move.second;
    assert (row < NUM_ROWS && col < NUM_COLS);
    assert (_grid.value(row, col) == Move::EMPTY);
    if (_remaining_seeds[row][col] > 1) {
        _remaining_seeds[row][col] -= 1;
    }
}

void MatchBox::_print_remaining_seeds() const {
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            printf("%d ", _remaining_seeds[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}