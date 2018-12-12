#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>
#include <map>

#define NUM_ROWS (3)
#define NUM_COLS (3)
#define NUM_DIAGS (2)
#define MAX_RANK ((NUM_ROWS) * (NUM_COLS))

#define MAX_NUM_SEEDS 5

enum class Move {
    EMPTY = 0,
    NOUGHT = 1,
    CROSS = 2
};

#define STR_MOVE(s) (\
    s == Move::EMPTY ? "EMPTY" :\
    s == Move::NOUGHT ? "NOUGHT" :\
    s == Move::CROSS ? "CROSS" :\
    "UNKNOWN")

#define STR_MOVE_SYMBOL(s) (\
    s == Move::EMPTY ? "." :\
    s == Move::NOUGHT ? "O" :\
    s == Move::CROSS ? "X" :\
    "?")

#define STR_MOVE_SYMBOL_TO_MOVE(s) (\
    s == "." ? Move::EMPTY  :\
    s == "O" ? Move::NOUGHT  :\
    s == "X" ? Move::CROSS  :\
    Move::EMPTY)

#define STR_MOVE_TO_MOVE(s) (\
    s == "EMPTY" ? Move::EMPTY  :\
    s == "NOUGHT" ? Move::NOUGHT  :\
    s == "CROSS" ? Move::CROSS  :\
    Move::EMPTY)

enum class GameState {
    ONGOING = 0,
    NOUGHT_WINS = 1,
    CROSS_WINS = 2,
    DRAW = 3,
    INVALID = 4,
};

#define STR_GAME_STATE(g) (\
    g == GameState::ONGOING ? "ONGOING" :\
    g == GameState::NOUGHT_WINS ? "NOUGHT_WINS" :\
    g == GameState::CROSS_WINS ? "CROSS_WINS" :\
    g == GameState::DRAW ? "DRAW" :\
    g == GameState::INVALID ? "INVALID" :\
    "UNKNOWN")

enum class GameOutcome {
    BOT_WINS = 0,
    PLAYER_WINS = 1,
    DRAW = 2,
    UNKNOWN = 3,
};

#define STR_GAME_OUTCOME(g) (\
    g == GameOutcome::BOT_WINS ? "BOT_WINS" :\
    g == GameOutcome::PLAYER_WINS ? "PLAYER_WINS" :\
    g == GameOutcome::DRAW ? "DRAW" :\
    "UNKNOWN")

#define STR_GAME_OUTCOME_TO_GAME_OUTCOME(s) (\
    s == "BOT_WINS" ? GameOutcome::BOT_WINS  :\
    s == "PLAYER_WINS" ? GameOutcome::PLAYER_WINS :\
    s == "DRAW" ? GameOutcome::DRAW  :\
    GameOutcome::UNKNOWN)

typedef std::pair<size_t, size_t> MovePosition;

#define FIRST_PLAYER_MOVE (Move::CROSS)
#define PLAYER_MOVE (Move::NOUGHT)
#define BOT_MOVE (Move::CROSS)
#define PLAY_BOT (true)

#endif // CONSTANTS_H
