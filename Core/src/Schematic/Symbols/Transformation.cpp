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

#include "Core/incl/Schematic/Symbols/Transformation.h"
#include "Core/incl/Schematic/Shapes/Text.h"
#include "Core/incl/Schematic/Shapes/EvalText.h"
#include "Log/incl/LogMacros.h"

namespace Core {
    namespace Schematic {

        Transformation::Transformation():
                t11_(1),
                t12_(0),
                t21_(0),
                t22_(1),
                angle_(0)
        {
        }

        void Transformation::setTransformationMatrix(int t11, int t12, int t21, int t22)
        {
            t11_ = t11;
            t12_ = t12;
            t21_ = t21;
            t22_ = t22;
        }

        void Core::Schematic::Transformation::setAngle(int angle)
        {
            angle_ = angle;
        }

        void Core::Schematic::Transformation::setMirrorType(const MirrorType& reflectionType)
        {
            reflectionType_ = reflectionType;
        }

        int Transformation::getT11() const
        {
            return t11_;
        }

        int Transformation::getT12() const
        {
            return t12_;
        }

        int Transformation::getT21() const
        {
            return t21_;
        }

        int Transformation::getT22() const
        {
            return t22_;
        }

        int Core::Schematic::Transformation::getAngle() const
        {
            return angle_;
        }

        MirrorType Core::Schematic::Transformation::getReflectionType() const
        {
            return reflectionType_;
        }

        Transformation Transformation::computeInverseTransformation() const
        {
            Transformation inverseTransformation;
            int determinante = computeDeterminante();
            int t11 = getT22()/determinante;
            int t12 = -getT12()/determinante;
            int t21 = -getT21()/determinante;
            int t22 = getT11()/determinante;
            int angle = -getAngle();
            MirrorType mirrorType = getReflectionType();

            inverseTransformation.setTransformationMatrix(t11, t12, t21, t22);
            inverseTransformation.setAngle(angle);
            inverseTransformation.setMirrorType(mirrorType);
            return inverseTransformation;
        }

        Transformation Transformation::operator *(const Transformation & other) const
        {
            int t11 = other.getT11();
            int t12 = other.getT12();
            int t21 = other.getT21();
            int t22 = other.getT22();

            int newT11 = t11_ * t11 + t12_ * t21;
            int newT12 = t11_ * t12 + t12_ * t22;
            int newT21 = t21_ * t11 + t22_ * t21;
            int newT22 = t21_ * t12 + t22_ * t22;

            int newAngle= (angle_ + other.getAngle()) % 360;
            MirrorType newReflectionType;
            if (reflectionType_ == MirrorType::noMirror()) {
                newReflectionType = other.getReflectionType();
                newAngle = (angle_ + other.getAngle()) % 360;
            }
            else if (reflectionType_ == MirrorType::xMirror()) {
                if (other.getReflectionType() == MirrorType::noMirror()) {
                    newReflectionType = reflectionType_;
                }
                else if (other.getReflectionType() == MirrorType::xMirror()) {
                    newReflectionType = MirrorType::noMirror();
                }
                else if (other.getReflectionType() == MirrorType::yMirror()) {
                    newReflectionType = MirrorType::noMirror();
                    newAngle = (newAngle + 180) % 360;
                }
            }
            else if (reflectionType_ == MirrorType::yMirror()) {
                if (other.getReflectionType() == MirrorType::noMirror()) {
                    newReflectionType = reflectionType_;
                }
                else if (other.getReflectionType() == MirrorType::xMirror()) {
                    newReflectionType = MirrorType::noMirror();
                    newAngle = (newAngle + 180) % 360;
                }
                else if (other.getReflectionType() == MirrorType::yMirror()) {
                    newReflectionType = MirrorType::noMirror();
                }
            }

            Transformation newTransformation;
            newTransformation.setTransformationMatrix(newT11, newT12, newT21, newT22);
            newTransformation.setAngle(newAngle);
            newTransformation.setMirrorType(newReflectionType);

            return newTransformation;
        }

        Point Transformation::operator*(Point & point) const
        {
            transformPoint(&point);
            return point;
        }

        Arc Transformation::operator *(Arc & arc) const
        {
            Point topLeftCorner = arc.getTopLeftCorner();
            int startAngle = arc.getStartAngle() + angle_;
            int stopAngle = arc.getStopAngle() + angle_;

            transformPoint(&topLeftCorner);
            if (reflectionType_ == MirrorType::xMirror()) {
                startAngle = -startAngle;
                stopAngle = -stopAngle;
            }
            else if (reflectionType_ == MirrorType::yMirror()) {
                startAngle = 180 - startAngle;
                stopAngle = 180 - stopAngle;
            }

            arc.setTopLeftCorner(topLeftCorner);
            arc.setStartAngle(startAngle);
            arc.setStopAngle(stopAngle);

            return arc;
        }

        Line Transformation::operator *(const Line & line) const
        {
            Point startPoint = line.getStartPoint();
            Point endPoint = line.getEndPoint();
            transformPoint(&startPoint);
            transformPoint(&endPoint);

            Line transformedLine(line);
            transformedLine.setStartPoint(startPoint);
            transformedLine.setEndPoint(endPoint);
            return transformedLine;
        }

        Ellipse Transformation::operator *(Ellipse & ellipse) const
        {
            Point topLeftCorner = ellipse.getTopLeftCorner();
            transformPoint(&topLeftCorner);
            ellipse.setTopLeftCorner(topLeftCorner);
            return ellipse;
        }

        Rect Transformation::operator *(const Rect & rect) const
        {
            Point topLeftCorner = rect.getTopLeftCorner();
            Point bottomRightCorner = rect.getBottomRightCorner();
            transformPoint(&topLeftCorner);
            transformPoint(&bottomRightCorner);

        	Rect transformedRect(rect);
        	transformedRect.setTopLeftCorner(topLeftCorner);
        	transformedRect.setBottomRightCorner(bottomRightCorner);
        	transformedRect.correctPointsAfterTransformation();
            return transformedRect;
        }

        Polygon Transformation::operator*(Polygon & polygon) const
        {
            int numPolygonPoints = polygon.getNumPoints();
            Polygon transformedPolygon;
            for(int i = 0; i< numPolygonPoints; i++) {
                Point polygonPoint = polygon.getPolygonPoint(i);
                transformPoint(&polygonPoint);
                transformedPolygon.addPolygonPoint(polygonPoint);
            }
            transformedPolygon.setShapeType(polygon.getShapeType());
            return transformedPolygon;
        }

        Text Transformation::operator*(Text & text) const
        {
            Point position = text.getPosition();
            transformPoint(&position);
            Text transformedText;
            transformedText.setText(text.getText());
            transformedText.setPosition(position);
            return transformedText;
        }

        EvalText Transformation::operator*(EvalText & evalText) const
        {
            Point position = evalText.getPosition();
            transformPoint(&position);
            EvalText transformedText;
            transformedText.setText(evalText.getText());
            transformedText.setPosition(position);
            return transformedText;
        }

        int Transformation::computeDeterminante() const
        {
            return getT11()*getT22() - getT21()*getT12();
        }

        void Transformation::transformPoint(Point * point) const
        {
            int newX = t11_ * point->getX() + t12_ * point->getY();
            int newY = t21_ * point->getX() + t22_ * point->getY();
            point->setX(newX);
            point->setY(newY);
        }

        bool Transformation::operator==(const Transformation& other) const
        {
            return t11_ == other.t11_ &&
                    t12_ == other.t12_ &&
                    t21_ == other.t21_ &&
                    t22_ == other.t22_ &&
                    angle_ == other.angle_ &&
                    reflectionType_ == other.reflectionType_;
        }
    }
}
