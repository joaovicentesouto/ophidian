#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "dialoglefdef.h"
#include "application.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    control::Application mApp;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionICCAD_2017_triggered();

    void on_actionICCAD_2015_triggered();

    void on_actionQuit_triggered();

    void on_circuit_labelsChanged(QString name, size_t die, size_t cells, size_t pins, size_t nets);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H