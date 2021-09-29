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

#include "Core/incl/Schematic/Shapes/Rect.h"
#include "Core/incl/Schematic/Shapes/Line.h"
#include <sstream>

namespace Core {
    namespace Schematic {

    	Rect::Rect(const Rect& other):
			Shape(other),
			topLeftCorner_(other.topLeftCorner_),
			bottomRightCorner_(other.bottomRightCorner_)
		{

		}


        void Rect::setTopLeftCorner(const Point& topLeftCorner)
        {
            topLeftCorner_ = topLeftCorner;
        }

        void Rect::setBottomRightCorner(const Point& bottomRightCorner)
        {
            bottomRightCorner_ = bottomRightCorner;
        }

        std::vector<Line> Rect::computeBoundingLines() const
        {
        	std::vector<Line> boundingLines;

            Point topLeft = getTopLeftCorner();
            Point topRight = computeTopRightCorner();
            Point bottomRight = getBottomRightCorner();
            Point bottomLeft = computeBottomLeftCorner();

            Line lineA;
            lineA.setStartPoint(topLeft);
            lineA.setEndPoint(topRight);
            boundingLines.push_back(lineA);

            Line lineB;
            lineB.setStartPoint(topRight);
            lineB.setEndPoint(bottomRight);
            boundingLines.push_back(lineB);

            Line lineC;
            lineC.setStartPoint(bottomRight);
            lineC.setEndPoint(bottomLeft);
            boundingLines.push_back(lineC);

            Line lineD;
            lineD.setStartPoint(bottomLeft);
            lineD.setEndPoint(topLeft);
            boundingLines.push_back(lineD);
            return boundingLines;
        }

        int Rect::width() const
        {
            return getBottomRightCorner().getX() - getTopLeftCorner().getX();
        }

        int Rect::height() const
        {
            return getTopLeftCorner().getY() - getBottomRightCorner().getY();
        }

        void Rect::correctPointsAfterTransformation()
        {
            int newLeft;
            int newRight;
            int newTop;
            int newBottom;

            int oldLeft = topLeftCorner_.getX();
            int oldRight = bottomRightCorner_.getX();
            int oldTop = topLeftCorner_.getY();
            int oldBottom = bottomRightCorner_.getY();

            if (oldLeft < oldRight) {
                newLeft = oldLeft;
                newRight = oldRight;
            }
            else {
                newLeft = oldRight;
                newRight = oldLeft;
            }

            if (oldTop < oldBottom) {
                newTop = oldTop;
                newBottom = oldBottom;
            }
            else {
                newTop = oldBottom;
                newBottom = oldTop;
            }

            topLeftCorner_.setX(newLeft);
            topLeftCorner_.setY(newTop);
            bottomRightCorner_.setX(newRight);
            bottomRightCorner_.setY(newBottom);
        }

        Point Rect::getTopLeftCorner() const
        {
            return topLeftCorner_;
        }

        Point Rect::getBottomRightCorner() const
        {
            return bottomRightCorner_;
        }

        Point Rect::computeBottomLeftCorner() const
        {
            Point point;
            point.setX(0);
            point.setY(height());

            return getTopLeftCorner() - point;
        }

        Point Rect::computeTopRightCorner() const
        {
            Point point;
            point.setX(0);
            point.setY(height());

            return getBottomRightCorner() + point;
        }

        Rect Rect::operator+(const Point & position)
        {
            topLeftCorner_ + position;
            bottomRightCorner_ + position;
            return *this;
        }

        bool Rect::hasPointInside(const Point & point) const
        {
        	if(topLeftCorner_.getX() <= point.getX() &&
        	   topLeftCorner_.getY() <= point.getY() &&
			   bottomRightCorner_.getX() >= point.getX() &&
			   bottomRightCorner_.getY() >= point.getY())
        		return true;
        	else
        		return false;
        }


        std::string Rect::toStr() const
        {
            std::ostringstream oss;
            oss << "TopLeft: " << getTopLeftCorner() << " BottomRight: " << getBottomRightCorner();
            return oss.str();
        }
    }
}
