#include "game_widget.h"

#include <cstdint>

#include <QFont>
#include <QGridLayout>
#include <QObject>
#include <QString>
#include <QVariant>

#include "constants.h"

#define CELL_WIDTH 200
#define CELL_HEIGHT 200

GameWidget::GameWidget(QWidget *parent) :
    QWidget(parent)
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
    _move_label = new QLabel(this);
    _move_label->setAlignment(Qt::AlignRight);
    layout->addWidget(_move_label, NUM_ROWS, 0, 1, NUM_COLS);

    _status_label = new QLabel(this);
    _status_label->setAlignment(Qt::AlignRight);
    layout->addWidget(_status_label, NUM_ROWS + 1, 0, 1, NUM_COLS);

    reset_game();
}

GameWidget::~GameWidget() {
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            delete _game_cells[row][col];
            _game_cells[row][col] = nullptr;
        }
    }
}

void GameWidget::reset_game() {
    printf("\n\n\n\n");
    printf("---------NEW GAME-----------\n");
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            _game_cells[row][col]->setText("");
            _game_cells[row][col]->setStyleSheet("background-color:white;");
        }
    }
    _game.reset();
    _update_status_string();
    _unfreeze_game();
    if (PLAY_BOT == true && FIRST_PLAYER_MOVE == BOT_MOVE) {
        int8_t row_index = 0, col_index = 0;
        Move next_move = Move::EMPTY;
        bool success = false;

        success = _game.play_bot(row_index, col_index, next_move);
        _update_status_string();
        _update_game_cells(success, row_index, col_index, next_move);
    }
}

void GameWidget::_cell_clicked(int8_t row_index, int8_t col_index) {
    assert(row_index >= 0 && row_index < NUM_ROWS);
    assert(col_index >= 0 && col_index < NUM_COLS);

    Move next_move = Move::EMPTY;
    bool success = _game.play_next(row_index, col_index, next_move);
    _update_status_string();
    _update_game_cells(success, row_index, col_index, next_move);

    if(success == false || PLAY_BOT == false) {
        return;
    }
    GameState game_state = _game.get_game_state();

    if (game_state != GameState::ONGOING) {
         return;
    }
    success = _game.play_bot(row_index, col_index, next_move);
    _update_status_string();
    _update_game_cells(success, row_index, col_index, next_move);
}

bool GameWidget::_play_next(int8_t row_index, int8_t col_index, Move & next_player) {
    assert(row_index >= 0 && row_index < NUM_ROWS);
    assert(col_index >= 0 && col_index < NUM_COLS);

    return _game.play_next(row_index, col_index, next_player);
}

void GameWidget::_update_game_cells(bool success, int8_t row_index, int8_t col_index, Move move) {
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

void GameWidget::_update_status_string() {
    assert(_move_label != nullptr);
    assert(_status_label != nullptr);

    _move_label->setText(_game.get_game_status_string().c_str());

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
        _status_label->setText("");
        break;
    }
    }

    std::vector<MovePosition> winning_moves = _game.get_winning_moves();
    for (MovePosition position : winning_moves) {
        _game_cells[position.first][position.second]->setStyleSheet("background-color:green;");
    }
}

void GameWidget::_freeze_game() {
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            _game_cells[row][col]->setEnabled(false);
        }
    }
}

void GameWidget::_unfreeze_game() {
    for (int8_t row = 0; row < NUM_ROWS; ++row) {
        for (int8_t col = 0; col < NUM_COLS; ++col) {
            _game_cells[row][col]->setEnabled(true);
        }
    }
}