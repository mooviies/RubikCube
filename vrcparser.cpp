#include "vrcparser.h"

#include <QRegularExpression>

#include "vrcaction.h"
#include "constants.h"

typedef VRCAction::Layer Layer;
typedef VRCAction::Option Option;
typedef VRCAction::Rotation Rotation;

VRCParser::VRCParser()
{

}

QList<VRCAction> VRCParser::parse(const QString& expression)
{
    QRegularExpression re("(\\d*)(F|B|R|L|U|D|x|y|z|M|E|H|S)?(f|b|r|l|u|d)?(w?)((?:2|')?)");
    auto iter = re.globalMatch(expression);
    QList<VRCAction> actions;

    while(iter.hasNext())
    {
        auto match = iter.next();
        QString nbLayer = match.captured(1);
        QString face = match.captured(2);
        QString faceSmall = match.captured(3);
        QString wide = match.captured(4);
        QString turn = match.captured(5);

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
            flags += currentNbLayer;
        else
            flags += QVariant(nbLayer).toInt();

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
