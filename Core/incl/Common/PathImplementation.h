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

#include <sstream>
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

    template<class NameClass, class DerivedPath>
    inline Path<NameClass, DerivedPath>::Path()
    {
    }

    template<class NameClass, class DerivedPath>
    inline Path<NameClass, DerivedPath>::~Path()
    {
    }

    template<class NameClass, class DerivedPath>
    inline void Path<NameClass, DerivedPath>::setName(const NameClass& name)
    {
        this->name_ = name;
    }

    template<class NameClass, class DerivedPath>
    inline void Path<NameClass, DerivedPath>::setPathToRoot()
    {
        path_.setIsRoot();
    }

    template<class NameClass, class DerivedPath>
    inline void Path<NameClass, DerivedPath>::setPath(const InstanceNamePath& path)
    {
        this->path_ = path;
    }

    template<class NameClass, class DerivedPath>
    inline bool Path<NameClass, DerivedPath>::isInitialized() const
    {
        return hasName() && hasPath();
    }


    template<class NameClass, class DerivedPath>
    inline bool Path<NameClass, DerivedPath>::isRoot() const
    {
        return path_.isRoot();
    }

    template<class NameClass, class DerivedPath>
    inline std::string Path<NameClass, DerivedPath>::toStr() const
    {
        assert(isInitialized());
        std::ostringstream tempStream;
        tempStream << path_ << name_;
        return tempStream.str();
    }

    template<class NameClass, class DerivedPath>
    inline bool Path<NameClass, DerivedPath>::operator ==(const DerivedPath& other) const
    {
        assert(isInitialized());
        return name_ == other.name_ && path_ == other.path_;
    }

    template<class NameClass, class DerivedPath>
    inline bool Path<NameClass, DerivedPath>::operator !=(const DerivedPath& other) const
    {
        return !(*this == other);
    }

    template<class NameClass, class DerivedPath>
    inline bool Path<NameClass, DerivedPath>::operator <(const DerivedPath& other) const
    {
        assert(isInitialized());
        if(path_ == other.path_) {
            return name_ < other.name_;
        } else {
            return path_ < other.path_;
        }
    }

    template<class NameClass, class DerivedPath>
    inline bool Path<NameClass, DerivedPath>::operator >(const DerivedPath& other) const
    {
        return !(*this < other) && !(*this == other);
    }

    template<class NameClass, class DerivedPath>
    inline NameClass Path<NameClass, DerivedPath>::getName() const
    {
        assert(isInitialized());
        return name_;
    }

    template<class NameClass, class DerivedPath>
    inline InstanceNamePath Path<NameClass, DerivedPath>::getInstanceNamePath() const
    {
        assert(hasPath());
        return path_;
    }

    template<class NameClass, class DerivedPath>
    inline bool Path<NameClass, DerivedPath>::hasName() const
    {
        return name_.isInitialized();
    }

    template<class NameClass, class DerivedPath>
    inline bool Path<NameClass, DerivedPath>::hasPath() const
    {
        return path_.isInitialized();
    }

}
