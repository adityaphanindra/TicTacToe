#ifndef GAME_H
#define GAME_H

#include <cstdint>
#include <string>
#include <tuple>

#include "constants.h"
#include "game_bot.h"

class Game
{
public:
    Game();
    void reset();
    bool play_next(int8_t row_index, int8_t col_index, Move & next_move);
    bool play_bot(int8_t & row_index, int8_t & col_index, Move & next_move);
    GameState get_game_state();
    std::string get_game_status_string();
    std::vector<MovePosition> get_winning_moves();
private:
    bool _play(int8_t row_index, int8_t col_index);
    bool _play_bot(int8_t & row_index, int8_t & col_index);
    void _switch_next_player();
    // void _check_game();
    void _print_game();

    Grid _grid;
    GameBot _game_bot;
    // Move _next_player;
    std::string _game_status_string;
    // GameState _current_game_state;
    // std::tuple<int8_t, int8_t, int8_t> _winning_moves;
};

#endif // GAME_H
