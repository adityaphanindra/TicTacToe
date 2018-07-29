#ifndef GAME_CELL_H
#define GAME_CELL_H

#include <cstdint>

#include <QLabel>
#include <QMouseEvent>
#include <QObject>
#include <QWidget>

class GameCell : public QLabel
{
    Q_OBJECT
public:
    explicit GameCell(QWidget * parent, int8_t row_index, int8_t col_index);
signals:
    void clicked(int8_t row_index, int8_t col_index);
protected:
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
private:
    int8_t _row_index;
    int8_t _col_index;
};

#endif // GAME_CELL_H
