#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    // 自动运行定时器
    //
    connect(&timer,
            &QTimer::timeout,
            this,
            &MainWindow::runCPU);

    //
    // 加载测试程序
    //
    loadProgram();

    //
    // 更新界面
    //
    updateUI();

    //
    // 可选：界面美化
    //
    setStyleSheet(
        "QMainWindow {"
        "background:#202124;"
        "color:white;"
        "}"

        "QLabel {"
        "font-size:16px;"
        "color:#00FFAA;"
        "}"

        "QPushButton {"
        "background:#303134;"
        "border:1px solid #555;"
        "padding:6px;"
        "color:white;"
        "}"

        "QTextEdit {"
        "background:black;"
        "color:#00FF00;"
        "font-family:Consolas;"
        "}"
        );
}

MainWindow::~MainWindow()
{
    delete ui;
}

//
// ==================== 加载程序 ====================
//

void MainWindow::loadProgram()
{
    //
    // 示例程序：
    //
    // R0 = 5
    // R1 = 3
    // R0 = R0 + R1
    // memory[20] = R0
    // halt
    //

    // LDI R0,5
    cpu.setMem(0, 0x1005);

    // LDI R1,3
    cpu.setMem(1, 0x1203);

    // ADD R0,R1
    cpu.setMem(2, 0x3040);

    // ST R0,20
    cpu.setMem(3, 0x9014);

    // HALT
    cpu.setMem(4, 0xF000);
}

//
// ==================== 更新界面 ====================
//

void MainWindow::updateUI()
{
    //
    // CPU寄存器
    //
    ui->pcLabel->setText(
        QString("0x%1")
            .arg(cpu.PC,4,16,QChar('0')));

    ui->irLabel->setText(
        QString("0x%1")
            .arg(cpu.IR,4,16,QChar('0')));

    ui->marLabel->setText(
        QString("0x%1")
            .arg(cpu.MAR,4,16,QChar('0')));

    ui->mdrLabel->setText(
        QString("0x%1")
            .arg(cpu.MDR,4,16,QChar('0')));

    ui->busLabel->setText(
        QString("0x%1")
            .arg(cpu.BUS,4,16,QChar('0')));

    //
    // 通用寄存器
    //
    ui->r0Label->setText(
        QString("0x%1")
            .arg(cpu.R[0],4,16,QChar('0')));

    ui->r1Label->setText(
        QString("0x%1")
            .arg(cpu.R[1],4,16,QChar('0')));

    ui->r2Label->setText(
        QString("0x%1")
            .arg(cpu.R[2],4,16,QChar('0')));

    ui->r3Label->setText(
        QString("0x%1")
            .arg(cpu.R[3],4,16,QChar('0')));

    ui->r4Label->setText(
        QString("0x%1")
            .arg(cpu.R[4],4,16,QChar('0')));

    ui->r5Label->setText(
        QString("0x%1")
            .arg(cpu.R[5],4,16,QChar('0')));

    ui->r6Label->setText(
        QString("0x%1")
            .arg(cpu.R[6],4,16,QChar('0')));

    ui->r7Label->setText(
        QString("0x%1")
            .arg(cpu.R[7],4,16,QChar('0')));

    //
    // Flags
    //
    ui->zLabel->setText(cpu.z_flag ? "1" : "0");

    ui->cLabel->setText(cpu.c_flag ? "1" : "0");

    ui->sLabel->setText(cpu.s_flag ? "1" : "0");

    ui->oLabel->setText(cpu.o_flag ? "1" : "0");

    //
    // 当前周期
    //
    QString cycle;

    switch(cpu.current_cycle)
    {
    case CPU::T0:
        cycle = "T0";
        break;

    case CPU::T1:
        cycle = "T1";
        break;

    case CPU::T2:
        cycle = "T2";
        break;

    case CPU::T3:
        cycle = "T3";
        break;
    }

    ui->cycleLabel->setText(cycle);

    //
    // 状态文本
    //
    ui->stateText->setText(
        QString::fromStdString(
            cpu.getStateString()));
}

//
// ==================== 单步执行 ====================
//

void MainWindow::on_stepButton_clicked()
{
    if(cpu.halted)
        return;

    cpu.Step();

    updateUI();
}

//
// ==================== 自动运行 ====================
//

void MainWindow::on_runButton_clicked()
{
    if(timer.isActive())
    {
        timer.stop();

        ui->runButton->setText("Run");
    }
    else
    {
        timer.start(500);

        ui->runButton->setText("Stop");
    }
}

//
// ==================== 定时器运行 ====================
//

void MainWindow::runCPU()
{
    if(cpu.halted)
    {
        timer.stop();

        ui->runButton->setText("Run");

        return;
    }

    cpu.Step();

    updateUI();
}

//
// ==================== 重置 ====================
//

void MainWindow::on_resetButton_clicked()
{
    timer.stop();

    cpu.Reset();

    loadProgram();

    updateUI();

    ui->runButton->setText("Run");
}