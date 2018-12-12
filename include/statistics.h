#ifndef STATISTICS_H
#define STATISTICS_H

#include <cstdint>
#include <vector>

#include "constants.h"

class Statistics
{
public:
    explicit Statistics();
    void start_new_game();
    void log_move(Move move, MovePosition move_position);
    void game_finished(GameState game_state);
    void read_move_history(std::vector<std::vector<Move>> & move_history, std::vector<std::vector<MovePosition>> & move_position_history, std::vector<GameState> & game_state_history);
private:
	GameOutcome _get_game_outcome_from_game_state(GameState game_state);
	GameState _get_game_state_from_game_outcome(GameOutcome game_outcome);
	void _save_game_moves(GameOutcome game_outcome);
	void _read_game_moves(std::string game_log_filename, std::vector<Move> & moves, std::vector<MovePosition> & move_positions, GameState & game_state);

	std::vector<Move> moves;
	std::vector<MovePosition> move_positions;
};

#endif // STATISTICS_H
