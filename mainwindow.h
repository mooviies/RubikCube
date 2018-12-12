#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QPushButton>
#include <QSettings>

#include "rubikscube.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool rotate(int flags);
    bool rotate(const QList<int>& flagsList);
    void setCube(RubiksCube *cube);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    QList<int> getCommands(const QString& expression);

private slots:
    void newCube();
    void save();
    void load();
    void reset();

    void execute();

    void scramble();
    void solve();

    void rotateWithControls(int flags);
    void rotateClockwise();
    void rotateCounterClockwise();
    void rotateTurn180();

    void pushF(bool checked);
    void pushB(bool checked);
    void pushL(bool checked);
    void pushR(bool checked);
    void pushU(bool checked);
    void pushD(bool checked);
    void pushE(bool checked);
    void pushM(bool checked);
    void pushS(bool checked);
    void pushX(bool checked);
    void pushY(bool checked);
    void pushZ(bool checked);

    void pushf(bool checked);
    void pushb(bool checked);
    void pushl(bool checked);
    void pushr(bool checked);
    void pushu(bool checked);
    void pushd(bool checked);

    void nbLChanged(int value);

private:
    Ui::MainWindow *ui;
    RubiksCube *_cube;

    QList<QPushButton*> _groupA;
    QList<QPushButton*> _groupB;

    int _currentCommand;
    QList<int> _history;

    QSettings _settings;
};

#endif // MAINWINDOW_H
