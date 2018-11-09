#include "game.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <string>
#include <tuple>

#include "game_bot.h"

Game::Game() {
    reset();
}

void Game::reset() {
    _grid.reset();
    _game_status_string = "";
}

bool Game::play_next(int8_t row_index, int8_t col_index, Move & next_move) {
    printf("\nGame::play_next(): MOVE #%lu ~~~~~~~~~~~~~~~~~~~~~\n", _grid.rank() + 1);
    _grid.print_grid();

    next_move = _grid.next_player();
    if (next_move != PLAYER_MOVE) {
        return false;
    }
    return _play(row_index, col_index);
}

bool Game::play_bot(int8_t & row_index, int8_t & col_index, Move & next_move) {
    printf("\nGame::play_bot(): MOVE #%lu ~~~~~~~~~~~~~~~~~~~~~\n", _grid.rank() + 1);
    _grid.print_grid();

    next_move = _grid.next_player();
    if (next_move != BOT_MOVE) {
        return false;
    }
    return _play_bot(row_index, col_index);
}

GameState Game::get_game_state() {
    return _grid.game_state();
}

std::string Game::get_game_status_string() {
    return _game_status_string;
}

std::vector<MovePosition> Game::get_winning_moves() {
    return _grid.winning_moves();
}

bool Game::_play(int8_t row_index, int8_t col_index) {
    Move next_player = _grid.next_player();
    GameState current_game_state = _grid.game_state();

    assert(row_index >= 0 && row_index < NUM_ROWS);
    assert(col_index >= 0 && col_index < NUM_COLS);
    assert(next_player == Move::NOUGHT or next_player == Move::CROSS);

    if (current_game_state != GameState::ONGOING) {
        printf("Game::_play(): Warning: Game Over. Result is %s\n", STR_GAME_STATE(current_game_state));
        return false;
    }

    GameState previous_game_state = current_game_state;

    bool success = _grid.set_value(row_index, col_index);
    if (!success) {
        printf("\nGame::_play(): Warning: Cannot play %s on (%d, %d)", STR_MOVE(next_player), row_index, col_index);
        return false;
    }

    current_game_state = _grid.game_state();

    if (previous_game_state != current_game_state) {
        _game_status_string = STR_GAME_STATE(current_game_state);
    }

    char status[128];
    snprintf(status, 128, "%s on (%d, %d)", STR_MOVE(next_player), row_index, col_index);
    _game_status_string = status;

    if (_grid.has_game_ended()) {
        _game_bot.finish_game(_grid.game_state());
    }

    return true;
}

bool Game::_play_bot(int8_t & row_index, int8_t & col_index) {
    MovePosition position;
    bool valid_move = _game_bot.get_next_move(_grid, position);
    if (!valid_move) {
        return false;
    }
    row_index = static_cast<int8_t>(position.first);
    col_index = static_cast<int8_t>(position.second);

    return _play(row_index, col_index);
}