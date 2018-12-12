#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QRegularExpression>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(newCube()));
    connect(ui->actionReset, SIGNAL(triggered(bool)), ui->pushButtonReset, SLOT(click()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), ui->pushButtonSave, SLOT(click()));
    connect(ui->actionLoad, SIGNAL(triggered(bool)), ui->pushButtonLoad, SLOT(click()));

    connect(ui->pushButtonSave, SIGNAL(clicked(bool)), this, SLOT(save()));
    connect(ui->pushButtonLoad, SIGNAL(clicked(bool)), this, SLOT(load()));
    connect(ui->pushButtonReset, SIGNAL(clicked(bool)), this, SLOT(reset()));
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
    connect(ui->control_x, SIGNAL(toggled(bool)), this, SLOT(pushX(bool)));
    connect(ui->control_y, SIGNAL(toggled(bool)), this, SLOT(pushY(bool)));
    connect(ui->control_z, SIGNAL(toggled(bool)), this, SLOT(pushZ(bool)));

    connect(ui->control_f, SIGNAL(toggled(bool)), this, SLOT(pushf(bool)));
    connect(ui->control_b, SIGNAL(toggled(bool)), this, SLOT(pushb(bool)));
    connect(ui->control_l, SIGNAL(toggled(bool)), this, SLOT(pushl(bool)));
    connect(ui->control_r, SIGNAL(toggled(bool)), this, SLOT(pushr(bool)));
    connect(ui->control_u, SIGNAL(toggled(bool)), this, SLOT(pushu(bool)));
    connect(ui->control_d, SIGNAL(toggled(bool)), this, SLOT(pushd(bool)));

    _groupA.append(ui->control_F);
    _groupA.append(ui->control_B);
    _groupA.append(ui->control_L);
    _groupA.append(ui->control_R);
    _groupA.append(ui->control_U);
    _groupA.append(ui->control_D);
    _groupA.append(ui->control_E);
    _groupA.append(ui->control_M);
    _groupA.append(ui->control_S);
    _groupA.append(ui->control_x);
    _groupA.append(ui->control_y);
    _groupA.append(ui->control_z);

    _groupB.append(ui->control_f);
    _groupB.append(ui->control_b);
    _groupB.append(ui->control_l);
    _groupB.append(ui->control_r);
    _groupB.append(ui->control_u);
    _groupB.append(ui->control_d);

    int size = 3;
    ui->control_nbLayer->setMaximum(size / 2);

    _cube = new RubiksCube(size);
    ui->openGLWidget->setCube(_cube);

    _currentCommand = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _cube;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    ui->openGLWidget->keyPressEvent(event);
}

void MainWindow::addCommands(const QString& expression)
{
    QRegularExpression re("(\\d*)(F|B|R|L|U|D|x|y|z|M|E|H|S)?(f|b|r|l|u|d)?(w?)((?:2|')?)");
    auto iter = re.globalMatch(expression);

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
            case LayerOut::L_Up:
                flags |= RotationComponent::Up;
                break;
            case LayerOut::L_Down:
                flags |= RotationComponent::Down;
                break;
            case LayerOut::L_Front:
                flags |= RotationComponent::Front;
                break;
            case LayerOut::L_Back:
                flags |= RotationComponent::Back;
                break;
            case LayerOut::L_Left:
                flags |= RotationComponent::Left;
                break;
            case LayerOut::L_Right:
                flags |= RotationComponent::Right;
                break;
            case LayerOut::L_Equator:
                flags |= RotationComponent::Equator;
                break;
            case LayerOut::L_Horizontal:
                flags |= RotationComponent::Horizontal;
                break;
            case LayerOut::L_Middle:
                flags |= RotationComponent::Middle;
                break;
            case LayerOut::L_Standing:
                flags |= RotationComponent::Standing;
                break;
            case LayerOut::L_CubeX:
                flags |= RotationComponent::CubeX;
                break;
            case LayerOut::L_CubeY:
                flags |= RotationComponent::CubeY;
                break;
            case LayerOut::L_CubeZ:
                flags |= RotationComponent::CubeZ;
                break;
            }
        }

        int currentNbLayer = 1;
        if(faceSmall.size() > 0)
        {
            if(hasBigFace)
            {
                if(face.size() == 2 && face[1].toUpper() == face[0])
                    flags |= RotationComponent::Wide;
            }
            else
            {
                switch(faceSmall[0].toLatin1())
                {
                case LayerIn::Li_Up:
                    flags |= RotationComponent::Up;
                    break;
                case LayerIn::Li_Down:
                    flags |= RotationComponent::Down;
                    break;
                case LayerIn::Li_Front:
                    flags |= RotationComponent::Front;
                    break;
                case LayerIn::Li_Back:
                    flags |= RotationComponent::Back;
                    break;
                case LayerIn::Li_Left:
                    flags |= RotationComponent::Left;
                    break;
                case LayerIn::Li_Right:
                    flags |= RotationComponent::Right;
                    break;
                }
                currentNbLayer = 2;
            }
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

        _commands.append(flags);
    }
}

void MainWindow::newCube()
{
    bool ok = false;
    int size = QInputDialog::getInt(this, tr("New Cube"), tr("Cube Size"), _cube->size(), 2, 100, 1, &ok);
    if(ok)
    {
        delete _cube;
        _cube = new RubiksCube(size);
        ui->openGLWidget->setCube(_cube);
    }
}

void MainWindow::save()
{

}

void MainWindow::load()
{

}

void MainWindow::reset()
{
    int size = _cube->size();
    delete _cube;
    _cube = new RubiksCube(size);
    ui->openGLWidget->setCube(_cube);
}

void MainWindow::execute()
{
    _commands.clear();
    addCommands(ui->textEditActions->toPlainText());
    _cube->rotate(_commands);
}

void MainWindow::scramble()
{

}

void MainWindow::solve()
{

}

void MainWindow::rotateWithControls(int flags)
{
    if(ui->control_B->isChecked())
    {
        flags |= RotationComponent::Back;
    }
    else if(ui->control_F->isChecked())
    {
        flags |= RotationComponent::Front;
    }
    else if(ui->control_D->isChecked())
    {
        flags |= RotationComponent::Down;
    }
    else if(ui->control_U->isChecked())
    {
        flags |= RotationComponent::Up;
    }
    else if(ui->control_L->isChecked())
    {
        flags |= RotationComponent::Left;
    }
    else if(ui->control_R->isChecked())
    {
        flags |= RotationComponent::Right;
    }
    else if(ui->control_E->isChecked())
    {
        flags |= RotationComponent::Equator;
    }
    else if(ui->control_M->isChecked())
    {
        flags |= RotationComponent::Middle;
    }
    else if(ui->control_S->isChecked())
    {
        flags |= RotationComponent::Standing;
    }
    else if(ui->control_x->isChecked())
    {
        flags |= RotationComponent::CubeX;
    }
    else if(ui->control_y->isChecked())
    {
        flags |= RotationComponent::CubeY;
    }
    else if(ui->control_z->isChecked())
    {
        flags |= RotationComponent::CubeZ;
    }

    flags += ui->control_nbLayer->value();

    if(ui->control_w->isChecked())
        flags |= RotationComponent::Wide;

    _cube->rotate(flags);
}

void MainWindow::rotateClockwise()
{
    rotateWithControls(RotationComponent::Clockwise);
}

void MainWindow::rotateCounterClockwise()
{
    rotateWithControls(RotationComponent::CounterClockwise);
}

void MainWindow::rotateTurn180()
{
    rotateWithControls(RotationComponent::Turn180);
}

void MainWindow::pushF(bool checked)
{
    if(!checked)
    {
        ui->control_f->setChecked(false);
        return;
    }

    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_F)
            obj->setChecked(false);
    }
}

void MainWindow::pushB(bool checked)
{
    if(!checked)
    {
        ui->control_b->setChecked(false);
        return;
    }

    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_B)
            obj->setChecked(false);
    }
}

void MainWindow::pushL(bool checked)
{
    if(!checked)
    {
        ui->control_l->setChecked(false);
        return;
    }

    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_L)
            obj->setChecked(false);
    }
}

void MainWindow::pushR(bool checked)
{
    if(!checked)
    {
        ui->control_r->setChecked(false);
        return;
    }

    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_R)
            obj->setChecked(false);
    }
}

void MainWindow::pushU(bool checked)
{
    if(!checked)
    {
        ui->control_u->setChecked(false);
        return;
    }

    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_U)
            obj->setChecked(false);
    }
}

void MainWindow::pushD(bool checked)
{
    if(!checked)
    {
        ui->control_d->setChecked(false);
        return;
    }

    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_D)
            obj->setChecked(false);
    }
}

void MainWindow::pushE(bool checked)
{
    if(!checked) return;
    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_E)
            obj->setChecked(false);
    }
}

void MainWindow::pushM(bool checked)
{
    if(!checked) return;
    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_M)
            obj->setChecked(false);
    }
}
void MainWindow::pushS(bool checked)
{
    if(!checked) return;
    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_S)
            obj->setChecked(false);
    }
}

void MainWindow::pushX(bool checked)
{
    if(!checked) return;
    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_x)
            obj->setChecked(false);
    }
}

void MainWindow::pushY(bool checked)
{
    if(!checked) return;
    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_y)
            obj->setChecked(false);
    }
}

void MainWindow::pushZ(bool checked)
{
    if(!checked) return;
    foreach(auto obj, _groupA)
    {
        if(obj != ui->control_z)
            obj->setChecked(false);
    }
}

void MainWindow::pushf(bool checked)
{
    if(!checked)
    {
        ui->control_w->setChecked(false);
        ui->control_nbLayer->setValue(1);
        return;
    }

    foreach(auto obj, _groupB)
    {
        if(obj != ui->control_f)
            obj->setChecked(false);
    }
    ui->control_F->setChecked(true);
    ui->control_w->setChecked(true);
    ui->control_nbLayer->setValue(2);
}

void MainWindow::pushb(bool checked)
{
    if(!checked)
    {
        ui->control_w->setChecked(false);
        ui->control_nbLayer->setValue(1);
        return;
    }

    foreach(auto obj, _groupB)
    {
        if(obj != ui->control_b)
            obj->setChecked(false);
    }
    ui->control_B->setChecked(true);
    ui->control_w->setChecked(true);
    ui->control_nbLayer->setValue(2);
}

void MainWindow::pushl(bool checked)
{
    if(!checked)
    {
        ui->control_w->setChecked(false);
        ui->control_nbLayer->setValue(1);
        return;
    }

    foreach(auto obj, _groupB)
    {
        if(obj != ui->control_l)
            obj->setChecked(false);
    }
    ui->control_L->setChecked(true);
    ui->control_w->setChecked(true);
    ui->control_nbLayer->setValue(2);
}

void MainWindow::pushr(bool checked)
{
    if(!checked)
    {
        ui->control_w->setChecked(false);
        ui->control_nbLayer->setValue(1);
        return;
    }

    foreach(auto obj, _groupB)
    {
        if(obj != ui->control_r)
            obj->setChecked(false);
    }
    ui->control_R->setChecked(true);
    ui->control_w->setChecked(true);
    ui->control_nbLayer->setValue(2);
}

void MainWindow::pushu(bool checked)
{
    if(!checked)
    {
        ui->control_w->setChecked(false);
        ui->control_nbLayer->setValue(1);
        return;
    }

    foreach(auto obj, _groupB)
    {
        if(obj != ui->control_u)
            obj->setChecked(false);
    }
    ui->control_U->setChecked(true);
    ui->control_w->setChecked(true);
    ui->control_nbLayer->setValue(2);
}

void MainWindow::pushd(bool checked)
{
    if(!checked)
    {
        ui->control_w->setChecked(false);
        ui->control_nbLayer->setValue(1);
        return;
    }

    foreach(auto obj, _groupB)
    {
        if(obj != ui->control_d)
            obj->setChecked(false);
    }
    ui->control_D->setChecked(true);
    ui->control_w->setChecked(true);
    ui->control_nbLayer->setValue(2);
}

void MainWindow::nbLChanged(int value)
{
    if(value == 1)
    {
        ui->control_w->setChecked(false);
        foreach(auto obj, _groupB)
        {
            obj->setChecked(false);
        }
    }
}
