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

#include "Core/incl/Schematic/Symbols/TransformationMatrices.h"
#include "Core/incl/Schematic/Symbols/MirrorType.h"

namespace Core {
    namespace Schematic {

        TransformationMatrices::TransformationMatrices()
        {
        }

        Transformation TransformationMatrices::oacR0()
        {
            Transformation transformation;
            return transformation;
        }

        Transformation TransformationMatrices::oacR90()
        {
            Transformation rotation;
            rotation.setTransformationMatrix(0, -1, 1, 0);
            rotation.setAngle(90);

            Transformation reflection;
            reflection.setTransformationMatrix(1, 0, 0, 1);

            Transformation transformation = rotation * reflection;
            return transformation;
        }

        Transformation TransformationMatrices::oacR180()
        {
            Transformation rotation;
            rotation.setTransformationMatrix(-1, 0, 0, -1);
            rotation.setAngle(180);

            Transformation reflection;
            reflection.setTransformationMatrix(1, 0, 0, 1);

            Transformation transformation = rotation * reflection;
            return transformation;
        }

        Transformation TransformationMatrices::oacR270()
        {
            Transformation rotation;
            rotation.setTransformationMatrix(0, 1, -1, 0);
            rotation.setAngle(270);

            Transformation reflection;
            reflection.setTransformationMatrix(1, 0, 0, 1);

            Transformation transformation = rotation * reflection;
            return transformation;
        }

        Transformation TransformationMatrices::oacMX()
        {
            Transformation rotation;
            rotation.setTransformationMatrix(1,0,0,1);
            Transformation reflection;

            reflection.setTransformationMatrix(1, 0, 0, -1);
            reflection.setMirrorType(MirrorType::xMirror());

            Transformation transformation = rotation * reflection;
            return transformation;
        }

        Transformation TransformationMatrices::oacMXR90()
        {
            Transformation rotation;
            rotation.setTransformationMatrix(0, -1, 1, 0);
            rotation.setAngle(90);

            Transformation reflection;
            reflection.setTransformationMatrix(1, 0, 0, -1);
            reflection.setMirrorType(MirrorType::xMirror());

            Transformation transformation = rotation * reflection;
            return transformation;
        }

        Transformation TransformationMatrices::oacMY()
        {
            Transformation rotation;
            rotation.setTransformationMatrix(1, 0, 0, 1);

            Transformation reflection;
            reflection.setTransformationMatrix(-1, 0, 0, 1);
            reflection.setMirrorType(MirrorType::yMirror());

            Transformation transformation = rotation * reflection;
            return transformation;
        }

        Transformation TransformationMatrices::oacMYR90()
        {
            Transformation rotation;
            rotation.setTransformationMatrix(0, -1, 1, 0);
            rotation.setAngle(90);

            Transformation reflection;
            reflection.setTransformationMatrix(1, 0, 0, 1);
            reflection.setTransformationMatrix(-1, 0, 0, 1);
            reflection.setMirrorType(MirrorType::yMirror());

            Transformation transformation = rotation * reflection;
            return transformation;
        }
    }
}
