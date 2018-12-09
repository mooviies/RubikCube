#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QVector2D>

class Vertex
{
public:
    // Constructors
    Q_DECL_CONSTEXPR Vertex();
    Q_DECL_CONSTEXPR explicit Vertex(const QVector3D &position, const QVector2D &uv);
    Q_DECL_CONSTEXPR Vertex(const QVector3D &position, const QVector2D &uv, const QVector3D &color);
    Vertex(const QVector3D &position, const QVector2D &uv, uint color, float borderWidth);

    // Accessors / Mutators
    Q_DECL_CONSTEXPR const QVector3D& position() const;
    Q_DECL_CONSTEXPR const QVector2D& uv() const;
    Q_DECL_CONSTEXPR const QVector3D& color() const;
    Q_DECL_CONSTEXPR float borderwidth() const;
    void setPosition(const QVector3D& position);
    void setUV(const QVector2D& uv);
    void setColor(const QVector3D& color);
    void setColor(uint color);
    void setBorderWidth(float width);

    // OpenGL Helpers
    static const int PositionTupleSize = 3;
    static const int UVTupleSize = 2;
    static const int ColorTupleSize = 3;
    static const int BorderTupleSize = 1;
    static Q_DECL_CONSTEXPR int positionOffset();
    static Q_DECL_CONSTEXPR int uvOffset();
    static Q_DECL_CONSTEXPR int colorOffset();
    static Q_DECL_CONSTEXPR int borderWidthOffset();
    static Q_DECL_CONSTEXPR int stride();

private:
    QVector3D _position;
    QVector2D _uv;
    QVector3D _color;
    float _borderWidth;
};

/*******************************************************************************
 * Inline Implementation
 ******************************************************************************/

// Note: Q_MOVABLE_TYPE means it can be memcpy'd.
Q_DECLARE_TYPEINFO(Vertex, Q_MOVABLE_TYPE);

// Constructors
Q_DECL_CONSTEXPR inline Vertex::Vertex() : _borderWidth(0) {}
Q_DECL_CONSTEXPR inline Vertex::Vertex(const QVector3D &position, const QVector2D &uv) : _position(position), _uv(uv), _borderWidth(0) {}
Q_DECL_CONSTEXPR inline Vertex::Vertex(const QVector3D &position, const QVector2D &uv, const QVector3D &color) : _position(position), _uv(uv), _color(color), _borderWidth(0) {}
inline Vertex::Vertex(const QVector3D &position, const QVector2D &uv, uint color, float borderWidth) : _position(position), _uv(uv), _borderWidth(borderWidth) { setColor(color); }

// Accessors / Mutators
Q_DECL_CONSTEXPR inline const QVector3D& Vertex::position() const { return _position; }
Q_DECL_CONSTEXPR inline const QVector2D& Vertex::uv() const { return _uv; }
Q_DECL_CONSTEXPR inline const QVector3D& Vertex::color() const { return _color; }
Q_DECL_CONSTEXPR inline float Vertex::borderwidth() const { return _borderWidth; }
void inline Vertex::setPosition(const QVector3D& position) { _position = position; }
void inline Vertex::setUV(const QVector2D& uv) { _uv = uv; }
void inline Vertex::setColor(const QVector3D& color) { _color = color; }
void inline Vertex::setColor(uint color) { _color = QVector3D(float(color >> 16) / 255.0f, float((color >> 8) & 0xFF) / 255.0f, float(color & 0xFF) / 255.0f); }
void inline Vertex::setBorderWidth(float width) { _borderWidth = width; }

// OpenGL Helpers
Q_DECL_CONSTEXPR inline int Vertex::positionOffset() { return offsetof(Vertex, _position); }
Q_DECL_CONSTEXPR inline int Vertex::uvOffset() { return offsetof(Vertex, _uv); }
Q_DECL_CONSTEXPR inline int Vertex::colorOffset() { return offsetof(Vertex, _color); }
Q_DECL_CONSTEXPR inline int Vertex::borderWidthOffset() { return offsetof(Vertex, _borderWidth); }
Q_DECL_CONSTEXPR inline int Vertex::stride() { return sizeof(Vertex); }

#endif // VERTEX_H
