#-------------------------------------------------
#
# Project created by QtCreator 2018-07-28T13:28:42
#
#-------------------------------------------------

QT       += core gui

CONFIG += debug
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TicTacToe
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .moc
UI_DIR = .ui

SOURCES += \
        src/game.cpp \
        src/game_bot.cpp \
        src/game_cell.cpp \
        src/game_widget.cpp \
        src/grid.cpp \
        src/main.cpp \
        src/mainwindow.cpp \
        src/match_box.cpp \
        src/statistics.cpp

HEADERS += \
        include/constants.h \
        include/game.h \
        include/game_bot.h \
        include/game_cell.h \
        include/game_widget.h \
        include/grid.h \
        include/mainwindow.h \
        include/match_box.h \
        include/statistics.h

INCLUDEPATH = include \
    MOC_DIR \
    UI_DIR

FORMS += \
        ui/mainwindow.ui
