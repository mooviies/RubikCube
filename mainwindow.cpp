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
#include "vrcparser.h"

typedef VRCAction::Rotation Rotation;
typedef VRCAction::Layer Layer;
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
    connect(ui->actionUndo, SIGNAL(triggered(bool)), this, SLOT(undo()));
    connect(ui->actionRedo, SIGNAL(triggered(bool)), this, SLOT(redo()));

    connect(ui->pushButtonNew, SIGNAL(clicked(bool)), this, SLOT(newCube()));
    connect(ui->pushButtonReset, SIGNAL(clicked(bool)), this, SLOT(reset()));
    connect(ui->pushButtonReverse, SIGNAL(clicked(bool)), this, SLOT(reverse()));
    connect(ui->pushButtonExecute, SIGNAL(clicked(bool)), this, SLOT(execute()));
    connect(ui->pushButtonScramble, SIGNAL(clicked(bool)), this, SLOT(scramble()));
    connect(ui->pushButtonSolve, SIGNAL(clicked(bool)), this, SLOT(solve()));
    connect(ui->control_clockwise, SIGNAL(clicked(bool)), this, SLOT(rotateClockwise()));
    connect(ui->control_counter, SIGNAL(clicked(bool)), this, SLOT(rotateCounterClockwise()));
    connect(ui->control_turn180, SIGNAL(clicked(bool)), this, SLOT(rotateTurn180()));

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

    _model = nullptr;
    _view = nullptr;
    _controller = nullptr;
    _timer = new QTimer(this);
    setModel(new VRCModel(_settings.value(SETTINGS_KEY_SIZE, 3).toInt()));
    loadSettings();
    loadStyle();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _model;
    delete _view;
    delete _timer;
    delete _controller;
}

void MainWindow::setModel(VRCModel *model)
{
    ui->textEditHistory->clear();
    ui->control_nbLayer->setMaximum(model->getMaxLayerNumber());

    if(_model != nullptr)
        delete _model;
    _model = model;

    if(_view != nullptr)
        delete _view;

    _view = new VRCView(_model);
    ui->openGLWidget->setView(_view);
    _view->setModel(_model);
    _model->setView(_view);

    if(_controller == nullptr)
    {
        _controller = new VRCController(_model, _view);
        connect(_timer, SIGNAL(timeout()), this, SLOT(updateController()));
        _timer->start();
    }
    else
    {
        _controller->setModelView(_model, _view);
    }
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
    bool ok = false;
    int size = QInputDialog::getInt(this, tr("New Cube"), tr("Cube Size"), _model->getSize(), 2, 100, 1, &ok);
    if(ok)
    {
        setModel(new VRCModel(size));
        _settings.setValue(SETTINGS_KEY_SIZE, size);
    }
}

void MainWindow::save()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Cube"), _settings.value(SETTINGS_KEY_SAVE, QDir::currentPath()).toString(), tr("Text files (*.txt)"));
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
        out << "cubesize=" << QVariant(_model->getSize()).toString() << "\n";
        out << ui->textEditHistory->toPlainText();
        file.close();
    }
}

void MainWindow::load()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Cube"), _settings.value(SETTINGS_KEY_SAVE, QDir::currentPath()).toString(), tr("Text files (*.txt)"));
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
                    setModel(new VRCModel(size));
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
    }
}

void MainWindow::reset()
{
    int size = _model->getSize();
    delete _model;
    setModel(new VRCModel(size));
}

void MainWindow::undo()
{
    _controller->undo();
}

void MainWindow::redo()
{
    _controller->redo();
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
    auto actions = VRCParser::parse(ui->textEditActions->toPlainText());
    std::reverse(actions.begin(), actions.end());
    _controller->execute(actions);
}

void MainWindow::execute()
{
    _controller->execute(VRCParser::parse(ui->textEditActions->toPlainText()));
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

void MainWindow::updateController()
{
    _controller->update();
}
