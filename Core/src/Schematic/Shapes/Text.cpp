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

#include "Core/incl/Schematic/Shapes/Text.h"

namespace Core {
    namespace Schematic {

        const int Text::AVERAGE_LETTER_WIDTH_ = 10;
        const int Text::AVERAGE_LETTER_HEIGTH_ = 16;

        Text::Text()
        {
        }

        void Text::setText(const std::string& text)
        {
            text_ = text;
        }

        void Text::setPosition(const Point& position)
        {
            position_ = position;
        }

        void Text::setTransformation(const Transformation& transformation)
        {
            transformation_ = transformation;
        }

        std::string Text::getText() const
        {
            return text_;
        }

        Point Text::getPosition() const
        {
            return position_;
        }

        Transformation Text::getTransformation() const
        {
            return transformation_;
        }

        int Text::computeTextWidth() const
        {
            return AVERAGE_LETTER_WIDTH_ * text_.length();
        }

        int Text::computeTextHeigth() const
        {
            return AVERAGE_LETTER_HEIGTH_;
        }

        Rect Text::computeBoundingBox() const
        {
            int bottomRightX = getPosition().getX() + computeTextWidth();
            int bottomRightY = getPosition().getY() - computeTextHeigth();
            Point bottomRight;
            bottomRight.setX(bottomRightX);
            bottomRight.setY(bottomRightY);
            bottomRight = getTransformation()*bottomRight;

            Rect boundingBox;
            boundingBox.setTopLeftCorner(getPosition());
            boundingBox.setBottomRightCorner(bottomRight);
            return boundingBox;
        }

        Text Text::operator+(const Point & position)
        {
            position_ = position_ + position;
            return *this;
        }

        bool Text::operator ==(const Text& other) const
        {
            return text_ == other.text_;
        }

        void Text::moveX(int xOffset)
        {
            position_.moveX(xOffset);
        }

        void Text::moveY(int yOffset)
        {
            position_.moveY(yOffset);
        }

        void Text::mirrorX(int mirrorAxisY)
        {
            position_.mirrorX(mirrorAxisY);
        }

        void Text::mirrorY(int mirrorAxisX)
        {
            position_.mirrorY(mirrorAxisX);
        }

        void Text::transform(const Transformation& transformation, const Point& center)
        {
            position_ = transformation*position_+center;
        }

        Point& Text::getPositionReference()
        {
            return position_;
        }
    }
}

