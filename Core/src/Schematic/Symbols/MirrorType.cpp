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

#include "Core/incl/Schematic/Symbols/MirrorType.h"

namespace Core {
    namespace Schematic {

    const std::string MirrorType::NO_MIRROR_STRING_ = "noMirror";
    const std::string MirrorType::X_MIRROR_TYPE_STRING_ = "xMirror";
    const std::string MirrorType::Y_MIRROR_TYPE_STRING_ = "yMirror";

    MirrorType::MirrorType():
                typeString_(NO_MIRROR_STRING_)
    {
    }

    MirrorType MirrorType::noMirror()
    {
        MirrorType noMirrorReflectionType;
        noMirrorReflectionType.setTypeString(NO_MIRROR_STRING_);
        return noMirrorReflectionType;
    }

    MirrorType MirrorType::xMirror()
    {
        MirrorType xMirrorReflectionType;
        xMirrorReflectionType.setTypeString(X_MIRROR_TYPE_STRING_);
        return xMirrorReflectionType;
    }

    MirrorType MirrorType::yMirror()
    {
        MirrorType yMirrorReflectionType;
        yMirrorReflectionType.setTypeString(Y_MIRROR_TYPE_STRING_);
        return yMirrorReflectionType;
    }

    bool MirrorType::operator ==(const MirrorType& other) const
    {
        return typeString_ == other.typeString_;
    }

    bool MirrorType::operator !=(const MirrorType& other) const
    {
        return !(*this == other);
    }

    std::string MirrorType::toStr() const
    {
        return typeString_;
    }

    void MirrorType::setTypeString(const std::string& typeString)
    {
        typeString_ = typeString;
    }

    }
}


