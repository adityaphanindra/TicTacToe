#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QMenu>
#include <QObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->statusBar()->setSizeGripEnabled(false);
    this->setFixedSize(600, 600);
    _create_menu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_reset_game()
{
    assert(ui->game_widget != nullptr);
    ui->game_widget->reset_game();
}

void MainWindow::_create_menu()
{
    QMenu * menu = menuBar()->addMenu(tr("&Menu"));
    QAction * reset_action = new QAction(tr("&Reset"), this);
    reset_action->setStatusTip(tr("Reset"));
    QObject::connect(reset_action, SIGNAL(triggered()), this, SLOT(_reset_game()));

    menu->addAction(reset_action);
}
