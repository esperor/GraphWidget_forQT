
#pragma once

#include <QMainWindow>
#include <QObject>
#include <QTimer>

#include "GraphLib_global.h"
#include "canvas.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void focusOutEvent(QFocusEvent *) override;
    void focusInEvent(QFocusEvent *) override;

private:
    Ui::MainWindow *ui;
    GraphLib::Canvas *_canvas;
    QTimer *_timer;
};

