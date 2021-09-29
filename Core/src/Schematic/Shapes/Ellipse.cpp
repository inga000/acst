/**************************************************************************************************
 *  Main authors:
 *     Inga Abel <inga.abel@tum.de>, 
 *	   Maximilian Neuner <maximilian.neuner@tum.de>, 
 *     Michael Eick <michael.eick@tum.de>
 *
 * 
 *  Copyright (C) 2021
 *  Chair of Electronic Design Automation
 *  Univ.-Prof. Dr.-Ing. Ulf Schlichtmann
 *  TU Muenchen
 *  Arcisstrasse 21
 *  D-80333 Muenchen
 *  Germany
 *
 *  This file is part of acst, a analog circuit analysis, sizing and synthesis enviroment:
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *************************************************************************************************/

#include "Core/incl/Schematic/Shapes/Ellipse.h"
#include "Core/incl/Schematic/Symbols/Transformation.h"

namespace Core {
    namespace Schematic {

        Ellipse::Ellipse():
                width_(0),
                height_(0)
        {
        }

        void Ellipse::setTopLeftCorner(const Point& topLeftCorner)
        {
            topLeftCorner_ = topLeftCorner;
        }

        void Ellipse::setWidth(const int& width)
        {
            width_ = width;
        }

        void Ellipse::setHeight(const int& height)
        {
            height_ = height;
        }

        int Ellipse::getHeight() const
        {
            return height_;
        }

        Point Ellipse::getTopLeftCorner() const
        {
            return topLeftCorner_;
        }

        int Ellipse::getWidth() const
        {
            return width_;
        }

        Ellipse Ellipse::operator+(const Point & position)
        {
            topLeftCorner_ + position;
            return *this;
        }

        void Ellipse::moveX(int xOffset)
        {
            topLeftCorner_.moveX(xOffset);
        }

        void Ellipse::moveY(int yOffset)
        {
            topLeftCorner_.moveY(yOffset);
        }

        void Ellipse::mirrorX(int mirrorAxisY)
        {
            topLeftCorner_.mirrorX(mirrorAxisY);
        }

        void Ellipse::mirrorY(int mirrorAxisX)
        {
            topLeftCorner_.mirrorY(mirrorAxisX);
            topLeftCorner_.moveX(-getWidth());

        }

        void Ellipse::transform(const Transformation& transformation, const Point& center)
        {
            topLeftCorner_ = transformation*topLeftCorner_ + center;
        }
    }
}
