#ifndef VRCPARSER_H
#define VRCPARSER_H

#include <QList>

class VRCAction;

class VRCParser
{
public:
    static QList<VRCAction> parse(const QString& expression);

private:
    enum class LayerMain
    {
        Left       = 'L',
        Front      = 'F',
        Right      = 'R',
        Back       = 'B',
        Up         = 'U',
        Down       = 'D',
        Middle     = 'M',
        Equator    = 'E',
        Standing   = 'S',
        CubeX      = 'x',
        CubeY      = 'y',
        CubeZ      = 'z'
    };

    enum class LayerSub
    {
        Left  = 'l',
        Front = 'f',
        Right = 'r',
        Back  = 'b',
        Up    = 'u',
        Down  = 'd'
    };

private:
    VRCParser();
};

#endif // VRCPARSER_H
