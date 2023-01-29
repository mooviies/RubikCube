#ifndef VRCFACE_H
#define VRCFACE_H

#include <QList>
#include <QMap>
#include "vrcaction.h"

class VRCFace
{
public:
    enum class Side : uchar
    {
        Left   = 0,
        Front  = 1,
        Right  = 2,
        Back   = 3,
        Up     = 4,
        Down   = 5
    };

    enum class Color
    {
        Green  = 0x53D151,
        Red    = 0xDB2617,
        Yellow = 0xF2F70C,
        White  = 0xFFFFFF,
        Blue   = 0x026CDB,
        Orange = 0xFF8E07,
        Black  = 0x000000,
        Error  = 0xFF0DFF
    };

private:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Side;
        using pointer           = value_type*;
        using reference         = value_type&;

        Iterator(pointer ptr, uint size) : _ptr(ptr), _size(size) {}

        reference operator*() const { return *_ptr; }
        pointer operator->() { return _ptr; }

        friend bool operator==(const Iterator& a, const Iterator&b) { return a._ptr == b._ptr; }
        friend bool operator!=(const Iterator& a, const Iterator&b) { return a._ptr != b._ptr; }

    protected:
        pointer _ptr;
        uint _size;
    };

public:
    struct RowIterator : Iterator {
        RowIterator(pointer ptr, uint size) : Iterator(ptr, size) {}
        RowIterator& operator++() { _ptr++; return *this; }
        RowIterator operator++(int) { auto tmp = *this; ++(*this); return tmp;}
    };

    struct ReverseRowIterator : Iterator {
        ReverseRowIterator(pointer ptr, uint size) : Iterator(ptr, size) {}
        ReverseRowIterator& operator++() { _ptr--; return *this; }
        ReverseRowIterator operator++(int) { auto tmp = *this; ++(*this); return tmp;}
    };

    struct ColumnIterator : Iterator {
        ColumnIterator(pointer ptr, uint size) : Iterator(ptr, size) {}
        ColumnIterator& operator++() { _ptr += _size; return *this; }
        ColumnIterator operator++(int) { auto tmp = *this; ++(*this); return tmp;}
    };

    struct ReverseColumnIterator : Iterator {
        ReverseColumnIterator(pointer ptr, uint size) : Iterator(ptr, size) {}
        ReverseColumnIterator& operator++() { _ptr -= _size; return *this; }
        ReverseColumnIterator operator++(int) { auto tmp = *this; ++(*this); return tmp;}
    };

    VRCFace(uint size, Side side);
    ~VRCFace() { delete[] _pieces; }

    uint getSize() const { return _size; }

    Side getInitialSide() const { return _initialSide; }
    Side getSide(uint row, uint col) const { return _pieces[getPosition(row - 1, col - 1)]; }

    RowIterator beginRow(uint row) { return RowIterator(&_pieces[getPosition(row - 1, 0)], _size); }
    RowIterator endRow(uint row) { return RowIterator(&_pieces[getPosition(row - 1, _size)], _size); }

    ReverseRowIterator rbeginRow(uint row) { return ReverseRowIterator(&_pieces[getPosition(row - 1, _size - 1)], _size); }
    ReverseRowIterator rendRow(uint row) { return ReverseRowIterator(&_pieces[getPosition(row - 1, -1)], _size); }

    ColumnIterator beginCol(uint col) { return ColumnIterator(&_pieces[getPosition(0, col - 1)], _size); }
    ColumnIterator endCol(uint col) { return ColumnIterator(&_pieces[getPosition(_size, col - 1)], _size); }

    ReverseColumnIterator rbeginCol(uint col) { return ReverseColumnIterator(&_pieces[getPosition(_size - 1, col - 1)], _size); }
    ReverseColumnIterator rendCol(uint col) { return ReverseColumnIterator(&_pieces[getPosition(-1, col - 1)], _size); }

    void rotate(VRCAction::Rotation rotation);

    template<class ForwardIt1, class ForwardIt2, class ForwardIt3, class ForwardIt4>
    static void rotate(ForwardIt1 it1, ForwardIt1 it1End, ForwardIt2 it2, ForwardIt3 it3, ForwardIt4 it4, VRCAction::Rotation rotation);

private:
    uint getPosition(uint row, uint col) const { return col + row * _size; }
    uint getPosition(uint row, uint col, VRCAction::Rotation rotation) const;

private:
    Side _initialSide;
    uint _size;
    uint _arraySize;
    Side* _pieces;
};

template<class ForwardIt1, class ForwardIt2, class ForwardIt3, class ForwardIt4>
void VRCFace::rotate(ForwardIt1 it1, ForwardIt1 it1End, ForwardIt2 it2, ForwardIt3 it3, ForwardIt4 it4, VRCAction::Rotation rotation)
{
    for(; it1 != it1End; ++it1,++it2,++it3,++it4)
    {
        auto buffer = *it4;
        switch(rotation)
        {
            case VRCAction::Rotation::Clockwise:
                *it4 = *it3;
                *it3 = *it2;
                *it2 = *it1;
                *it1 = buffer;
                break;
            case VRCAction::Rotation::CounterClockwise:
                *it4 = *it1;
                *it1 = *it2;
                *it2 = *it3;
                *it3 = buffer;
                break;
            case VRCAction::Rotation::Turn180:
                *it4 = *it2;
                *it2 = buffer;
                buffer = *it1;
                *it1 = *it3;
                *it3 = buffer;
                break;
        }
    }
}

#endif // VRCFACE_H
