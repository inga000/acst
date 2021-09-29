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

#ifndef CORE_SCHEMATIC_TERMINAL_H_
#define CORE_SCHEMATIC_TERMINAL_H_

#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Schematic/Shapes/Point.h"
#include "Core/incl/Schematic/Shapes/Rect.h"
#include "Core/incl/Common/Object.h"

namespace Core {
    namespace Schematic {

        class Terminal : public Object
        {
        public:
            Terminal();

            void setTerminalId(const TerminalId& terminalId);
            void setPosition(const Point& position);
            void setRect(const Rect& boundingBox);
            void updateBoundingBoxAfterPositionChanged();

            TerminalId getTerminalId() const;
            Point getPosition() const;
            Rect getBoundingBox() const;

            bool operator<(const Terminal& other) const;

            std::string toStr() const;
        private:

            TerminalId terminalId_;
            Point position_;
            Rect boundingBox_;
        };
    }
}


#endif
