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

 
#ifndef CORE_DEVICETYPE_H_
#define CORE_DEVICETYPE_H_

#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeName.h"
#include "Core/incl/Circuit/Device/DeviceType/PinConnections.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include <string>

namespace Core {

    class Device;

    class DeviceType : public Object
    {
    protected:
    	typedef std::map<PinName, PinType> PinTypeMap;
    	typedef std::set<TechType> TechTypesSet;

    public:
    	DeviceType();
    	~DeviceType();

    private:
        virtual void initialize(){};

    public:
        void setName(const DeviceTypeName& deviceTypeName);
        void addPinType(const PinType& pinType);
        void addTechType(const TechType& techType);

        DeviceTypeName getName() const;
        PinType findPinType(const PinName& pinName) const;
        std::vector<PinType> getPinTypes() const;

    	PinConnections getEmptyPinToNetConnections() const;

        std::string toStr() const;

        bool isInitialized() const;
        bool hasPinType(const PinName& pinName) const;
    	bool operator==(const DeviceTypeName& other) const;
    	bool operator==(const DeviceType& other) const;

    protected:
        DeviceTypeName name_;
        PinTypeMap pinTypes_;
        TechTypesSet techTypes_;
    };
}
#endif
