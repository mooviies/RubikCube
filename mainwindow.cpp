#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionReset, SIGNAL(triggered(bool)), ui->pushButtonReset, SLOT(click()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), ui->pushButtonSave, SLOT(click()));
    connect(ui->actionLoad, SIGNAL(triggered(bool)), ui->pushButtonLoad, SLOT(click()));

    connect(ui->pushButtonSave, SIGNAL(clicked(bool)), this, SLOT(save()));
    connect(ui->pushButtonLoad, SIGNAL(clicked(bool)), this, SLOT(load()));
    connect(ui->pushButtonReset, SIGNAL(clicked(bool)), this, SLOT(reset()));
    connect(ui->pushButtonExecute, SIGNAL(clicked(bool)), this, SLOT(execute()));
    connect(ui->pushButtonScramble, SIGNAL(clicked(bool)), this, SLOT(scramble()));
    connect(ui->pushButtonSolve, SIGNAL(clicked(bool)), this, SLOT(solve()));

    ui->openGLWidget->setCube(new RubiksCube(3));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    ui->openGLWidget->keyPressEvent(event);
}

void MainWindow::save()
{

}

void MainWindow::load()
{

}

void MainWindow::reset()
{

}

void MainWindow::execute()
{

}

void MainWindow::scramble()
{

}

void MainWindow::solve()
{

}
