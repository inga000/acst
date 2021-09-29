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
#include "StructRec/incl/RuleGeneration/PinConnectionKeyMap.h"

namespace StructRec {

	PinConnectionKeyMap::PinConnectionKeyMap()
	{
	}

	PinConnectionKeyMap::~PinConnectionKeyMap()
	{
	}

	void PinConnectionKeyMap::addPinConnectionKey(
		PinConnectionKey& pinConnectionKey)
	{
		std::map<int,std::vector<PinConnectionKey*> > & childNumberToPinConnectionKeyMap = sortedPinConnectionKeys_[pinConnectionKey.getConnected()];
		std::vector<PinConnectionKey*> & pinConnectionKeys = childNumberToPinConnectionKeyMap[pinConnectionKey.getChildNumber()];
		pinConnectionKeys.push_back(&pinConnectionKey);
	}

	std::map<int,std::vector<PinConnectionKey*> > & PinConnectionKeyMap::findChildNumberToPinConnectionKeysMap(
		bool connected)
	{
		if(hasPinConnectionKeysForThisConnection(connected))
		{
			return sortedPinConnectionKeys_.at(connected);
		}
		else
		{
			std::map<int, std::vector<PinConnectionKey*> > * emptyMap = new std::map<int, std::vector<PinConnectionKey*> >;
			return * emptyMap;
		}
	}

	bool PinConnectionKeyMap::hasPinConnectionMap() const
	{
		return !sortedPinConnectionKeys_.empty();
	}

	bool PinConnectionKeyMap::hasPinConnectionKeysForThisConnection(bool connected) const
	{
		return sortedPinConnectionKeys_.find(connected) != sortedPinConnectionKeys_.end();
	}

	bool PinConnectionKeyMap::PinConnectionsOfChildrenAreEquivalent()
	{
		return PinConnectionsOfChildrenAreEquivalent(true) && PinConnectionsOfChildrenAreEquivalent(false);
	}

	bool PinConnectionKeyMap::PinConnectionsOfChildrenAreEquivalent(bool connected)
	{
		int numberOfEquivalentPins = 0;
		std::map<int, std::vector<PinConnectionKey*> >  & pinsToChildMap = findChildNumberToPinConnectionKeysMap(connected);
		std::vector<PinConnectionKey*> keysChild1 = pinsToChildMap[1];
		std::vector<PinConnectionKey*> keysChild2 = pinsToChildMap[2];

			for(std::vector<PinConnectionKey*>::const_iterator it_keysChild1 = keysChild1.begin(); it_keysChild1 != keysChild1.end(); it_keysChild1++)
			{
				PinConnectionKey* keyChild1 = *it_keysChild1;

				for(std::vector<PinConnectionKey*>::const_iterator it_keysChild2 = keysChild2.begin(); it_keysChild2 != keysChild2.end(); it_keysChild2++)
				{
					PinConnectionKey* keyChild2 = *it_keysChild2;
					if(connected == true)
					{
						if(keyChild1->getPin().getPinType() == keyChild2->getPin().getPinType() && keyChild2->getConnectedPin().getPinType() == keyChild2->getConnectedPin().getPinType())
						{
							numberOfEquivalentPins++;
						}
					}
					else
					{
						if(keyChild1->getPin().getPinType() == keyChild2->getPin().getPinType())
						{
							numberOfEquivalentPins++;
						}
					}

				}
			}
			int numberOfPins = keysChild1.size();
		return keysChild1.size() == keysChild2.size() && numberOfPins == numberOfEquivalentPins;
	}
}


