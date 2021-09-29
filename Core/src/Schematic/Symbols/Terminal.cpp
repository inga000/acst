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

#include "Core/incl/Schematic/Symbols/Terminal.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace Core {
    namespace Schematic {

        Terminal::Terminal()
        {
        }

        void Terminal::setTerminalId(const TerminalId& terminalId)
        {
            terminalId_ = terminalId;
        }

        void Terminal::setPosition(const Point& position)
        {
            position_ = position;
        }

        void Terminal::setRect(const Rect& boundingBox)
        {
            boundingBox_ = boundingBox;
        }

        void Terminal::updateBoundingBoxAfterPositionChanged()
        {
            int width = boundingBox_.width();
            int heigth = boundingBox_.height();

            Core::Schematic::Point topLeftCorner;
            topLeftCorner.setX(position_.getX() - width/2);
            topLeftCorner.setY(position_.getY() + heigth/2);
            Core::Schematic::Point bottomRightCorner;
            bottomRightCorner.setX(position_.getX() + width/2);
            bottomRightCorner.setY(position_.getY() - width/2);

            boundingBox_.setTopLeftCorner(topLeftCorner);
            boundingBox_.setBottomRightCorner(bottomRightCorner);
        }

        TerminalId Terminal::getTerminalId() const
        {
            assert(terminalId_.isIntialized());
            return terminalId_;
        }

        Point Terminal::getPosition() const
        {
            return position_;
        }

        Rect Terminal::getBoundingBox() const
        {
            return boundingBox_;
        }

        bool Terminal::operator<(const Terminal& other) const
        {
            if(terminalId_ == other.terminalId_) {
                return position_ < other.position_;
            } else {
                return terminalId_ < other.terminalId_;
            }
        }

        std::string Terminal::toStr() const
        {
            std::ostringstream oss;
            oss << "Terminal: " << terminalId_.getName() << " Position: " << position_;
            return oss.str();
        }
    }
}


