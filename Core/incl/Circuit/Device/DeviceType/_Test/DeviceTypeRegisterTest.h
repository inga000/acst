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

 
#ifndef CORE_DEVICETYPEREGISTERTEST_H_
#define CORE_DEVICETYPEREGISTERTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace Core {

    class DeviceType;
    class DeviceTypeName;
    class DeviceTypeRegister;
    class Core;

    class DeviceTypeRegisterTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE(DeviceTypeRegisterTest);
        CPPUNIT_TEST(getTypeByName_empty_returnsDeviceTypesAndThrows);
        CPPUNIT_TEST(getTypeByName_devicesOnly_returnsDeviceTypes);
        CPPUNIT_TEST(getTypeByName_registerNewDevice_returnsDevice);
        CPPUNIT_TEST(getTypeByName_wrongComponentString_shouldThrow);
        CPPUNIT_TEST(testComparisonOperatorsOfComponentType);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();

    private:
        // Tests:
        void getTypeByName_empty_returnsDeviceTypesAndThrows();
        void getTypeByName_devicesOnly_returnsDeviceTypes();
        void getTypeByName_registerNewDevice_returnsDevice();
        void getTypeByName_wrongComponentString_shouldThrow();
        void testComparisonOperatorsOfComponentType();

        // Tools:
        void getDeviceTypeAndTestIt(const DeviceTypeName & typeString) const;

        void resetComponentTypeRegister();
        void resetComponentTypeRegisterAndAddDevices();
        void addTwoDevices();

        void testBooleanFlagsForDevice(const DeviceType & ct) const;

        Core * core_;
        DeviceTypeRegister * reg_;
    };

}
#endif
