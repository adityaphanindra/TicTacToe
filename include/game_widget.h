#ifndef GAME_WIDGET_H
#define GAME_WIDGET_H

#include <QLabel>
#include <QWidget>

#include "constants.h"
#include "game.h"
#include "game_cell.h"

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    virtual ~GameWidget() override;
    void reset_game();
private slots:
    void _cell_clicked(int8_t row_index, int8_t col_index);
private:
    bool _play_next(int8_t row_index, int8_t col_index, Move & next_player);
    void _update_game_cells(bool success, int8_t row_index, int8_t col_index, Move move);
    void _update_status_string();
    void _freeze_game();
    void _unfreeze_game();

    GameCell * _game_cells[NUM_ROWS][NUM_COLS];
    QLabel * _move_label;
    QLabel * _status_label;
    Game _game;
};

#endif // GAME_WIDGET_H
