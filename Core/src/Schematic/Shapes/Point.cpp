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

#include "Core/incl/Schematic/Shapes/Point.h"
#include <sstream>

namespace Core {
    namespace Schematic {

        Point::Point() :
            x_(0),
            y_(0),
            xInitialized_(false),
            yInitialized_(false)
        {
        }

        void Point::setX(const int& x)
        {
            x_ = x;
            xInitialized_ = true;
        }

        void Point::setY(const int& y)
        {
            y_ = y;
            yInitialized_ = true;
        }

        int Point::getX() const
        {
            assert(isInitialized());
            return x_;
        }

        int Point::getY() const
        {
            assert(isInitialized());
            return y_;
        }

        bool Point::isInitialized() const
        {
            return xInitialized_ && yInitialized_;
        }

        Point Point::operator +(const Point & other)
        {
            x_ += other.x_;
            y_ += other.y_;
            return *this;
        }

        Point Point::operator-(const Point& other)
        {
            x_ -= other.x_;
            y_ -= other.y_;
            return *this;
        }

        Point Point::operator/(const int& other)
        {
        	x_ = x_/other;
        	y_ = y_/other;
        	return *this;
        }

        bool Point::operator==(const Point& other) const
        {
            return getX() == other.getX() &&
                    getY() == other.getY();
        }

        bool Point::operator<(const Point& other) const
        {
            int absSquare = getX()*getX() + getY()*getY();
            int otherAbsSquare = other.getX()*other.getX() + other.getY()*other.getY();
            return absSquare < otherAbsSquare;
        }

        std::string Point::toStr() const
        {
            std::ostringstream oss;
            oss << "(" << getX() << ", " << getY() << ")";
            return oss.str();
        }

        void Point::moveX(int xOffset)
        {
            x_ += xOffset;
        }

        void Point::moveY(int yOffset)
        {
            y_ += yOffset;
        }

        void Point::mirrorX(int mirrorAxisY)
        {
            y_ -= 2*(y_ - mirrorAxisY);
        }

        void Point::mirrorY(int mirrorAxisX)
        {
            x_ += 2*(mirrorAxisX - x_);
        }
    }
}


