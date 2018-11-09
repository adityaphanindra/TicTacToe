#ifndef MATCH_BOX_H
#define MATCH_BOX_H

#include <cstdint>
#include <istream>
#include <map>

#include "constants.h"
#include "grid.h"

class MatchBox
{
public:
	MatchBox();
    MatchBox(const Grid grid);
    MatchBox(const MatchBox & other);
    void operator=(const MatchBox & other);
    MovePosition pick_random_move();
    Grid get_grid() const;
    void reward_drawn_move(MovePosition move_position);
    void reward_move(MovePosition move_position);
    void punish_move(MovePosition move_position);
private:
    void _print_remaining_seeds() const;
    Grid _grid;
    int8_t _remaining_seeds[NUM_ROWS][NUM_COLS];
};

#endif // MATCH_BOX_H
