#include "vrcparser.h"

#include <QRegularExpression>

#include "vrcaction.h"
#include "constants.h"

typedef VRCAction::Layer Layer;
typedef VRCAction::Option Option;
typedef VRCAction::Rotation Rotation;

QRegularExpression VRCParser::_regularExpression("(\\d*)(F|B|R|L|U|D|x|y|z|M|E|H|S)?(f|b|r|l|u|d)?(w?)((?:2|')?)\\s*");

VRCParser::VRCParser()
{

}

QList<VRCAction> VRCParser::parse(const QString& expression)
{
    auto iter = _regularExpression.globalMatch(expression);
    QList<VRCAction> actions;

    while(iter.hasNext())
    {
        auto match = iter.next();

        QString nbLayer = match.captured(1);
        QString face = match.captured(2);
        QString faceSmall = match.captured(3);
        QString wide = match.captured(4);
        QString turn = match.captured(5);

        if(nbLayer.isEmpty() && face.isEmpty() && faceSmall.isEmpty() && wide.isEmpty() && turn.isEmpty())
            continue;

        uint flags = 0;
        bool hasBigFace = face.size() > 0;
        if(hasBigFace)
        {
            switch((LayerMain)face[0].toLatin1())
            {
            case LayerMain::Up:
                flags |= (uint)Layer::Up;
                break;
            case LayerMain::Down:
                flags |= (uint)Layer::Down;
                break;
            case LayerMain::Front:
                flags |= (uint)Layer::Front;
                break;
            case LayerMain::Back:
                flags |= (uint)Layer::Back;
                break;
            case LayerMain::Left:
                flags |= (uint)Layer::Left;
                break;
            case LayerMain::Right:
                flags |= (uint)Layer::Right;
                break;
            case LayerMain::Equator:
                flags |= (uint)Layer::Equator;
                break;
            case LayerMain::Middle:
                flags |= (uint)Layer::Middle;
                break;
            case LayerMain::Standing:
                flags |= (uint)Layer::Standing;
                break;
            case LayerMain::CubeX:
                flags |= (uint)Layer::CubeX;
                break;
            case LayerMain::CubeY:
                flags |= (uint)Layer::CubeY;
                break;
            case LayerMain::CubeZ:
                flags |= (uint)Layer::CubeZ;
                break;
            }
        }

        int currentNbLayer = 1;
        if(faceSmall.size() > 0)
        {
            if(hasBigFace)
            {
                if(faceSmall[0].toUpper() == face[0])
                    flags |= (uint)Option::Wide;
            }
            else
            {
                switch((LayerSub)faceSmall[0].toLatin1())
                {
                case LayerSub::Up:
                    flags |= (uint)Layer::Up;
                    break;
                case LayerSub::Down:
                    flags |= (uint)Layer::Down;
                    break;
                case LayerSub::Front:
                    flags |= (uint)Layer::Front;
                    break;
                case LayerSub::Back:
                    flags |= (uint)Layer::Back;
                    break;
                case LayerSub::Left:
                    flags |= (uint)Layer::Left;
                    break;
                case LayerSub::Right:
                    flags |= (uint)Layer::Right;
                    break;
                }
            }
            currentNbLayer = 2;
        }

        if(nbLayer.isEmpty())
            flags |= currentNbLayer << VRCAction::LAYER_MASK_SHIFT;
        else
            flags |= QVariant(nbLayer).toUInt() << VRCAction::LAYER_MASK_SHIFT;

        if(!wide.isEmpty() && wide[0] == SYMBOL_WIDE)
        {
            flags |= (uint)Option::Wide;
        }

        if(turn.isEmpty())
            flags |= (uint)Rotation::Clockwise;
        else if(turn[0] == SYMBOL_COUNTER_CLOCKWISE)
            flags |= (uint)Rotation::CounterClockwise;
        else
            flags |= (uint)Rotation::Turn180;

        actions.append(VRCAction(flags));
    }

    return actions;
}

QString VRCParser::toString(VRCAction action)
{
    auto layer = action.getLayer();
    auto option = action.getOption();
    auto rotation = action.getRotation();
    auto layerNumber = action.getLayerNumber();

    QString expression;

    if(layerNumber > 2 && ((uint)layer & (uint)VRCAction::LayerMask::Face))
        expression.append(QVariant::fromValue(layerNumber).toString());

    switch(layer)
    {
        case VRCAction::Layer::Left:
            expression.append(QChar((char)LayerMain::Left));
            break;
        case VRCAction::Layer::Front:
            expression.append(QChar((char)LayerMain::Front));
            break;
        case VRCAction::Layer::Right:
            expression.append(QChar((char)LayerMain::Right));
            break;
        case VRCAction::Layer::Back:
            expression.append(QChar((char)LayerMain::Back));
            break;
        case VRCAction::Layer::Up:
            expression.append(QChar((char)LayerMain::Up));
            break;
        case VRCAction::Layer::Down:
            expression.append(QChar((char)LayerMain::Down));
            break;
        case VRCAction::Layer::Middle:
            expression.append(QChar((char)LayerMain::Middle));
            break;
        case VRCAction::Layer::Equator:
            expression.append(QChar((char)LayerMain::Equator));
            break;
        case VRCAction::Layer::Standing:
            expression.append(QChar((char)LayerMain::Standing));
            break;
        case VRCAction::Layer::CubeX:
            expression.append(QChar((char)LayerMain::CubeX));
            break;
        case VRCAction::Layer::CubeY:
            expression.append(QChar((char)LayerMain::CubeY));
            break;
        case VRCAction::Layer::CubeZ:
            expression.append(QChar((char)LayerMain::CubeZ));
            break;
    }

    if(layerNumber == 2)
    {
        switch(layer)
        {
            case VRCAction::Layer::Left:
                expression.append(QChar((char)LayerSub::Left));
                break;
            case VRCAction::Layer::Front:
                expression.append(QChar((char)LayerSub::Front));
                break;
            case VRCAction::Layer::Right:
                expression.append(QChar((char)LayerSub::Right));
                break;
            case VRCAction::Layer::Back:
                expression.append(QChar((char)LayerSub::Back));
                break;
            case VRCAction::Layer::Up:
                expression.append(QChar((char)LayerSub::Up));
                break;
            case VRCAction::Layer::Down:
                expression.append(QChar((char)LayerSub::Down));
                break;
            default:
                break;
        }
    }
    else if(layerNumber > 2 && option == VRCAction::Option::Wide && ((uint)layer & (uint)VRCAction::LayerMask::Face))
    {
        expression.append(SYMBOL_WIDE);
    }

    switch(rotation)
    {
        case VRCAction::Rotation::Clockwise:
            break;
        case VRCAction::Rotation::CounterClockwise:
            expression.append(SYMBOL_COUNTER_CLOCKWISE);
            break;
        case VRCAction::Rotation::Turn180:
            expression.append(SYMBOL_180);
            break;
    }

    return expression;
}

QList<QString> VRCParser::toString(const QList<VRCAction> &actions)
{
    QList<QString> expressions;
    expressions.reserve(actions.length());

    for(auto action : actions)
        expressions.append(toString(action));

    return expressions;
}
