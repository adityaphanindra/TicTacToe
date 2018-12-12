#include "game_bot.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "grid.h"

#include <fstream>

GameBot::GameBot() {
    Grid start_grid;

    _valid_grids.clear();
    _valid_grids[0].push_back(start_grid);

    size_t num_valid_grids = 0;
    size_t num_valid_grids_by_rank[MAX_RANK + 1] = {0};

    _find_valid_children(start_grid, _valid_grids);

    for (size_t rank = 0; rank < MAX_RANK + 1; ++rank) {
        num_valid_grids_by_rank[rank] = _valid_grids[rank].size();
        num_valid_grids += num_valid_grids_by_rank[rank];

        for (const Grid & valid_grid : _valid_grids[rank]) {
            MatchBox match_box(valid_grid);
            _match_boxes[rank].push_back(match_box);
        }
        printf("GameBot::GameBot(): Rank = %ld, num_valid_grids = %ld\n", rank, num_valid_grids_by_rank[rank]);
    }
    printf("GameBot::GameBot(): Valid grids found = %ld\n", num_valid_grids);
    fflush(stdout);
}

bool GameBot::get_next_move(const Grid & grid, MovePosition & position) {
    bool valid_position = false;
    MatchBox * match_box = _find_match_box(grid);
    MovePosition position_before_transform;

    if (match_box == nullptr) {
        printf("Corresponding match box not found.\n");
        grid.print_grid();
        printf("Matchboxes:\n");
        size_t rank = grid.rank();
        for (MatchBox & current_match_box : _match_boxes.at(rank)) {
            current_match_box.get_grid().print_grid();
        }
        return false;
    }
    position_before_transform = match_box->pick_random_move();
    position = position_before_transform;
    valid_position = _transform_position(match_box->get_grid(), grid, position);

    if (valid_position) {
        _match_box_history.push_back(match_box);
        _move_position_history.push_back(position_before_transform);
    }

    printf("GameBot::get_next_move(): GameBot wants to play %s at (%lu, %lu)\n", STR_MOVE(BOT_MOVE), position.first, position.second);
    return valid_position;
}

void GameBot::finish_game(GameState game_state) {
    assert(_match_box_history.size() == _move_position_history.size());
    switch (game_state) {
        case GameState::DRAW:
            _reward_drawn_moves();
            break;
        case GameState::NOUGHT_WINS:
            if (BOT_MOVE == Move::NOUGHT) {
                _reward_moves();
            } else {
                _punish_moves();
            }
            break;
        case GameState::CROSS_WINS:
            if (BOT_MOVE == Move::CROSS) {
                _reward_moves();
            } else {
                _punish_moves();
            }
            break;
        case GameState::ONGOING:
        case GameState::INVALID:
        default:
            // Shouldn't be here
            return;
    }
    _match_box_history.clear();
    _move_position_history.clear();
}

void GameBot::load_move_history(const std::vector<Move> move_history, const std::vector<MovePosition> move_position_history) {
    assert(move_history.size() == move_position_history.size());

    _match_box_history.clear();
    _move_position_history.clear();

    Grid grid;
    for (size_t move_index = 0; move_index < move_history.size(); ++move_index) {
        const Move move = move_history.at(move_index);
        const size_t row_index = move_position_history.at(move_index).first;
        const size_t col_index = move_position_history.at(move_index).second;

        printf("Replaying moves: move = %s at (%lu, %lu)\n", STR_MOVE(move), row_index, col_index);
        assert(move != Move::EMPTY);
        assert(row_index < NUM_ROWS);
        assert(col_index < NUM_COLS);

        Grid grid_before_move = grid;
        grid.set_value(row_index, col_index, move);

        if (grid_before_move.rank() >= _match_boxes.size()) {
            break;
        }
        if (move != BOT_MOVE) {
            continue;
        }
        if (grid_before_move.has_game_ended()) {
            continue;
        }

        MatchBox * match_box = _find_match_box(grid_before_move);
        grid_before_move.print_grid();
        assert(match_box != nullptr);
        match_box->get_grid().print_grid();

        MovePosition transformed_position(row_index, col_index);
        _transform_position(grid_before_move, match_box->get_grid(), transformed_position);

        printf("Transformed position: (%lu, %lu)\n", transformed_position.first, transformed_position.second);
        _match_box_history.push_back(match_box);
        _move_position_history.push_back(transformed_position);
    }
}

bool GameBot::_transform_position(const Grid & grid, const Grid & match_box_grid, MovePosition & position) {
    size_t row_index = position.first;
    size_t col_index = position.second;
    assert(row_index < NUM_ROWS);
    assert(col_index < NUM_COLS);

    GridTransformation grid_transformation = _get_grid_transformation(grid, match_box_grid);
    switch (grid_transformation) {
        case GridTransformation::GRID_UNEQUAL: {
            printf("GameBot::_transform_position(): GRID_UNEQUAL\n");
            return false;
        }
        case GridTransformation::GRID_ROTATION_180: {
            printf("GameBot::_transform_position(): GRID_ROTATION_180\n");
            position.first = NUM_COLS - col_index - 1;
            position.second = NUM_ROWS - row_index - 1;
            return true;
        }
        case GridTransformation::GRID_ROTATION_LEFT: {
            printf("GameBot::_transform_position(): GRID_ROTATION_LEFT\n");
            position.first = NUM_COLS - col_index - 1;
            position.second = row_index;
            return true;
        }
        case GridTransformation::GRID_ROTATION_RIGHT: {
            printf("GameBot::_transform_position(): GRID_ROTATION_RIGHT\n");
            position.first = col_index;
            position.second = NUM_ROWS - row_index - 1;
            return true;
        }
        case GridTransformation::GRID_REFLECTION_X: {
            printf("GameBot::_transform_position(): GRID_REFLECTION_X\n");
            position.first = NUM_ROWS - row_index - 1;
            position.second = col_index;
            return true;
        }
        case GridTransformation::GRID_REFLECTION_Y: {
            printf("GameBot::_transform_position(): GRID_REFLECTION_Y\n");
            position.first = row_index;
            position.second = NUM_COLS - col_index - 1;
            return true;
        }
        case GridTransformation::GRID_REFLECTION_DIAG: {
            printf("GameBot::_transform_position(): GRID_REFLECTION_DIAG\n");
            position.first = NUM_COLS - row_index - 1;
            position.second = NUM_ROWS - col_index - 1;
            return true;
        }
        case GridTransformation::GRID_REFLECTION_DIAG_1: {
            printf("GameBot::_transform_position(): GRID_REFLECTION_DIAG_1\n");
            position.first = col_index;
            position.second = row_index;
            return true;
        }
        case GridTransformation::GRID_REFLECTION_DIAG_2: {
            printf("GameBot::_transform_position(): GRID_REFLECTION_DIAG_2\n");
            position.first = NUM_COLS - col_index - 1;
            position.second = NUM_ROWS - row_index - 1;
            return true;
        }
        case GridTransformation::GRID_EQUAL: {
            printf("GameBot::_transform_position(): GRID_EQUAL\n");
            return true;
        }
        default:
            return false;
    }
    return false;
}

MatchBox * GameBot::_find_match_box(const Grid & grid) {
    MatchBox * match_box = nullptr;
    size_t rank = grid.rank();
    assert(rank < _match_boxes.size());

    for (MatchBox & current_match_box : _match_boxes.at(rank)) {
        if (_check_grids_equal(current_match_box.get_grid(), grid)) {
            match_box = &current_match_box;

            return match_box;
        }
    }
    return nullptr;
}

bool GameBot::_check_grids_equal(const Grid & first, const Grid & second) {
    if (_get_grid_transformation(first, second) != GridTransformation::GRID_UNEQUAL) {
        return true;
    }
    return false;
}

GridTransformation GameBot::_get_grid_transformation(const Grid & first, const Grid & second) {
    if (first == second) {
        return GridTransformation::GRID_EQUAL;
    }
    bool symmetric = true, rotation_180 = true, rotation_left = true, rotation_right = true, reflection_x = true, reflection_y = true;
    bool reflection_diag = true, reflection_diag_1 = true, reflection_diag_2 = true;
    for (size_t row = 0; row < NUM_ROWS && symmetric; ++row) {
        for (size_t col = 0; col < NUM_COLS && symmetric; ++col) {
            if (NUM_ROWS == NUM_COLS && first.value(row, col) != second.value(NUM_COLS - col - 1, NUM_ROWS - row - 1)) {
                rotation_180 = false;
            }
            if (NUM_ROWS == NUM_COLS && first.value(row, col) != second.value(NUM_COLS - col - 1, row)) {
                rotation_left = false;
            }
            if (NUM_ROWS == NUM_COLS && first.value(row, col) != second.value(col, NUM_ROWS - row - 1)) {
                rotation_right = false;
            }
            if (first.value(row, col) != second.value(NUM_ROWS - row - 1, col)) {
                reflection_x = false;
            }
            if (first.value(row, col) != second.value(row, NUM_COLS - col - 1)) {
                reflection_y = false;
            }
            if (NUM_ROWS == NUM_COLS && first.value(row, col) != second.value(NUM_COLS - row - 1, NUM_ROWS - col - 1)) {
                reflection_diag = false;
            }
            if (NUM_ROWS == NUM_COLS && first.value(row, col) != second.value(col, row)) {
                reflection_diag_1 = false;
            }
            if (NUM_ROWS == NUM_COLS && first.value(row, col) != second.value(NUM_COLS - col - 1, NUM_ROWS - row - 1)) {
                reflection_diag_2 = false;
            }
            symmetric = rotation_180 || rotation_left || rotation_right || reflection_x || reflection_y || 
            reflection_diag || reflection_diag_1 || reflection_diag_2;
        }
    }

    if (rotation_180) {
        return GridTransformation::GRID_ROTATION_180;
    } else if (rotation_left) {
        return GridTransformation::GRID_ROTATION_LEFT;
    } else if (rotation_right) {
        return GridTransformation::GRID_ROTATION_RIGHT;
    } else if (reflection_x) {
        return GridTransformation::GRID_REFLECTION_X;
    } else if (reflection_y) {
        return GridTransformation::GRID_REFLECTION_Y;
    } else if (reflection_diag) {
        return GridTransformation::GRID_REFLECTION_DIAG;
    } else if (reflection_diag_1) {
        return GridTransformation::GRID_REFLECTION_DIAG_1;
    } else if (reflection_diag_2) {
        return GridTransformation::GRID_REFLECTION_DIAG_2;
    }else {
        return GridTransformation::GRID_UNEQUAL;
    }
}

bool GameBot::_check_grid_unique(const std::vector<Grid> & unique_grids, const Grid & grid) {
    for (const Grid & unique_grid : unique_grids) {
        if (_check_grids_equal(grid, unique_grid)) {
            return false;
        }
    }
    return true;
}
void GameBot::_punish_moves() {
    for (size_t match_box_index = 0; match_box_index < _match_box_history.size(); ++match_box_index) {
        MatchBox * match_box = _match_box_history.at(match_box_index);
        MovePosition move_position = _move_position_history.at(match_box_index);
        assert(match_box != nullptr);

        printf("GameBot::_punish_moves(): move_position = (%lu, %lu)\n", move_position.first, move_position.second);
        match_box->punish_move(move_position);
    }
}

void GameBot::_reward_moves() {
    for (size_t match_box_index = 0; match_box_index < _match_box_history.size(); ++match_box_index) {
        MatchBox * match_box = _match_box_history.at(match_box_index);
        MovePosition move_position = _move_position_history.at(match_box_index);
        assert(match_box != nullptr);

        printf("GameBot::_reward_moves(): move_position = (%lu, %lu)\n", move_position.first, move_position.second);
        match_box->reward_move(move_position);
    }
}

void GameBot::_reward_drawn_moves() {
    for (size_t match_box_index = 0; match_box_index < _match_box_history.size(); ++match_box_index) {
        MatchBox * match_box = _match_box_history.at(match_box_index);
        MovePosition move_position = _move_position_history.at(match_box_index);
        assert(match_box != nullptr);

        printf("GameBot::_reward_drawn_moves(): move_position = (%lu, %lu)\n", move_position.first, move_position.second);
        match_box->reward_drawn_move(move_position);
    }
}

void GameBot::_find_valid_children(const Grid & start_grid, std::map<size_t, std::vector<Grid>> & valid_grids) {
    std::vector<MovePosition> valid_positions = start_grid.valid_move_positions();

    for (MovePosition position : valid_positions) {
        Grid grid_after_move = start_grid;
        grid_after_move.set_value(position.first, position.second);

        size_t rank = grid_after_move.rank();
        bool unique_grid = _check_grid_unique(valid_grids[rank], grid_after_move);
        if (!unique_grid) {
            continue;
        }

        if (grid_after_move.has_game_ended()) {
            continue;
        }

        valid_grids[rank].push_back(grid_after_move);
        _find_valid_children(grid_after_move, valid_grids);
    }
}