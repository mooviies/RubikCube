#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QRegularExpression>
#include <QInputDialog>
#include <QRandomGenerator>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _settings("mooviies", "VirtualRubiksCube")
{
    ui->setupUi(this);

    connect(ui->actionNew, SIGNAL(triggered(bool)), ui->pushButtonNew, SLOT(click()));
    connect(ui->actionReset, SIGNAL(triggered(bool)), ui->pushButtonReset, SLOT(click()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(save()));
    connect(ui->actionLoad, SIGNAL(triggered(bool)), this, SLOT(load()));

    connect(ui->pushButtonNew, SIGNAL(clicked(bool)), this, SLOT(newCube()));
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

    setCube(new RubiksCube(_settings.value(SETTINGS_KEY_SIZE, 3).toInt()));

    _currentCommand = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _cube;
}

bool MainWindow::rotate(int flags)
{
    return _cube->rotate(flags, ui->checkBoxFastMode->isChecked());
}

bool MainWindow::rotate(const QList<int>& flagsList)
{
    return _cube->rotate(flagsList, ui->checkBoxFastMode->isChecked());
}

void MainWindow::setCube(RubiksCube *cube)
{
    _history.clear();
    _currentCommand = 0;
    ui->textEditHistory->clear();

    _cube = cube;
    ui->openGLWidget->setCube(_cube);
    ui->control_nbLayer->setMaximum(_cube->maxLayer());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    ui->openGLWidget->keyPressEvent(event);
}

QList<int> MainWindow::getCommands(const QString& expression)
{
    QRegularExpression re("(\\d*)(F|B|R|L|U|D|x|y|z|M|E|H|S)?(f|b|r|l|u|d)?(w?)((?:2|')?)");
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
                if(faceSmall[0].toUpper() == face[0])
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

    return commands;
}

void MainWindow::newCube()
{
    bool ok = false;
    int size = QInputDialog::getInt(this, tr("New Cube"), tr("Cube Size"), _cube->size(), 2, 100, 1, &ok);
    if(ok)
    {
        delete _cube;
        setCube(new RubiksCube(size));
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
        out << "cubesize=" << QVariant(_cube->size()).toString() << "\n";
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
                    setCube(new RubiksCube(size));
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
    int size = _cube->size();
    delete _cube;
    setCube(new RubiksCube(size));
}

void MainWindow::execute()
{
    auto commands = getCommands(ui->textEditActions->toPlainText());
    rotate(commands);

    ui->textEditHistory->append(ui->textEditActions->toPlainText());
}

void MainWindow::scramble()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->textEditActions->clear();
    QString expr;

    QList<char> mainFaces;
    mainFaces.append(LayerOut::L_Up);
    mainFaces.append(LayerOut::L_Down);
    mainFaces.append(LayerOut::L_Left);
    mainFaces.append(LayerOut::L_Right);
    mainFaces.append(LayerOut::L_Front);
    mainFaces.append(LayerOut::L_Back);

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

    ui->textEditActions->setText(expr);
}

void MainWindow::solve()
{

}

void MainWindow::rotateWithControls(int flags)
{
    QString expr;
    if(ui->control_B->isChecked())
    {
        flags |= RotationComponent::Back;
        expr = LayerOut::L_Back;
    }
    else if(ui->control_F->isChecked())
    {
        flags |= RotationComponent::Front;
        expr = LayerOut::L_Front;
    }
    else if(ui->control_D->isChecked())
    {
        flags |= RotationComponent::Down;
        expr = LayerOut::L_Down;
    }
    else if(ui->control_U->isChecked())
    {
        flags |= RotationComponent::Up;
        expr = LayerOut::L_Up;
    }
    else if(ui->control_L->isChecked())
    {
        flags |= RotationComponent::Left;
        expr = LayerOut::L_Left;
    }
    else if(ui->control_R->isChecked())
    {
        flags |= RotationComponent::Right;
        expr = LayerOut::L_Right;
    }
    else if(ui->control_E->isChecked())
    {
        flags |= RotationComponent::Equator;
        expr = LayerOut::L_Equator;
    }
    else if(ui->control_M->isChecked())
    {
        flags |= RotationComponent::Middle;
        expr = LayerOut::L_Middle;
    }
    else if(ui->control_S->isChecked())
    {
        flags |= RotationComponent::Standing;
        expr = LayerOut::L_Standing;
    }
    else if(ui->control_x->isChecked())
    {
        flags |= RotationComponent::CubeX;
        expr = LayerOut::L_CubeX;
    }
    else if(ui->control_y->isChecked())
    {
        flags |= RotationComponent::CubeY;
        expr = LayerOut::L_CubeY;
    }
    else if(ui->control_z->isChecked())
    {
        flags |= RotationComponent::CubeZ;
        expr = LayerOut::L_CubeZ;
    }

    int nbLayer = ui->control_nbLayer->value();
    flags += nbLayer;

    if(ui->control_w->isChecked())
    {
        flags |= RotationComponent::Wide;
        if(!(flags & (RotationComponent::CenterLayers | RotationComponent::WholeCube)))
        {
            if(nbLayer == 2)
                expr.append(expr.toLower());
            else if(nbLayer > 2)
            {
                expr.insert(0, QVariant(nbLayer).toString());
                expr.append(SYMBOL_WIDE);
            }
        }
    }
    else
    {
        if(!(flags & RotationComponent::CenterLayers))
        {
            if(nbLayer > 2)
            {
                expr.insert(0, QVariant(nbLayer).toString());
            }
            else if(nbLayer == 2)
                expr = expr.toLower();
        }
    }

    if(flags & RotationComponent::CounterClockwise)
        expr.append(SYMBOL_COUNTER_CLOCKWISE);
    else if(flags & RotationComponent::Turn180)
        expr.append(SYMBOL_180);

    if(rotate(flags))
        ui->textEditHistory->append(expr);
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
