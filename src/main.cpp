#include "mainwindow.h"
#include <QApplication>

#include <cstdio>

#include "constants.h"
#include "game.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
