
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"

using namespace GraphLib;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui{ new Ui::MainWindow }
{
    ui->setupUi(this);

    _canvas = new Canvas(this);
    setCentralWidget(_canvas);

    QPalette palette(c_paletteDefaultColor);
    palette.setColor(QPalette::ColorRole::Window, c_paletteDefaultColor);
    this->setPalette(palette);

    setFocusPolicy(Qt::StrongFocus);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, [&](){
        _canvas->update();
    });
    _timer->start(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::focusOutEvent(QFocusEvent *)
{
    _timer->stop();
}

void MainWindow::focusInEvent(QFocusEvent *)
{
    _timer->start();
}

