#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QVector2D>

class Vertex
{
public:
    // Constructors
    Q_DECL_CONSTEXPR Vertex() : _id(0) {}
    Vertex(const QVector3D &position, const QVector2D &uv, uint color, int id) : _position(position), _uv(uv), _id(id) { setColor(color); }

    // Accessors / Mutators
    inline Q_DECL_CONSTEXPR const QVector3D& position() const { return _position; }
    inline Q_DECL_CONSTEXPR const QVector2D& uv() const { return _uv; }
    inline Q_DECL_CONSTEXPR const QVector3D& color() const { return _color; }
    //inline Q_DECL_CONSTEXPR float borderwidth() const { return _borderWidth; }
    inline Q_DECL_CONSTEXPR int id() const { return _id; }
    inline void setPosition(const QVector3D& position) { _position = position; }
    inline void setUV(const QVector2D& uv) { _uv = uv; }
    inline void setColor(uint color) { _color = QVector3D(float(color >> 16) / 255.0f, float((color >> 8) & 0xFF) / 255.0f, float(color & 0xFF) / 255.0f); }
    //inline void setBorderWidth(float width) { _borderWidth = width; }
    inline void setID(int id) { _id = id; }

    // OpenGL Helpers
    static const int PositionTupleSize = 3;
    static const int UVTupleSize = 2;
    static const int ColorTupleSize = 3;
    //static const int BorderTupleSize = 1;
    static const int IDTupleSize = 1;

    inline static Q_DECL_CONSTEXPR int positionOffset() { return offsetof(Vertex, _position); }
    inline static Q_DECL_CONSTEXPR int uvOffset() { return offsetof(Vertex, _uv); }
    inline static Q_DECL_CONSTEXPR int colorOffset() { return offsetof(Vertex, _color); }
    //inline static Q_DECL_CONSTEXPR int borderWidthOffset() { return offsetof(Vertex, _borderWidth); }
    inline static Q_DECL_CONSTEXPR int idOffset() { return offsetof(Vertex, _id); }
    inline static Q_DECL_CONSTEXPR int stride() { return sizeof(Vertex); }

private:
    QVector3D _position;
    QVector2D _uv;
    QVector3D _color;
    //float _borderWidth;
    int _id;
};

// Note: Q_MOVABLE_TYPE means it can be memcpy'd.
Q_DECLARE_TYPEINFO(Vertex, Q_MOVABLE_TYPE);

#endif // VERTEX_H
