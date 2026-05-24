#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "cpu.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_stepButton_clicked();

    void on_runButton_clicked();

    void on_resetButton_clicked();

    void runCPU();

    void on_assembleButton_clicked();




private:

    Ui::MainWindow *ui;

    CPU cpu;

    QTimer timer;

    void updateUI();

    void loadProgram();

    void assembleProgram();
};

#endif