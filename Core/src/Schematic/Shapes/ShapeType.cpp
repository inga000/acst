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

#include "Core/incl/Schematic/Shapes/ShapeType.h"

namespace Core {
    namespace Schematic {

        const std::string ShapeType::NOT_INITIALIZED_STRING_ = "NotIntialized";
        const std::string ShapeType::WIRE_TYPE_STRING_ = "wire";
        const std::string ShapeType::PIN_TYPE_STRING_ = "pin";
        const std::string ShapeType::LABEL_TYPE_STRING_ = "label";
        const std::string ShapeType::SYMBOL_TYPE_STRING_ = "symbol";
        const std::string ShapeType::BOUNDINGBOX_TYPE_STRING_ = "boundingBox";
        const std::string ShapeType::TEXT_TYPE_STRING_ = "text";
        const std::string ShapeType::OTHER_TYPE_STRING_ = "other";

        ShapeType::ShapeType() :
                typeString_(NOT_INITIALIZED_STRING_)
        {
        }

        bool ShapeType::isInitialized() const
        {
            return typeString_ != NOT_INITIALIZED_STRING_;
        }

        ShapeType ShapeType::wire()
        {
            ShapeType wireLayerType;
            wireLayerType.setTypeString(WIRE_TYPE_STRING_);
            return wireLayerType;
        }

        ShapeType ShapeType::pin()
        {
            ShapeType pinLayerType;
            pinLayerType.setTypeString(PIN_TYPE_STRING_);
            return pinLayerType;
        }

        ShapeType ShapeType::label()
        {
            ShapeType labelLayerType;
            labelLayerType.setTypeString(LABEL_TYPE_STRING_);
            return labelLayerType;
        }

        ShapeType ShapeType::symbol()
        {
            ShapeType symbolLayerType;
            symbolLayerType.setTypeString(SYMBOL_TYPE_STRING_);
            return symbolLayerType;
        }

        ShapeType ShapeType::boundingBox()
        {
            ShapeType boundingBoxLayerType;
            boundingBoxLayerType.setTypeString(BOUNDINGBOX_TYPE_STRING_);
            return boundingBoxLayerType;
        }

        ShapeType ShapeType::text()
        {
            ShapeType textLayerType;
            textLayerType.setTypeString(TEXT_TYPE_STRING_);
            return textLayerType;
        }

        ShapeType ShapeType::other()
        {
            ShapeType otherType;
            otherType.setTypeString(OTHER_TYPE_STRING_);
            return otherType;
        }

        bool ShapeType::operator ==(const ShapeType& other) const
        {
            return typeString_ == other.typeString_;
        }

        bool ShapeType::operator !=(const ShapeType& other) const
        {
            return !(*this == other);
        }

        std::string ShapeType::toStr() const
        {
            return typeString_;
        }

        void ShapeType::setTypeString(const std::string& typeString)
        {
            typeString_ = typeString;
        }

    }
}
