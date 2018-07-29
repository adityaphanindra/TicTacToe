#include "game_cell.h"

GameCell::GameCell(QWidget * parent, int8_t row_index, int8_t col_index) :
    QLabel(parent),
    _row_index(row_index),
    _col_index(col_index)
{

}

void GameCell::mouseReleaseEvent(QMouseEvent *)
{
    emit clicked(_row_index, _col_index);
}
