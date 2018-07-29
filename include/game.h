#ifndef GAME_H
#define GAME_H

#include <cstdint>
#include <string>

#include "constants.h"

class Game
{
public:
    Game();
    void reset();
    bool play_nought(int8_t row_index, int8_t col_index);
    bool play_cross(int8_t row_index, int8_t col_index);
    GameState get_game_state();
    std::string get_game_status_string();
private:
    bool _play(int8_t row_index, int8_t col_index, Move next_move);
    void _check_game();
    void _draw_game();
    Move _grid[NUM_ROWS][NUM_COLS];
    GameState _current_game_state;
    Move _next_player;
    std::string _game_status_string;
};

#endif // GAME_H
