#include "game_widget.h"

#include <cstdint>

#include <QFont>
#include <QGridLayout>
#include <QObject>
#include <QString>
#include <QVariant>

#include "constants.h"

#define CELL_WIDTH 100
#define CELL_HEIGHT 100

GameWidget::GameWidget(QWidget *parent) :
    QWidget(parent),
    _next_player(Move::EMPTY)
{
    QGridLayout * layout = new QGridLayout();
    this->setLayout(layout);

    QFont font;
    font.setBold(true);
    font.setPointSize(72);
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            _game_cells[row][col] = new GameCell(this, row, col);
            _game_cells[row][col]->resize(CELL_WIDTH, CELL_HEIGHT);
            _game_cells[row][col]->setFont(font);
            _game_cells[row][col]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            _game_cells[row][col]->setStyleSheet("background-color:white;");

            QObject::connect(_game_cells[row][col], SIGNAL(clicked(int8_t, int8_t)),
                             this, SLOT(_cell_clicked(int8_t, int8_t)));

            layout->addWidget(_game_cells[row][col], row, col);
        }
    }
    _status_label = new QLabel(this);
    _status_label->setAlignment(Qt::AlignRight);
    layout->addWidget(_status_label, NUM_ROWS, 0, 1, NUM_COLS);
}

GameWidget::~GameWidget() {
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            delete _game_cells[row][col];
            _game_cells[row][col] = nullptr;
        }
    }
}

void GameWidget::reset_game()
{
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            _game_cells[row][col]->setText("");
        }
    }
    _game.reset();
    _update_status_string(true);
    _unfreeze_game();
}

void GameWidget::_cell_clicked(int8_t row_index, int8_t col_index) {
    assert(row_index >= 0 && row_index < NUM_ROWS);
    assert(col_index >= 0 && col_index < NUM_COLS);

    Move move = Move::EMPTY;
    if (_next_player == Move::CROSS || _next_player == Move::EMPTY) {
        move = Move::CROSS;
    } else {
        move = Move::NOUGHT;
    }

    const bool success = _play_next(row_index, col_index, move);
    _update_status_string(success);
    _update_grid(success, row_index, col_index, move);
    _update_next_player(move);
}

bool GameWidget::_play_next(int8_t row_index, int8_t col_index, Move move)
{
    assert(row_index >= 0 && row_index < NUM_ROWS);
    assert(col_index >= 0 && col_index < NUM_COLS);

    bool success = false;
    if (move == Move::CROSS) {
        success = _game.play_cross(row_index, col_index);
    } else if (move == Move::NOUGHT) {
        success = _game.play_nought(row_index, col_index);
    }

    return success;
}

void GameWidget::_update_grid(bool success, int8_t row_index, int8_t col_index, Move move)
{
    assert(row_index >= 0 && row_index < NUM_ROWS);
    assert(col_index >= 0 && col_index < NUM_COLS);

    if (success) {
        const QString symbol = STR_MOVE_SYMBOL(move);
        _game_cells[row_index][col_index]->setText(symbol);
        _game_cells[row_index][col_index]->setEnabled(false);
    } else {
        // Do Nothing?
    }
}

void GameWidget::_update_status_string(bool success) {
    assert(_status_label != nullptr);
    if (!success) {
        _status_label->setText("ERROR!");
        return;
    }
    _status_label->setText(_game.get_game_status_string().c_str());
    GameState game_state = _game.get_game_state();
    switch (game_state) {
    case GameState::CROSS_WINS: {
        _status_label->setText("Crosses Win!");
        _freeze_game();
        break;
    }
    case GameState::NOUGHT_WINS: {
        _status_label->setText("Noughts Win!");
        _freeze_game();
        break;
    }
    case GameState::DRAW: {
        _status_label->setText("Draw!");
        _freeze_game();
        break;
    }
    default: {
        break;
    }
    }
}

void GameWidget::_update_next_player(Move move) {
    _next_player = move == Move::CROSS ? Move::NOUGHT :
                   move == Move::NOUGHT ? Move::CROSS :
                                          _next_player;
}

void GameWidget::_freeze_game()
{
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            _game_cells[row][col]->setEnabled(false);
        }
    }
}

void GameWidget::_unfreeze_game()
{
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            _game_cells[row][col]->setEnabled(true);
        }
    }
}
