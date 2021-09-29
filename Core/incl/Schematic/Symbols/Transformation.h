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

#ifndef CORE_SCHEMATIC_TRANSFORMATION_H_
#define CORE_SCHEMATIC_TRANSFORMATION_H_

#include "Core/incl/Schematic/Shapes/Arc.h"
#include "Core/incl/Schematic/Shapes/Ellipse.h"
#include "Core/incl/Schematic/Shapes/Line.h"
#include "Core/incl/Schematic/Shapes/Rect.h"
#include "Core/incl/Schematic/Shapes/Point.h"
#include "Core/incl/Schematic/Shapes/Polygon.h"
#include "Core/incl/Schematic/Symbols/MirrorType.h"

namespace Core {
    namespace Schematic {

        class Text;
        class EvalText;

        class Transformation
        {
        public:
            Transformation();

            void setTransformationMatrix(int t11, int t12, int t21, int t22);
            void setAngle(int angle);
            void setMirrorType(const MirrorType & reflectionType);

            int getT11() const;
            int getT12() const;
            int getT21() const;
            int getT22() const;
            int getAngle() const;
            MirrorType getReflectionType() const;
            Transformation computeInverseTransformation() const;

            Transformation operator*(const Transformation & other) const;
            Point operator*(Point & point) const;
            Arc operator*(Arc & arc) const;
            Line operator*(const Line & line) const;
            Ellipse operator*(Ellipse & ellipse) const;
            Rect operator*(const Rect & rect) const;
            Polygon operator*(Polygon & polygon) const;
            Text operator*(Text & text) const;
            EvalText operator*(EvalText& text) const;

            bool operator==(const Transformation& other) const;
        private:
            int computeDeterminante() const;
            void transformPoint(Point * point) const;

            int t11_;
            int t12_;
            int t21_;
            int t22_;

            int angle_;
            MirrorType reflectionType_;
        };
    }
}


#endif
