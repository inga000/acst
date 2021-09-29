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
/*
 * LibrarayItem.cpp
 *
 *  Created on: Apr 11, 2016
 *      Author: ga76fuw
 */

#include "StructRec/incl/Library/LibraryItem/LibraryItem.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	const int LibraryItem::MAX_PERSISTENCE_ = -1;
	const int LibraryItem::PERSISTENCE_NOT_INITIALIZED_ =-2;
	const int LibraryItem::HIERARCHYLEVEL_NOT_INITIALIZED_ = -1;

	LibraryItem::LibraryItem() :
			hierarchyLevel_(HIERARCHYLEVEL_NOT_INITIALIZED_),
			persistence_(PERSISTENCE_NOT_INITIALIZED_),
			idCounter_(1)
	{
	}

	LibraryItem::LibraryItem(const LibraryItem& other):
		name_(other.name_),
		hierarchyLevel_(other.hierarchyLevel_),
		persistence_(other.persistence_),
		imageFilePath_(other.imageFilePath_),
		filePath_(other.filePath_),
		idCounter_(other.idCounter_)
	{

	}

	LibraryItem::~LibraryItem()
	{
	}

	void LibraryItem::setName(const StructureName& name)
	{
		name_ = name;
	}

	void LibraryItem::setHierarchyLevel(int level)
	{
		hierarchyLevel_ = level;
	}

	void LibraryItem::setPersistence(int persistence)
	{
		persistence_ = persistence;
	}

	int LibraryItem::getHierarchyLevel() const
	{
		assert(hierarchyLevel_ > HIERARCHYLEVEL_NOT_INITIALIZED_);
		return hierarchyLevel_;
	}

	int LibraryItem::getPersistence() const
	{
		return persistence_;
	}

	int LibraryItem::getMaxPersistence() const
	{
		return MAX_PERSISTENCE_;
	}

	StructureName  LibraryItem::getName() const
	{
		assert(name_.isInitialized());
		return name_;
	}

	int & LibraryItem::getIdCounter()
	{
		return idCounter_;
	}

	void LibraryItem::setImageFilePath(const std::string& path)
	{
		imageFilePath_= path;
	}

	const std::string& LibraryItem::getImageFilePath() const
	{
		return imageFilePath_;
	}

	void LibraryItem::setFilePath(const std::string& path)
	{
		filePath_= path;
	}

	const std::string& LibraryItem::getFilePath() const
	{
		assert(!filePath_.empty());
		return filePath_;
	}

	bool LibraryItem::hasMaxPersistence() const
	{
		return persistence_ == MAX_PERSISTENCE_;
	}

	bool LibraryItem::hasPersistence() const
	{
		return persistence_ != PERSISTENCE_NOT_INITIALIZED_;
	}
}

