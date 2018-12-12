#include "statistics.h"

#include <cassert>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

#include <QDir>
#include <QStringList>

#include "constants.h"

Statistics::Statistics() {
	start_new_game();
}

void Statistics::start_new_game() {
	moves.clear();
	move_positions.clear();
}

void Statistics::log_move(Move move, MovePosition move_position) {
	moves.push_back(move);
	move_positions.push_back(move_position);
}

void Statistics::game_finished(GameState game_state) {
	GameOutcome game_outcome = _get_game_outcome_from_game_state(game_state);
	_save_game_moves(game_outcome);
}

void Statistics::read_move_history(std::vector<std::vector<Move>> & move_history, std::vector<std::vector<MovePosition>> & move_position_history, std::vector<GameState> & game_state_history) {
	move_history.clear();
	move_position_history.clear();
	game_state_history.clear();

	std::string game_log_directory_name = "GameLog";

	if (!QDir(game_log_directory_name.c_str()).exists()) {
		printf("Statistics::read_move_history(): Cannot access directory = %s\n", game_log_directory_name.c_str());
		fflush(stdout);
		return;
	}

	QStringList filter;
	filter << "GameLog_*.log";

	const QFileInfoList file_info_list = QDir(game_log_directory_name.c_str()).entryInfoList(filter);

	foreach (const QFileInfo file_info, file_info_list) {
		QString game_log_filename = file_info.absoluteFilePath();

		std::vector<Move> moves;
		std::vector<MovePosition> move_positions;
		GameState game_state;

		_read_game_moves( game_log_filename.toStdString(), moves, move_positions, game_state);

		assert(moves.size() == move_positions.size());
		if (moves.size() == 0) {
			printf("Warning, no moves reading from file '%s'\n", game_log_filename.toStdString().c_str());
			fflush(stdout);
			continue;
		}

		move_history.push_back(moves);
		move_position_history.push_back(move_positions);
		game_state_history.push_back(game_state);
	}
}

GameOutcome Statistics::_get_game_outcome_from_game_state(GameState game_state) {
	GameOutcome game_outcome = GameOutcome::DRAW;
	switch (game_state) {
		case GameState::NOUGHT_WINS:
			switch (PLAYER_MOVE) {
				case Move::CROSS:
					game_outcome = GameOutcome::BOT_WINS;
					break;
				case Move::NOUGHT:
					game_outcome = GameOutcome::PLAYER_WINS;
					break;
				default:
					printf("Statistics::_get_game_outcome_from_game_state(): Invalid Player Move = %s\n", STR_MOVE(PLAYER_MOVE));
					assert(false);
			}
			break;
		case GameState::CROSS_WINS:
			switch (PLAYER_MOVE) {
				case Move::CROSS:
					game_outcome = GameOutcome::PLAYER_WINS;
					break;
				case Move::NOUGHT:
					game_outcome = GameOutcome::BOT_WINS;
					break;
				default:
					printf("Statistics::_get_game_outcome_from_game_state(): Invalid Player Move = %s\n", STR_MOVE(PLAYER_MOVE));
					assert(false);
			}
			break;
		case GameState::DRAW:
			game_outcome = GameOutcome::DRAW;
			break;
		case GameState::ONGOING:
		case GameState::INVALID:
		default:
			printf("Statistics::_get_game_outcome_from_game_state(): Game State = %s. Game not finished.\n", STR_GAME_STATE(game_state));
			assert(false);
	}
	return game_outcome;
}

GameState Statistics::_get_game_state_from_game_outcome(GameOutcome game_outcome) {
	GameState game_state = GameState::INVALID;
	switch (game_outcome) {
		case GameOutcome::PLAYER_WINS:
			switch (PLAYER_MOVE) {
				case Move::CROSS:
					game_state = GameState::CROSS_WINS;
					break;
				case Move::NOUGHT:
					game_state = GameState::NOUGHT_WINS;
					break;
				default:
					printf("Statistics::_get_game_state_from_game_outcome(): Invalid Player Move = %s\n", STR_MOVE(PLAYER_MOVE));
					assert(false);
			}
			break;
		case GameOutcome::BOT_WINS:
			switch (PLAYER_MOVE) {
				case Move::CROSS:
					game_state = GameState::NOUGHT_WINS;
					break;
				case Move::NOUGHT:
					game_state = GameState::CROSS_WINS;
					break;
				default:
					printf("Statistics::_get_game_state_from_game_outcome(): Invalid Player Move = %s\n", STR_MOVE(PLAYER_MOVE));
					assert(false);
			}
			break;
		case GameOutcome::DRAW:
			game_state = GameState::DRAW;
			break;
		case GameOutcome::UNKNOWN:
		default:
			printf("Statistics::_get_game_state_from_game_outcome(): Invalid game outcome (%d).\n", static_cast<int32_t>(game_outcome));
			assert(false);
	}
	return game_state;
}

void Statistics::_save_game_moves(GameOutcome game_outcome) {
	if (PLAY_BOT == false) {
		printf("Statistics::_save_game_moves(): PLAY_BOT = false. Not saving game log");
		return;
	}
	std::string game_log_directory_name = "GameLog";

	if (!QDir(game_log_directory_name.c_str()).exists()) {
		bool success = QDir().mkdir(game_log_directory_name.c_str());
		if (!success) {
			printf("Statistics::_save_game_moves(): Cannot create directory = %s", game_log_directory_name.c_str());
			return;
		}
	}

	std::time_t time = std::time(nullptr);
	std::ostringstream oss_filename;
	oss_filename << game_log_directory_name << "/GameLog_" << static_cast<uint32_t>(time) << ".log";

	std::string game_log_filename = oss_filename.str();
	printf("Statistics::_save_game_moves(): Saving game log file : %s\n", game_log_filename.c_str());
	fflush(stdout);

	assert(moves.size() == move_positions.size());

	std::ofstream fout_game_log(game_log_filename);
	fout_game_log << "FirstPlayer:" << "\t" << STR_MOVE(FIRST_PLAYER_MOVE) << "\t" << "Bot:" << "\t" << STR_MOVE(BOT_MOVE) << std::endl;
	fout_game_log << "#Moves:" << "\t" << moves.size() << std::endl;
	for (size_t move_index = 0; move_index < moves.size(); ++move_index) {
		fout_game_log << STR_MOVE(moves.at(move_index)) << "\t" 
		<< move_positions.at(move_index).first << "\t"
		<< move_positions.at(move_index).second << std::endl;
	}
	fout_game_log << STR_GAME_OUTCOME(game_outcome) << std::endl;
	fout_game_log.close();
}

void Statistics::_read_game_moves(std::string game_log_filename, std::vector<Move> & moves, std::vector<MovePosition> & move_positions, GameState & game_state) {
	moves.clear();
	move_positions.clear();

	printf("Reading game log file = %s\n", game_log_filename.c_str());

	std::ifstream fin_game_log(game_log_filename);
	std::string tag, first_player_move, bot_move;
	size_t num_moves = 0;

	fin_game_log >> tag >> first_player_move >> tag >> bot_move;
	fin_game_log >> tag >> num_moves;

	for (size_t move_index = 0; move_index < num_moves; ++move_index) {
		std::string move;
		size_t row_index = 0, col_index = 0;
		fin_game_log >> move >> row_index >> col_index;

		moves.push_back(STR_MOVE_TO_MOVE(move));
		move_positions.push_back(std::make_pair(row_index, col_index));
	}
	std::string game_outcome_string;
	fin_game_log >> game_outcome_string;
	fin_game_log.close();

	game_state = _get_game_state_from_game_outcome(STR_GAME_OUTCOME_TO_GAME_OUTCOME(game_outcome_string));
}