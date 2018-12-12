#ifndef GAME_BOT_H
#define GAME_BOT_H

#include <set>
#include <vector>

#include "constants.h"
#include "match_box.h"
#include "grid.h"

enum class GridTransformation {
    GRID_UNEQUAL = 0,
    GRID_ROTATION_180 = 1,
    GRID_ROTATION_LEFT = 2,
    GRID_ROTATION_RIGHT = 3,
    GRID_REFLECTION_X = 4,
    GRID_REFLECTION_Y = 5,
    GRID_REFLECTION_DIAG = 6,
    GRID_REFLECTION_DIAG_1 = 7,
    GRID_REFLECTION_DIAG_2 = 8,
    GRID_EQUAL = 9,
};

class GameBot
{
public:
    GameBot();
    bool get_next_move(const Grid & grid, MovePosition & position);
    void finish_game(GameState game_state);
    void load_move_history(const std::vector<Move> move_history, const std::vector<MovePosition> move_position_history);
private:
    bool _transform_position(const Grid & grid, const Grid & match_box_grid, MovePosition & position);
    MatchBox * _find_match_box(const Grid & grid);
    bool _check_grids_equal(const Grid & first, const Grid & second);
    GridTransformation _get_grid_transformation(const Grid & first, const Grid & second);
    bool _check_grid_unique(const std::vector<Grid> & unique_grids, const Grid & grid);
    void _punish_moves();
    void _reward_moves();
    void _reward_drawn_moves();

    void _find_valid_children(const Grid & start_grid, std::map<size_t, std::vector<Grid>> & valid_grids);

    std::map<size_t, std::vector<Grid>> _valid_grids;
    std::map<size_t, std::vector<MatchBox> > _match_boxes;
    std::vector<MatchBox *> _match_box_history;
    std::vector<MovePosition> _move_position_history;
};

#endif // GAME_BOT_H
