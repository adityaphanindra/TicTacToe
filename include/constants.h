#ifndef CONSTANTS_H
#define CONSTANTS_H

#define NUM_ROWS 3
#define NUM_COLS 3
#define NUM_DIAGS 2

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

enum class GameState {
    ONGOING = 0,
    NOUGHT_WINS = 1,
    CROSS_WINS = 2,
    DRAW = 3
};

#define STR_GAME_STATE(g) (\
    g == GameState::ONGOING ? "ONGOING" :\
    g == GameState::NOUGHT_WINS ? "NOUGHT_WINS" :\
    g == GameState::CROSS_WINS ? "CROSS_WINS" :\
    g == GameState::DRAW ? "DRAW" :\
    "UNKNOWN")

#endif // CONSTANTS_H
