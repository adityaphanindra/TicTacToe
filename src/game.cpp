#include "game.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <string>

Game::Game() :
    _current_game_state(GameState::ONGOING),
    _next_player(Move::EMPTY)
{
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            _grid[row][col] = Move::EMPTY;
        }
    }
}

void Game::reset()
{
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            _grid[row][col] = Move::EMPTY;
        }
    }
    _current_game_state = GameState::ONGOING;
    _game_status_string = "";
}

bool Game::play_nought(int8_t row_index, int8_t col_index)
{
    return _play(row_index, col_index, Move::NOUGHT);
}

bool Game::play_cross(int8_t row_index, int8_t col_index)
{
    return _play(row_index, col_index, Move::CROSS);
}

GameState Game::get_game_state()
{
    _draw_game();
    return _current_game_state;
}

std::string Game::get_game_status_string()
{
    return _game_status_string;
}

bool Game::_play(int8_t row_index, int8_t col_index, Move next_move)
{
    assert(row_index >= 0 && row_index < NUM_ROWS);
    assert(col_index >= 0 && col_index < NUM_COLS);

    if (_current_game_state != GameState::ONGOING) {
        printf("\nGame::_play(): Warning: Game Over. Result is %s", STR_GAME_STATE(_current_game_state));
        return false;
    }
    if (_grid[row_index][col_index] != Move::EMPTY) {
        printf("\nGame::_play(): Invalid move (%s) on cell (%d, %d) with current state (%s).",
               STR_MOVE(next_move), row_index, col_index, STR_MOVE(_grid[row_index][col_index]));
        return false;
    }

    switch (next_move) {
    case Move::CROSS: {
        if (_next_player == Move::CROSS || _next_player == Move::EMPTY) {
            _next_player = Move::NOUGHT;
        } else {
            printf("\nGame::_play(): Invalid move (%s). Next player is %s.",
                   STR_MOVE(next_move), STR_MOVE(_next_player));
            return false;
        }
        break;
    }
    case Move::NOUGHT: {
        if (_next_player == Move::NOUGHT || _next_player == Move::EMPTY) {
            _next_player = Move::CROSS;
        } else {
            printf("\nGame::_play(): Invalid move (%s). Next player is %s.",
                   STR_MOVE(next_move), STR_MOVE(_next_player));
            return false;
        }
        break;
    }
    default:
        printf("\nGame::_play(): Warning: Trying to empty the cell (%d, %d).", row_index, col_index);
        return false;
    }

    GameState previous_game_state = _current_game_state;

    _grid[row_index][col_index] = next_move;

    _check_game();

    if (previous_game_state != _current_game_state) {
        _game_status_string = STR_GAME_STATE(_current_game_state);
    }

    char status[128];
    snprintf(status, 128, "%s on (%d, %d)", STR_MOVE(next_move), row_index, col_index);
    _game_status_string = status;

    return true;
}

void Game::_check_game()
{
    if (_current_game_state != GameState::ONGOING) {
        return;
    }
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
            case Move::CROSS: {
                score = 1;
                break;
            }
            case Move::NOUGHT: {
                score = -1;
                break;
            }
            default: {
                empty_count += 1;
                break;
            }
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
            _current_game_state = GameState::CROSS_WINS;
            return;
        } else if (row_sums[row] == -NUM_COLS) {
            _current_game_state = GameState::NOUGHT_WINS;
            return;
        }
    }
    for (int8_t col = 0; col < NUM_COLS; ++col) {
        if (col_sums[col] == NUM_ROWS) {
            _current_game_state = GameState::CROSS_WINS;
            return;
        } else if (col_sums[col] == -NUM_ROWS) {
            _current_game_state = GameState::NOUGHT_WINS;
            return;
        }
    }
    for (int8_t diag = 0; diag < NUM_DIAGS; ++diag) {
        if (diag_sums[diag] == std::min(NUM_ROWS, NUM_COLS)) {
            _current_game_state = GameState::CROSS_WINS;
            return;
        } else if (diag_sums[diag] == -std::min(NUM_ROWS, NUM_COLS)) {
            _current_game_state = GameState::NOUGHT_WINS;
            return;
        }
    }
    if (_current_game_state == GameState::ONGOING && empty_count == 0) {
        _current_game_state = GameState::DRAW;
    }
}

void Game::_draw_game() {
    printf("\n");
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        printf("\n");
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            printf("%s ", STR_MOVE_SYMBOL(_grid[row][col]));
        }
    }
    printf("\n");
}
