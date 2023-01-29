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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QRegularExpression>
#include <QInputDialog>
#include <QRandomGenerator>
#include <QFileDialog>
#include <QStyle>
#include <QGuiApplication>
#include <QScreen>
#include <QWindowStateChangeEvent>

#include "constants.h"

typedef VRCAction::Rotation Rotation;
typedef VRCAction::Layer Layer;
typedef VRCAction::LayerMask LayerMask;
typedef VRCAction::Option Option;

using namespace acss;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _settings("mooviies", "VirtualRubiksCube")
{
    ui->setupUi(this);
    _aboutUI.setupUi(&_about);

    connect(ui->actionNew, SIGNAL(triggered(bool)), ui->pushButtonNew, SLOT(click()));
    connect(ui->actionReset, SIGNAL(triggered(bool)), ui->pushButtonReset, SLOT(click()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(save()));
    connect(ui->actionLoad, SIGNAL(triggered(bool)), this, SLOT(load()));
    connect(ui->actionFastMode, SIGNAL(toggled(bool)), this, SLOT(fastmode(bool)));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(ui->actionResetSettings, SIGNAL(triggered(bool)), this, SLOT(resetSettings()));
    //connect(ui->actionUndo, SIGNAL(triggered(bool)), this, SLOT(undo()));
    //connect(ui->actionRedo, SIGNAL(triggered(bool)), this, SLOT(redo()));

    connect(ui->pushButtonNew, SIGNAL(clicked(bool)), this, SLOT(newCube()));
    connect(ui->pushButtonReset, SIGNAL(clicked(bool)), this, SLOT(reset()));
    connect(ui->pushButtonReverse, SIGNAL(clicked(bool)), this, SLOT(reverse()));
    connect(ui->pushButtonExecute, SIGNAL(clicked(bool)), this, SLOT(execute()));
    connect(ui->pushButtonScramble, SIGNAL(clicked(bool)), this, SLOT(scramble()));
    connect(ui->pushButtonSolve, SIGNAL(clicked(bool)), this, SLOT(solve()));
    connect(ui->control_clockwise, SIGNAL(clicked(bool)), this, SLOT(rotateClockwise()));
    connect(ui->control_counter, SIGNAL(clicked(bool)), this, SLOT(rotateCounterClockwise()));
    connect(ui->control_turn180, SIGNAL(clicked(bool)), this, SLOT(rotateTurn180()));
    connect(ui->control_nbLayer, SIGNAL(valueChanged(int)), this, SLOT(nbLChanged(int)));

    connect(ui->control_F, SIGNAL(toggled(bool)), this, SLOT(pushF(bool)));
    connect(ui->control_B, SIGNAL(toggled(bool)), this, SLOT(pushB(bool)));
    connect(ui->control_L, SIGNAL(toggled(bool)), this, SLOT(pushL(bool)));
    connect(ui->control_R, SIGNAL(toggled(bool)), this, SLOT(pushR(bool)));
    connect(ui->control_U, SIGNAL(toggled(bool)), this, SLOT(pushU(bool)));
    connect(ui->control_D, SIGNAL(toggled(bool)), this, SLOT(pushD(bool)));
    connect(ui->control_E, SIGNAL(toggled(bool)), this, SLOT(pushE(bool)));
    connect(ui->control_M, SIGNAL(toggled(bool)), this, SLOT(pushM(bool)));
    connect(ui->control_S, SIGNAL(toggled(bool)), this, SLOT(pushS(bool)));
    connect(ui->control_x, SIGNAL(toggled(bool)), this, SLOT(pushx(bool)));
    connect(ui->control_y, SIGNAL(toggled(bool)), this, SLOT(pushy(bool)));
    connect(ui->control_z, SIGNAL(toggled(bool)), this, SLOT(pushz(bool)));

    _layerControls.append(ui->control_F);
    _layerControls.append(ui->control_B);
    _layerControls.append(ui->control_L);
    _layerControls.append(ui->control_R);
    _layerControls.append(ui->control_U);
    _layerControls.append(ui->control_D);
    _layerControls.append(ui->control_E);
    _layerControls.append(ui->control_M);
    _layerControls.append(ui->control_S);
    _layerControls.append(ui->control_x);
    _layerControls.append(ui->control_y);
    _layerControls.append(ui->control_z);

    _view = nullptr;
    _controller = nullptr;
    setModel(new VRCModel(_settings.value(SETTINGS_KEY_SIZE, 3).toInt()));
    loadSettings();
    loadStyle();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _model;
    delete _view;
    delete _controller;
}

bool MainWindow::rotate(int flags)
{
    /*if(_cube->rotate(flags, ui->actionFastMode->isChecked()))
    {
        addToHistory(flags);
        return true;
    }*/
    return false;
}

bool MainWindow::rotate(const QList<int>& flagsList)
{
    /*if(_cube->rotate(flagsList, ui->actionFastMode->isChecked()))
    {
        addToHistory(flagsList);
        return true;
    }*/
    return false;
}

void MainWindow::setModel(VRCModel *model)
{
    /*_history.clear();
    _currentCommand = -1;
    ui->textEditHistory->clear();

    _cube = cube;
    ui->openGLWidget->setCube(_cube);
    ui->control_nbLayer->setMaximum(_cube->maxLayer());*/

    _model = model;

    if(_view == nullptr)
    {
        _view = new VRCView(*_model);
        ui->openGLWidget->setView(_view);
        _model->setView(_view);
    }
    else
        _view->update(*model);

    if(_controller == nullptr)
        _controller = new VRCController(model);
    else
        _controller->setModel(model);
}

QList<int> MainWindow::getCommands(const QString& expression)
{
    /*QRegularExpression re("(\\d*)(F|B|R|L|U|D|x|y|z|M|E|H|S)?(f|b|r|l|u|d)?(w?)((?:2|')?)");
    auto iter = re.globalMatch(expression);
    QList<int> commands;

    while(iter.hasNext())
    {
        auto match = iter.next();
        QString nbLayer = match.captured(1);
        QString face = match.captured(2);
        QString faceSmall = match.captured(3);
        QString wide = match.captured(4);
        QString turn = match.captured(5);

        int flags = 0;
        bool hasBigFace = face.size() > 0;
        if(hasBigFace)
        {
            switch(face[0].toLatin1())
            {
            case LayerMain::L_Up:
                flags |= RotationComponent::Up;
                break;
            case LayerMain::L_Down:
                flags |= RotationComponent::Down;
                break;
            case LayerMain::L_Front:
                flags |= RotationComponent::Front;
                break;
            case LayerMain::L_Back:
                flags |= RotationComponent::Back;
                break;
            case LayerMain::L_Left:
                flags |= RotationComponent::Left;
                break;
            case LayerMain::L_Right:
                flags |= RotationComponent::Right;
                break;
            case LayerMain::L_Equator:
                flags |= RotationComponent::Equator;
                break;
            case LayerMain::L_Middle:
                flags |= RotationComponent::Middle;
                break;
            case LayerMain::L_Standing:
                flags |= RotationComponent::Standing;
                break;
            case LayerMain::L_CubeX:
                flags |= RotationComponent::CubeX;
                break;
            case LayerMain::L_CubeY:
                flags |= RotationComponent::CubeY;
                break;
            case LayerMain::L_CubeZ:
                flags |= RotationComponent::CubeZ;
                break;
            }
        }

        int currentNbLayer = 1;
        if(faceSmall.size() > 0)
        {
            if(hasBigFace)
            {
                if(faceSmall[0].toUpper() == face[0])
                    flags |= RotationComponent::Wide;
            }
            else
            {
                switch(faceSmall[0].toLatin1())
                {
                case LayerSub::Li_Up:
                    flags |= RotationComponent::Up;
                    break;
                case LayerSub::Li_Down:
                    flags |= RotationComponent::Down;
                    break;
                case LayerSub::Li_Front:
                    flags |= RotationComponent::Front;
                    break;
                case LayerSub::Li_Back:
                    flags |= RotationComponent::Back;
                    break;
                case LayerSub::Li_Left:
                    flags |= RotationComponent::Left;
                    break;
                case LayerSub::Li_Right:
                    flags |= RotationComponent::Right;
                    break;
                }
            }
            currentNbLayer = 2;
        }

        if(nbLayer.isEmpty())
            flags += currentNbLayer;
        else
            flags += QVariant(nbLayer).toInt();

        if(!wide.isEmpty() && wide[0] == SYMBOL_WIDE)
        {
            flags |= RotationComponent::Wide;
        }

        if(turn.isEmpty())
            flags |= RotationComponent::Clockwise;
        else if(turn[0] == SYMBOL_COUNTER_CLOCKWISE)
            flags |= RotationComponent::CounterClockwise;
        else
            flags |= RotationComponent::Turn180;

        commands.append(flags);
    }

    return commands;*/
}

void MainWindow::addToHistory(int flags)
{
    /*if(_currentCommand < _history.size() - 1)
    {
        int toRemove = _history.size() - _currentCommand - 1;
        for(int i = 0; i < toRemove; i++)
            _history.removeLast();
    }
    _history.append(flags);
    _currentCommand = _history.size() - 1;
    ui->actionUndo->setEnabled(true);
    ui->actionRedo->setEnabled(false);*/
}

void MainWindow::addToHistory(const QList<int>& flagsList)
{
    /*if(_currentCommand < _history.size() - 1)
    {
        int toRemove = _history.size() - _currentCommand - 1;
        for(int i = 0; i < toRemove; i++)
            _history.removeLast();
    }

    foreach(auto flags, flagsList)
        _history.append(flags);

    _currentCommand = _history.size() - 1;
    ui->actionUndo->setEnabled(true);
    ui->actionRedo->setEnabled(false);*/
}

void MainWindow::moveEvent(QMoveEvent *)
{
    if(!(this->windowState() & Qt::WindowMaximized))
    {
        _settings.setValue(SETTINGS_KEY_WINDOW_RECT, this->geometry());
    }
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    _settings.setValue(SETTINGS_KEY_WINDOW_RECT, this->geometry());
}

void MainWindow::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::WindowStateChange)
    {
        Qt::WindowStates state = this->windowState();
        _settings.setValue(SETTINGS_KEY_WINDOW_STATE, (uint)state);
        if(state & Qt::WindowMaximized)
            _settings.setValue(SETTINGS_KEY_WINDOW_RECT, _sizeBeforeMaximize);
    }
}

void MainWindow::newCube()
{
    /*bool ok = false;
    int size = QInputDialog::getInt(this, tr("New Cube"), tr("Cube Size"), _cube->size(), 2, 100, 1, &ok);
    if(ok)
    {
        delete _cube;
        setCube(new VirtualRubiksCube(ui->openGLWidget, size));
        _settings.setValue(SETTINGS_KEY_SIZE, size);
    }*/
}

void MainWindow::save()
{
    /*QString filename = QFileDialog::getSaveFileName(this, tr("Save Cube"), _settings.value(SETTINGS_KEY_SAVE, QDir::currentPath()).toString(), tr("Text files (*.txt)"));
    if(!filename.isEmpty())
    {
        QFileInfo info(filename);
        _settings.setValue(SETTINGS_KEY_SAVE, info.absoluteDir().absolutePath());

        QFile file(filename);
        if(info.exists())
            file.remove();

        if(!file.open(QIODevice::WriteOnly |  QIODevice::Text))
            return;

        QTextStream out(&file);
        out << "cubesize=" << QVariant(_cube->size()).toString() << "\n";
        out << ui->textEditHistory->toPlainText();
        file.close();
    }*/
}

void MainWindow::load()
{
    /*QString filename = QFileDialog::getOpenFileName(this, tr("Open Cube"), _settings.value(SETTINGS_KEY_SAVE, QDir::currentPath()).toString(), tr("Text files (*.txt)"));
    if(!filename.isEmpty())
    {
        QFileInfo info(filename);
        if(info.exists())
        {
            _settings.setValue(SETTINGS_KEY_SAVE, info.absoluteDir().absolutePath());
            ui->textEditHistory->clear();
            ui->textEditActions->clear();
            QFile file(filename);
            if(!file.open(QIODevice::ReadOnly |  QIODevice::Text))
                return;

            QTextStream in(&file);
            QString sizeStr = file.readLine();
            if(sizeStr.contains("cubesize="))
            {
                int size = sizeStr.remove("cubesize=").toInt();
                if(size >= 2 && size <= 100)
                    setCube(new VirtualRubiksCube(ui->openGLWidget, size));
            }
            else
            {
                ui->textEditActions->append(sizeStr);
            }

            while(!in.atEnd())
            {
                ui->textEditActions->append(in.readLine());
            }
            file.close();
        }
    }*/
}

void MainWindow::reset()
{
    /*int size = _cube->size();
    delete _cube;
    setCube(new VirtualRubiksCube(ui->openGLWidget, size));*/
}

void MainWindow::undo()
{
    /*if(_currentCommand >= 0 && _history.size() > 0)
    {
        ui->actionRedo->setEnabled(true);
        int flags = _history[_currentCommand--];
        if(flags & RotationComponent::Clockwise)
        {
            flags ^= RotationComponent::Clockwise;
            flags |= RotationComponent::CounterClockwise;
        }
        else if(flags & RotationComponent::CounterClockwise)
        {
            flags ^= RotationComponent::CounterClockwise;
            flags |= RotationComponent::Clockwise;
        }
        if(!_cube->rotate(flags))
        {

        }
    }
    else
        ui->actionUndo->setEnabled(false);*/
}

void MainWindow::redo()
{
    /*if(_currentCommand < _history.size() - 1)
    {
        ui->actionUndo->setEnabled(true);
        _cube->rotate(_history[_currentCommand++]);
    }
    else
    {
        ui->actionRedo->setEnabled(false);
    }*/
}

void MainWindow::fastmode(bool activated)
{
    _settings.setValue(SETTINGS_KEY_FAST_MODE, activated);
}

void MainWindow::about()
{
    _about.exec();
}

void MainWindow::resetSettings()
{
    _settings.clear();
    loadSettings();
}

void MainWindow::loadSettings()
{
    ui->actionFastMode->setChecked(_settings.value(SETTINGS_KEY_FAST_MODE, false).toBool());
    setGeometry(_settings.value(SETTINGS_KEY_WINDOW_RECT,
                                QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->minimumSize(), QGuiApplication::screens().first()->geometry())).toRect());

    _sizeBeforeMaximize = this->geometry();
    setWindowState((Qt::WindowState)_settings.value(SETTINGS_KEY_WINDOW_STATE, Qt::WindowNoState).toUInt());
}

void MainWindow::loadStyle()
{
    QString appDir = qApp->applicationDirPath();

    _advancedStylesheet.setStylesDirPath(appDir + "/styles");
    _advancedStylesheet.setOutputDirPath(appDir + "/output");

    _advancedStylesheet.setCurrentStyle("qt_material");
    _advancedStylesheet.setCurrentTheme("dark_white_orange");
    _advancedStylesheet.updateStylesheet();

    QString stylesheet = _advancedStylesheet.styleSheet();

    qApp->setStyleSheet(stylesheet);
}

void MainWindow::reverse()
{
    auto commands = getCommands(ui->textEditActions->toPlainText());
    std::reverse(std::begin(commands), std::end(commands));
    ui->textEditActions->clear();
}

void MainWindow::execute()
{
    auto commands = getCommands(ui->textEditActions->toPlainText());
    rotate(commands);

    ui->textEditHistory->append(ui->textEditActions->toPlainText());
}

void MainWindow::scramble()
{
    /*ui->tabWidget->setCurrentIndex(0);
    ui->textEditActions->clear();
    QString expr;

    QList<char> mainFaces;
    mainFaces.append(LayerMain::L_Up);
    mainFaces.append(LayerMain::L_Down);
    mainFaces.append(LayerMain::L_Left);
    mainFaces.append(LayerMain::L_Right);
    mainFaces.append(LayerMain::L_Front);
    mainFaces.append(LayerMain::L_Back);

    QList<QString> rotations;
    rotations.append("");
    rotations.append(QString(SYMBOL_COUNTER_CLOCKWISE));
    rotations.append(QString(SYMBOL_180));

    char previous = ' ';

    for(int i = 0; i < ui->spinBoxScrambleLength->value(); i++)
    {
        QString el;
        char face = ' ';
        do
        {
            face = mainFaces[QRandomGenerator::global()->generate() % mainFaces.size()];
        }while(previous == face);
        el = face;
        previous = face;

        bool wide = QRandomGenerator::global()->generateDouble() >= 0.5;

        int layerN = QRandomGenerator::global()->generate() % _cube->maxLayer() + 1;

        if(layerN == 2)
        {
            if(wide)
                el.append(el.toLower());
            else
                el = el.toLower();
        }
        else if(layerN > 2)
        {
            el.insert(0, QVariant(layerN).toString());
            if(wide)
                el.append(SYMBOL_WIDE);
        }

        el.append(rotations[QRandomGenerator::global()->generate() % rotations.size()]);
        expr += el + " ";
    }

    ui->textEditActions->setPlainText(expr);*/
}

void MainWindow::solve()
{

}

void MainWindow::rotateWithControls(Rotation rotation)
{
    Layer layer = Layer::Front;
    if(ui->control_B->isChecked())
    {
        layer = Layer::Back;
    }
    else if(ui->control_F->isChecked())
    {
        layer = Layer::Front;
    }
    else if(ui->control_D->isChecked())
    {
        layer = Layer::Down;
    }
    else if(ui->control_U->isChecked())
    {
        layer = Layer::Up;
    }
    else if(ui->control_L->isChecked())
    {
        layer = Layer::Left;
    }
    else if(ui->control_R->isChecked())
    {
        layer = Layer::Right;
    }
    else if(ui->control_E->isChecked())
    {
        layer = Layer::Equator;
    }
    else if(ui->control_M->isChecked())
    {
        layer = Layer::Middle;
    }
    else if(ui->control_S->isChecked())
    {
        layer = Layer::Standing;
    }
    else if(ui->control_x->isChecked())
    {
        layer = Layer::CubeX;
    }
    else if(ui->control_y->isChecked())
    {
        layer = Layer::CubeY;
    }
    else if(ui->control_z->isChecked())
    {
        layer = Layer::CubeZ;
    }

    ushort layerNumber = ui->control_nbLayer->value();
    auto option = ui->control_w->isChecked() ? Option::Wide : Option::None;

    _controller->execute(VRCAction(layer, option, rotation, layerNumber));
}

void MainWindow::rotateClockwise()
{
    rotateWithControls(Rotation::Clockwise);
}

void MainWindow::rotateCounterClockwise()
{
    rotateWithControls(Rotation::CounterClockwise);
}

void MainWindow::rotateTurn180()
{
    rotateWithControls(Rotation::Turn180);
}

void MainWindow::uncheckLayerControls(QPushButton* exception)
{
    foreach(auto obj, _layerControls)
    {
        if(obj == exception) continue;

        obj->setChecked(false);
    }
}

void MainWindow::pushF(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_F);
}

void MainWindow::pushB(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_B);
}

void MainWindow::pushL(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_L);
}

void MainWindow::pushR(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_R);
}

void MainWindow::pushU(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_U);
}

void MainWindow::pushD(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_D);
}

void MainWindow::pushE(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_E);
}

void MainWindow::pushM(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_M);
}
void MainWindow::pushS(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_S);
}

void MainWindow::pushx(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_x);
}

void MainWindow::pushy(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_y);
}

void MainWindow::pushz(bool checked)
{
    if(!checked) return;
    uncheckLayerControls(ui->control_z);
}

void MainWindow::nbLChanged(int value)
{
    /*if(value == 1)
    {
        ui->control_w->setChecked(false);
        foreach(auto obj, _subLayerControls)
        {
            obj->setChecked(false);
        }
    }*/
}
