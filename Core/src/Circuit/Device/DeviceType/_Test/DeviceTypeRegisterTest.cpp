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
#include "Core/incl/Circuit/Device/DeviceType/_Test/DeviceTypeRegisterTest.h"
#include "Core/incl/Core.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeNames.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeRegister.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeRegisterFiller.h"
#include <iostream>

namespace Core {

    void DeviceTypeRegisterTest::setUp()
    {
        core_ = new Core;
        reg_ = new DeviceTypeRegister();
    }

    void DeviceTypeRegisterTest::tearDown()
    {
        delete reg_;
        delete core_;
    }

    void DeviceTypeRegisterTest::getTypeByName_empty_returnsDeviceTypesAndThrows()
    {
        resetComponentTypeRegister();
        CPPUNIT_ASSERT_THROW(reg_->findComponentType(DeviceTypeNames::Mosfet), BacktraceException *);
    }

    void DeviceTypeRegisterTest::getTypeByName_devicesOnly_returnsDeviceTypes()
    {
        resetComponentTypeRegisterAndAddDevices();

        getDeviceTypeAndTestIt(DeviceTypeNames::Mosfet);
        getDeviceTypeAndTestIt(DeviceTypeNames::Bipolar);
        getDeviceTypeAndTestIt(DeviceTypeNames::Resistor);
        getDeviceTypeAndTestIt(DeviceTypeNames::Capacitor);
    }

    void DeviceTypeRegisterTest::getTypeByName_registerNewDevice_returnsDevice()
    {
        resetComponentTypeRegisterAndAddDevices();
        addTwoDevices();

        getDeviceTypeAndTestIt(DeviceTypeName("device2"));
        getDeviceTypeAndTestIt(DeviceTypeName("device1"));
    }

    void DeviceTypeRegisterTest::getTypeByName_wrongComponentString_shouldThrow()
    {
        CPPUNIT_ASSERT_THROW(reg_->findComponentType(DeviceTypeName("not_there")), BacktraceException *);
    }

    void DeviceTypeRegisterTest::getDeviceTypeAndTestIt(const DeviceTypeName& typeString) const
    {
        const DeviceType & ct = reg_->findComponentType(typeString);

        testBooleanFlagsForDevice(ct);
        CPPUNIT_ASSERT(ct.getName() == typeString);
        CPPUNIT_ASSERT(ct == typeString);
    }

    void DeviceTypeRegisterTest::testComparisonOperatorsOfComponentType()
    {
        resetComponentTypeRegisterAndAddDevices();
        addTwoDevices();

        const DeviceType & device1 = reg_->findComponentType(DeviceTypeName("device1"));
        const DeviceType & device2 = reg_->findComponentType(DeviceTypeName("device2"));

        CPPUNIT_ASSERT(device1 == device1);
        CPPUNIT_ASSERT(device2 == device2);
        CPPUNIT_ASSERT(device1 != device2);
    }

    void DeviceTypeRegisterTest::resetComponentTypeRegister()
    {
        reg_->reset();
    }

    void DeviceTypeRegisterTest::resetComponentTypeRegisterAndAddDevices()
    {
        reg_->reset();
        DeviceTypeRegisterFiller componentTypeRegisterFiller(*core_, *reg_);
        componentTypeRegisterFiller.registerDevices();
    }

    void DeviceTypeRegisterTest::addTwoDevices()
    {
        reg_->registerNewDevice(DeviceTypeName("device1"));
        reg_->registerNewDevice(DeviceTypeName("device2"));
    }
}
