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
#include <QDialog>
#include <QTimer>
#include <QtAdvancedStylesheet.h>

#include "vrcmodel.h"
#include "vrcview.h"
#include "vrccontroller.h"
#include "ui_dialoghelp.h"

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
    void setModel(VRCModel *model);

protected:
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void newCube();
    void save();
    void load();
    void reset();
    void undo();
    void redo();
    void fastmode(bool activated);
    void about();
    void resetSettings();
    void loadSettings();
    void loadStyle();

    void reverse();
    void execute();

    void scramble();
    void solve();

    void rotateWithControls(VRCAction::Rotation rotation);
    void rotateClockwise();
    void rotateCounterClockwise();
    void rotateTurn180();

    void uncheckLayerControls(QPushButton* exception = nullptr);

    void pushF(bool checked);
    void pushB(bool checked);
    void pushL(bool checked);
    void pushR(bool checked);
    void pushU(bool checked);
    void pushD(bool checked);
    void pushE(bool checked);
    void pushM(bool checked);
    void pushS(bool checked);
    void pushx(bool checked);
    void pushy(bool checked);
    void pushz(bool checked);

    void updateController();

private:
    void initControls();

private:
    Ui::MainWindow *ui;
    VRCModel *_model;
    VRCView *_view;
    VRCController *_controller;
    QTimer *_timer;

    QList<QPushButton*> _layerControls;

    int _currentCommand;

    QSettings _settings;
    Ui::Dialog _aboutUI;
    QDialog _about;
    QRect _sizeBeforeMaximize;
    acss::QtAdvancedStylesheet _advancedStylesheet;
};

#endif // MAINWINDOW_H
