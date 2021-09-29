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

#ifndef STRUCTREC_INCL_LIBRARY_LIBRARYITEM_H_
#define STRUCTREC_INCL_LIBRARY_LIBRARYITEM_H_

#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include <vector>

namespace StructRec
	{
	class LibraryItem
	{
	public:
		LibraryItem();
		LibraryItem(const LibraryItem& other);
		~LibraryItem();

		void setName(const StructureName& name);
		void setHierarchyLevel(int level);
        void setPersistence(int persistence);

		StructureName getName() const;
		int getHierarchyLevel() const;
		int getPersistence() const;
		int getMaxPersistence() const;
		bool hasMaxPersistence() const;
		bool hasPersistence() const;

		void setImageFilePath(const std::string& path);
		const std::string& getImageFilePath() const;
		void setFilePath(const std::string& path);
		const std::string& getFilePath() const;

	protected:

		int & getIdCounter();
	private:
		static const int MAX_PERSISTENCE_;
		static const int PERSISTENCE_NOT_INITIALIZED_;
		static const int HIERARCHYLEVEL_NOT_INITIALIZED_;

	private:
		StructureName name_;
		int idCounter_;
		int hierarchyLevel_;
		int persistence_;

		std::string imageFilePath_;
		std::string filePath_;

	};

}

#endif /* STRUCTREC_INCL_LIBRARY_LIBRARYITEM_H_ */
