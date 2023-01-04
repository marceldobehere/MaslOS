#pragma once
#include <stdint.h>

namespace GuiComponentStuff
{
    struct ComponentSize
    {
        int FixedX, FixedY;
        bool IsXFixed, IsYFixed; // basically is the size in pixel or in percent
        double ScaledX, ScaledY;
        //bool undefined;

        ComponentSize(int x, int y) // pixels
        {
            FixedX = x;
            IsXFixed = true;
            FixedY = y;
            IsYFixed = true;
            //undefined = false;
        }

        ComponentSize(double x, double y) // percent like 0.9 for 90%
        {
            ScaledX = x;
            IsXFixed = false;
            ScaledY = y;
            IsYFixed = false;
            //undefined = false;
        }

        // ComponentSize(bool any)
        // {
        //     if (any)
        //     {
        //         FixedX = 0;
        //         IsXFixed = false;
        //         FixedY = 0;
        //         IsYFixed = false;
        //         undefined = true;
        //     }
        //     else
        //     {
        //         FixedX = 0;
        //         IsXFixed = true;
        //         FixedY = 0;
        //         IsYFixed = true;
        //         undefined = false;
        //     }
        // }

        ComponentSize()
        {
            FixedX = 0;
            IsXFixed = true;
            FixedY = 0;
            IsYFixed = true;
        }
        
        bool operator!=(ComponentSize other)
        {
            return
                FixedX != other.FixedX ||
                FixedY != other.FixedY ||
                IsXFixed != other.IsXFixed ||
                IsYFixed != other.IsYFixed;
        }
    };

    struct Position
    {
        int x, y;
        Position(int x, int y)
        {
            this->x = x;
            this->y = y;
        }

        Position()
        {
            x = 0;
            y = 0;
        }

        Position operator-(Position other)
        {
            return Position(this->x - other.x, this->y - other.y);
        }

        Position operator+(Position other)
        {
            return Position(this->x + other.x, this->y + other.y);
        }

        bool operator!=(Position other)
        {
            return x != other.x ||
                    y != other.y;
        }
    };

    struct Field
    {
        Position TL, BR;
        
        Field(Position tl, Position br)
        {
            TL = tl;
            BR = br;
        }

        Field()
        {
            TL = Position();
            BR = Position();
        }

        Field operator- (Position other)
        {
            return Field(TL - other, BR - other);
        }
    };



}