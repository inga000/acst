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

#include "Core/incl/Schematic/Shapes/Line.h"
#include "Core/incl/Schematic/Symbols/Transformation.h"
#include <sstream>

namespace Core {
    namespace Schematic {

        Line::Line():
                busNetFlag_(false), visited_(false)
        {
        }

        Line::Line(const Line& other):
        		Shape(other),
                busNetFlag_(other.busNetFlag_),
				visited_(other.visited_),
				startPoint_(other.startPoint_),
				endPoint_(other.endPoint_)
        {
        }

        Line::~Line()
        {
        }

        void Line::setIsBusNet(bool isBusNet)
        {
            busNetFlag_ = isBusNet;
        }

        void Line::setStartPoint(const Point& startPoint)
        {
            startPoint_ = startPoint;
        }

        void Line::setEndPoint(const Point& endPoint)
        {
            endPoint_ = endPoint;
        }

        bool Line::isBusNet() const
        {
            return busNetFlag_;
        }

        Point Line::getStartPoint() const
        {
            return startPoint_;
        }

        Point Line::getEndPoint() const
        {
            return endPoint_;
        }

        Point Line::getLeftMostPoint() const
        {
            if(getStartPoint().getX() < getEndPoint().getX()) {
                return getStartPoint();
            } else {
                return getEndPoint();
            }
        }

        Point Line::getRightMostPoint() const
        {
            if(getStartPoint().getX() > getEndPoint().getX()) {
                return getStartPoint();
            } else {
                return getEndPoint();
            }
        }

        Point Line::getTopMostPoint() const
        {
            if(getStartPoint().getY() > getEndPoint().getY()) {
                return getStartPoint();
            } else {
                return getEndPoint();
            }
        }

        Point Line::getBottomMostPoint() const
        {
            if(getStartPoint().getY() < getEndPoint().getY()) {
                return getStartPoint();
            } else {
                return getEndPoint();
            }
        }

        bool Line::hasPoint(const Point& point) const
        {
            return getStartPoint() == point || getEndPoint() == point;
        }

        Point Line::getOtherEnd(const Point& point) const
        {
            assert(hasPoint(point));
            if(getStartPoint() == point) {
                return getEndPoint();
            } else {
                return getStartPoint();
            }
        }

        int Line::width() const
        {
            int width = getStartPoint().getX() - getEndPoint().getX();
            if(width < 0) {
                return -width;
            } else {
                return width;
            }
        }

        int Line::height() const
        {
            int height = getStartPoint().getY() - getEndPoint().getY();
            if(height < 0) {
                return -height;
            } else {
                return height;
            }
        }

        bool Line::isHorizontal() const
        {
            return getStartPoint().getY() == getEndPoint().getY();
        }

        bool Line::isVertical() const
        {
            return getStartPoint().getX() == getEndPoint().getX();
        }

        Line Line::operator+(const Point & position)
        {
            startPoint_ + position;
            endPoint_ + position;
            return *this;
        }

        bool Line::operator==(const Line& other) const
        {
            return getStartPoint() == other.getStartPoint() &&
                    getEndPoint() == other.getEndPoint();
        }

        bool Line::operator!=(const Line& other) const
        {
            return !operator==(other);
        }

        bool Line::operator<(const Line& other) const
        {
            if(getStartPoint() < other.getStartPoint()) {
                return true;
            } else {
                return getEndPoint() < other.getEndPoint();
            }
        }

        std::string Line::toStr() const
        {
            std::ostringstream oss;
            oss << "[ " << getStartPoint() << ", " << getEndPoint() << " ]";
            return oss.str();
        }

        void Line::moveX(int xOffset)
        {
            startPoint_.moveX(xOffset);
            endPoint_.moveX(xOffset);
        }

        void Line::moveY(int yOffset)
        {
            startPoint_.moveY(yOffset);
            endPoint_.moveY(yOffset);
        }

        void Line::mirrorX(int mirrorAxisY)
        {
            startPoint_.mirrorX(mirrorAxisY);
            endPoint_.mirrorX(mirrorAxisY);
        }

        void Line::mirrorY(int mirrorAxisX)
        {
            startPoint_.mirrorY(mirrorAxisX);
            endPoint_.mirrorY(mirrorAxisX);
        }

        void Line::transform(const Transformation& transformation, const Point& center)
        {
            startPoint_ = transformation*startPoint_ + center;
            endPoint_ = transformation*endPoint_ + center;
        }
    }
}
