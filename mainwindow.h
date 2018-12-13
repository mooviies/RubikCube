/*
    Virtual Rubik's Cube is a tool to explore Rubik's cubes of any sizes.
    Copyright (C) 2018 mooviies

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    https://github.com/mooviies/RubikCube
*/

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
    void addToHistory(int flags);
    void addToHistory(const QList<int>& flagsList);

private slots:
    void newCube();
    void save();
    void load();
    void reset();
    void undo();
    void redo();

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
