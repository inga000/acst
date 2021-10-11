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
#include "Core/incl/Circuit/Net/NetId/NetName.h"

#include "Synthesis/incl/SearchSpaceExploration/OpAmpSizing.h"
#include "Synthesis/incl/SearchSpaceExploration/MultiThreading.h"
#include "Synthesis/incl/Library/FunctionalBlockLibrary.h"
#include "Synthesis/incl/Library/Circuit.h"
#include "Synthesis/incl/Results/HSPICEOutputFile.h"

#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitParameter.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "AutomaticSizing/incl/ConstraintProgram/OptimizingSearchSpace.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Graph.h"
#include "AutomaticSizing/incl/ConstraintProgram/SimpleSearchSpace.h"
#include "AutomaticSizing/incl/Results/Result.h"

#include "Partitioning/incl/Results/Result.h"

#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Log/incl/LogMacros.h"


namespace Synthesis
{
	OpAmpSizing::OpAmpSizing() :
			library_(nullptr),
			circuitInformation_(nullptr)
	{
	}

	OpAmpSizing::~OpAmpSizing()
	{
		delete circuitInformation_;
	}

	void OpAmpSizing::setLibrary(const FunctionalBlockLibrary & library)
	{
		library_ = & library;
	}

	void OpAmpSizing::setCircuitInformation(const AutomaticSizing::CircuitInformation & circuitInformation)
	{
		circuitInformation_ = &circuitInformation;
	}

	void OpAmpSizing::setTransistorModel(std::string transistorModel)
	{
		transistorModel_ = transistorModel;
	}

	void OpAmpSizing::setDefinition(std::string definition)
	{
		definition_ = definition;
	}

	void OpAmpSizing::setHSpiceNetlistDirectoryPath(std::string path)
	{
		HSPICENetlistDirectoryPath_ = path;
	}

	const FunctionalBlockLibrary & OpAmpSizing::getLibrary() const
	{
		assert(library_ != nullptr);
		return * library_;
	}

	const AutomaticSizing::CircuitInformation & OpAmpSizing::getCircuitInformation() const
	{
		assert(circuitInformation_ != nullptr);
		return * circuitInformation_;
	}

	void  OpAmpSizing::compute()
	{
		if(getCircuitInformation().getCircuitParameter().isComplementary())
		{
			logDebug("testComplementaryOpAmps");
			testComplOrSymOpAmps();
		}
		else if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
		{
			logDebug("testFullyDifferentialOpAmps");
			testSimpleOrFullyDifferentialOpAmps();
		}
		else
		{

			logDebug("testSimpleOpAmps");
			testSimpleOrFullyDifferentialOpAmps();

			logDebug("testSymmetricalOpAmps");
			testComplOrSymOpAmps();


		}
	}


	void OpAmpSizing::testSimpleOrFullyDifferentialOpAmps()
	{
		std::vector<Circuit*> oneStageOpAmps;

		int caseNumber = 1;
		int index = 1;
		do
		{
			logDebug("caseNumber: " << caseNumber);
			if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
			{

				oneStageOpAmps = getLibrary().createFullyDifferentialOneStageOpAmps(caseNumber, index);

			}
			else
			{
				logDebug("Create one stage simple op amps");
				oneStageOpAmps = getLibrary().createSimpleOneStageOpAmps(caseNumber, index);
			}

			if (oneStageOpAmps.size() == 0)
			{
				break;
			}
			logDebug("oneStageOpAmps size: " << oneStageOpAmps.size());
			logDebug("Test circuits");

			MultiThreading *test = nullptr;
			auto[amountOfThreads, runsPerThread, startOffsetLast] = test->calculate(oneStageOpAmps.size());

			std::vector<std::thread> threads(amountOfThreads -1);
			std::mutex myMutex;

			for (int i = 0; i < amountOfThreads - 1; i++)
			{
				int startOffset = i * runsPerThread;
				int maxCount = static_cast<unsigned>(startOffset + runsPerThread) > oneStageOpAmps.size() ? oneStageOpAmps.size() : startOffset + runsPerThread;
				threads[i] = std::thread([this, oneStageOpAmps, startOffset, maxCount, &myMutex]() {this->testSimpleOrFullyDifferentialOpAmpsLoop(oneStageOpAmps, startOffset, maxCount, std::ref(myMutex));
				});
			}

			testSimpleOrFullyDifferentialOpAmpsLoop(oneStageOpAmps, startOffsetLast, oneStageOpAmps.size(), std::ref(myMutex));

			for (int i = 0; i < amountOfThreads - 1; i++)
			{
				threads[i].join();
			}

			caseNumber++;

		} while (!oneStageOpAmps.empty());
	}


	void OpAmpSizing::testSimpleOrFullyDifferentialOpAmpsLoop(const std::vector<Circuit*>& oneStageOpAmps, int startOffset, int maxCount, std::mutex &myMutex)
	{
		for(int i = startOffset; i < maxCount; i++)
		{
			Circuit* oneStageOpAmpsCircuit = oneStageOpAmps[i];
			Circuit * passedOneStageOpAmp = & testCircuit(*oneStageOpAmpsCircuit);
			if(passedOneStageOpAmp != nullptr)
			{
				if(passedOneStageOpAmp->fulfillsFirstStageSpecifications())
				{
					if(passedOneStageOpAmp->fulfillsAllSpecifications())
					{
						writeHSpiceFile(*passedOneStageOpAmp);
					}
					testSimpleOrFullyDifferentialTwoStageOpAmp(*passedOneStageOpAmp, std::ref(myMutex));
				}
				delete passedOneStageOpAmp;
			}
		}
	}


	void OpAmpSizing::testSimpleOrFullyDifferentialTwoStageOpAmp( const Circuit & OneStageOpAmp, std::mutex &myMutex)
	{
		std::vector<Circuit*> twoStageOpAmps;
		if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
		{
			logDebug("Create fully differential two stage  op amps for the one stage op amp " << OneStageOpAmp);
			twoStageOpAmps = getLibrary().createFullyDifferentialTwoStageOpAmps(OneStageOpAmp, std::ref(myMutex));
		}
		else
		{
			logDebug("Create simple two stage  op amps for the one stage op amp " << OneStageOpAmp);
			twoStageOpAmps = getLibrary().createSimpleTwoStageOpAmps(OneStageOpAmp, std::ref(myMutex));
		}

		logDebug("twoStageOpAmps size: " << twoStageOpAmps.size()); // neu

		logDebug("Test circuits");
		for(auto & twoStageOpAmp : twoStageOpAmps)
		{
			std::ostringstream oneStageOpAmpId;
			oneStageOpAmpId << OneStageOpAmp.getFlatCircuit().getCircuitIdentifier().getId();
			Circuit * passedTwoStageOpAmp = &testCircuit(*twoStageOpAmp);
			if(passedTwoStageOpAmp != nullptr)
			{
				if(passedTwoStageOpAmp->fulfillsAllSpecifications())
				{
					writeHSpiceFile(*passedTwoStageOpAmp, oneStageOpAmpId.str());
				}
				delete passedTwoStageOpAmp;
			}
		}
	 }



	void OpAmpSizing::testComplOrSymOpAmps()
	{
		std::vector<Circuit*> oneStageOpAmps;

		int caseNumber = 1;
		int index = 1;
		do
		{
			logDebug("caseNumber: " << caseNumber);
			if (getCircuitInformation().getCircuitParameter().isComplementary())
			{
				oneStageOpAmps = getLibrary().createComplementaryOpAmps(caseNumber, index);
			}
			else
			{
				oneStageOpAmps = getLibrary().createSymmetricalOpAmps(caseNumber, index);
			}


			if (oneStageOpAmps.size() == 0)
			{
				break;
			}
			logDebug("oneStageOpAmps size: " << oneStageOpAmps.size());
			logDebug("Test circuits");

			MultiThreading *test = nullptr;
			auto[amountOfThreads, runsPerThread, startOffsetLast] = test->calculate(oneStageOpAmps.size());

			std::vector<std::thread> threads(amountOfThreads -1);

			for (int i = 0; i < amountOfThreads - 1; i++)
			{
				int startOffset = i * runsPerThread;
				int maxCount = static_cast<unsigned>(startOffset + runsPerThread) > oneStageOpAmps.size() ? oneStageOpAmps.size() : startOffset + runsPerThread;
				threads[i] = std::thread([this, oneStageOpAmps, startOffset, maxCount]() {this->testComplOrSymOpAmpsLoop(oneStageOpAmps, startOffset, maxCount);
				});
			}

			testComplOrSymOpAmpsLoop(oneStageOpAmps, startOffsetLast, oneStageOpAmps.size());

			for (int i = 0; i < amountOfThreads - 1; i++)
			{
				threads[i].join();
			}

			caseNumber++;

		} while (!oneStageOpAmps.empty());
	}


	void OpAmpSizing::testComplOrSymOpAmpsLoop(const std::vector<Circuit*>& oneStageOpAmps, int startOffset, int maxCount)
	{

		for(int i = startOffset; i < maxCount; i++)
		{
			Circuit* oneStageOpAmpsCircuit = oneStageOpAmps[i];
			Circuit * passedCircuit = & testCircuit(*oneStageOpAmpsCircuit);
			if(passedCircuit != nullptr)
			{
				if(passedCircuit->fulfillsAllSpecifications())
				{
					writeHSpiceFile(*passedCircuit);
				}
				delete passedCircuit;
			}
		}
	}



	Circuit & OpAmpSizing::testCircuit(Circuit & circuit)
	{
		Circuit * passedCircuit = nullptr;
		circuit.setCircuitInformation(getCircuitInformation());

		passedCircuit = & runStrictSearch(circuit);

		if(passedCircuit == nullptr
				&& !getCircuitInformation().getCircuitParameter().isComplementary()
				&& !circuit.getPartitioningResult().hasSecondStage())
		{
			passedCircuit = & runOptimizingSearch(circuit);
		}

		if(passedCircuit == nullptr)
		{
			delete & circuit;
		}


		return *passedCircuit;
	}

	Circuit & OpAmpSizing::runStrictSearch(Circuit & circuit)
	{
		Circuit * passedCircuit = nullptr;
		AutomaticSizing::SimpleSearchSpace strictSearchSpace;
		strictSearchSpace.setResult(circuit.getPartitioningResult());
		strictSearchSpace.setCircuitInformation(getCircuitInformation());
		strictSearchSpace.setGraph(computeGraph(circuit));
		strictSearchSpace.setStructureRecognitionResult(circuit.getConstStructRecResult());
		strictSearchSpace.setTransistorModel(getTransistorModel());
		strictSearchSpace.setDefinition(getDefinition());
		strictSearchSpace.initialize();


		Gecode::FloatVal bestCircuitPerformance = -1;

		Gecode::Search::Stop* stop = Gecode::Search::Stop::time(60000);
		AutomaticSizing::SimpleSearchSpace * solution = nullptr;
		strictSearchSpace.status();

		Gecode::Search::Cutoff * luby = Gecode::Search::Cutoff::luby(10);
		Gecode::Search::Options opt;
		opt.cutoff = luby;
		opt.stop = stop;
		Gecode::RBS<AutomaticSizing::SimpleSearchSpace, Gecode::BAB>  search(&strictSearchSpace, opt );

		int numSolution =0;
		while(AutomaticSizing::SimpleSearchSpace* space = search.next())
		{
			numSolution ++;
			Gecode::FloatVal circuitPerformance = space->getBestCircuitPerformance();
			 if(circuitPerformance > bestCircuitPerformance)
			 {
				 if(solution != NULL)
				 {
					delete solution;
				  }
				  solution = space;
				 bestCircuitPerformance = circuitPerformance;

			 }
			else
			{
				delete space;
			 }
		}

		Gecode::Search::Statistics statistics = search.statistics();
		std::ostringstream stat;
		stat << "FailureNodes " << statistics.fail << std::endl << "Number of nodes " << statistics.node << std::endl << "Number of restarts " << statistics.restart << std::endl << "Depth " << statistics.depth << std::endl ;


		if(solution != NULL)
		{
			const AutomaticSizing::Result & sizingResult = solution->computeResult();
			solution->eraseVariables();
			circuit.setSizingResult(sizingResult);

			passedCircuit =  &circuit;
		}
		delete solution;
		return *passedCircuit;
	}

	Circuit & OpAmpSizing::runOptimizingSearch(Circuit & circuit)
	{
		Circuit * passedCircuit = nullptr;

		AutomaticSizing::OptimizingSearchSpace searchSpace;
		searchSpace.setResult(circuit.getPartitioningResult());
		searchSpace.setCircuitInformation(getCircuitInformation());
		searchSpace.setGraph(computeGraph(circuit));
		searchSpace.setStructureRecognitionResult(circuit.getConstStructRecResult());
		searchSpace.setTransistorModel(getTransistorModel());
		searchSpace.setDefinition(getDefinition());
		searchSpace.initialize();


		Gecode::Search::Stop* stop = Gecode::Search::Stop::time(60000);
		AutomaticSizing::OptimizingSearchSpace * solution = nullptr;
		searchSpace.status();


		Gecode::Search::Cutoff * luby = Gecode::Search::Cutoff::luby(10);
		Gecode::Search::Options opt;
		opt.cutoff = luby;
		opt.stop = stop;
		Gecode::RBS<AutomaticSizing::OptimizingSearchSpace, Gecode::BAB>  search(&searchSpace, opt );

		int numSolution =0;
		while(AutomaticSizing::OptimizingSearchSpace* space = search.next())
		{
			numSolution ++;
			if(solution != NULL)
			{
				delete solution;
			}
			solution = space;
		}

		Gecode::Search::Statistics statistics = search.statistics();
		std::ostringstream stat;
		stat << "FailureNodes " << statistics.fail << std::endl << "Number of nodes " << statistics.node << std::endl << "Number of restarts " << statistics.restart << std::endl << "Depth " << statistics.depth << std::endl;

		if(solution != NULL)
		{
			const AutomaticSizing::Result & sizingResult = solution->computeResult();
			solution->eraseVariables();
			circuit.setSizingResult(sizingResult);

			if(circuit.fulfillsFirstStageSpecifications())
			{
				passedCircuit =  &circuit;
			}
		}


		delete solution;
		return *passedCircuit;
	}

	AutomaticSizing::Graph &OpAmpSizing::computeGraph(Circuit &circuit)
	{

		AutomaticSizing::Graph * graph = new AutomaticSizing::Graph;
		graph->setCircuitParameter(getCircuitInformation().getCircuitParameter());
		std::vector<Partitioning::Component*> components = circuit.getPartitioningResult().getAllComponents();
		for(std::vector<Partitioning::Component*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			Partitioning::Component * comp = *it;
			graph->addEdges(*comp);
		}
		graph->equalizeParallelEdges();

		return * graph;
	}

	std::string OpAmpSizing::getTransistorModel() const
	{
		assert(transistorModel_ == "SHM" || transistorModel_ == "EKV", "Transistor model not supported");
		return transistorModel_;
	}

	std::string OpAmpSizing::getDefinition() const
	{
		assert(definition_ == "1mum" || definition_ == "0.1mum", "Definition not supported");
		return definition_;
	}

	std::string OpAmpSizing::getHSpiceNetlistDirectoryPath() const
	{
		assert(!HSPICENetlistDirectoryPath_.empty());
		return HSPICENetlistDirectoryPath_;
	}

	void OpAmpSizing::writeHSpiceFile(const Circuit &  circuit, std::string oneStageOpAmpId) const
	{

		std::ostringstream opAmpId;
		std::string opAmpName = circuit.getFlatCircuit().getCircuitIdentifier().getName();
		int opAmpNum = circuit.getFlatCircuit().getCircuitIdentifier().getId();

		if(oneStageOpAmpId == "empty")
		{
			opAmpId << opAmpName << opAmpNum;
		}
		else
		{
			opAmpId << opAmpName << "_" << oneStageOpAmpId << "_" << opAmpNum;
		}


		std::string stringFilePath(getHSpiceNetlistDirectoryPath() + "/" + opAmpId.str() + ".ckt");
		HSPICEOutputFile outputFile;
		outputFile.setPath(stringFilePath);
		outputFile.setId(opAmpId.str());
		outputFile.write(circuit);
	}

}
