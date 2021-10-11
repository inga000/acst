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

#include "Synthesis/incl/SearchSpaceExploration/SearchSpaceExploration.h"

#include "AutomaticSizing/incl/ConstraintProgram/SimpleSearchSpace.h"
#include "Synthesis/incl/Library/Library.h"
#include "Synthesis/incl/Library/Circuit.h"
#include "Synthesis/incl/Results/Result.h"

#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitParameter.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "AutomaticSizing/incl/ConstraintProgram/OptimizingSearchSpace.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Graph.h"

#include "Partitioning/incl/Results/Result.h"

#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Log/incl/LogMacros.h"

namespace Synthesis
{
	SearchSpaceExploration::SearchSpaceExploration() :
			result_(nullptr),
			library_(nullptr),
			circuitInformation_(nullptr)
	{
	}

	SearchSpaceExploration::~SearchSpaceExploration()
	{
		delete circuitInformation_;
	}

	void SearchSpaceExploration::setLibrary(const Library & library)
	{
		library_ = & library;
	}

	void SearchSpaceExploration::setCircuitInformation(const AutomaticSizing::CircuitInformation & circuitInformation)
	{
		circuitInformation_ = &circuitInformation;
	}

	void SearchSpaceExploration::setTransistorModel(std::string transistorModel)
	{
		transistorModel_ = transistorModel;
	}

	void SearchSpaceExploration::setDefinition(std::string definition)
	{
		definition_ = definition;
	}
	const Result & SearchSpaceExploration::compute()
	{
		result_ = new Result;
		logDebug("testOneStageOpAmps");
		std::vector<Circuit*> sucessfulFirstStages = testOneStageOpAmps();
		if(getCircuitInformation().getCircuitSpecification().getGain() < 85)
		{
			logDebug("testSymmetricalOpAmps");
			testSymmetricalOpAmps();
		}
		logDebug("testTwoStageOpAmps");
		testTwoStageOpAmps(sucessfulFirstStages);
		return getResult();
	}



	const Library & SearchSpaceExploration::getLibrary() const
	{
		assert(library_ != nullptr);
		return * library_;
	}

	const AutomaticSizing::CircuitInformation & SearchSpaceExploration::getCircuitInformation() const
	{
		assert(circuitInformation_ != nullptr);
		return * circuitInformation_;
	}

	Result & SearchSpaceExploration::getResult()
	{
		assert(result_ != nullptr);
		return * result_;
	}


	std::vector<Circuit*> SearchSpaceExploration::testOneStageOpAmps()
	{
		std::vector<Circuit*> passedFirstStages;

		logDebug("Create one stage op amps");
		std::vector<Circuit*> oneStageOpAmps = getLibrary().createOneStageOpAmps();

		logDebug("Test circuits");
		for(auto & oneStageOpAmp : oneStageOpAmps)
		{
			Core::NetId outputNet = createNetId("outFirstStage1");
			Core::TechType dopingBiasTran =findDopingBiasTran(oneStageOpAmp->getFlatCircuit());
			const AutomaticSizing::CircuitInformation & circuitInformation = createCircuitInformationForCircuit(outputNet, dopingBiasTran);
			oneStageOpAmp->setCircuitInformation(circuitInformation);
			Circuit * passedFirstStage = &testCircuit(*oneStageOpAmp);
			if(passedFirstStage != nullptr)
			{
				passedFirstStages.push_back(passedFirstStage);
			}
		}
		return passedFirstStages;

	}

	void SearchSpaceExploration::testSymmetricalOpAmps()
	{
		std::vector<Circuit*> symOpAmps = getLibrary().createSymmetricalOpAmps();
		for(auto & symOpAmp : symOpAmps)
		{
			Core::NetId outputNet = createNetId("outSecondStage");
			Core::TechType dopingBiasTran =findDopingBiasTran(symOpAmp->getFlatCircuit());
			const AutomaticSizing::CircuitInformation & circuitInformation = createCircuitInformationForCircuit(outputNet, dopingBiasTran);
			symOpAmp->setCircuitInformation(circuitInformation);
			Circuit * passedCircuit = &testCircuit(*symOpAmp);
			if(passedCircuit != nullptr && !passedCircuit->fulfillsAllSpecifications())
			{
				delete passedCircuit;
			}
		}
	}

	void SearchSpaceExploration::testTwoStageOpAmps(std::vector<Circuit*> firstStages)
	{
		std::vector<Circuit*> twoStageOpAmps = getLibrary().createTwoStageOpAmps(firstStages);

		for(auto & twoStageOpAmp : twoStageOpAmps)
		{
			Core::NetId outputNet = createNetId("outSecondStage");
			Core::TechType dopingBiasTran =findDopingBiasTran(twoStageOpAmp->getFlatCircuit());
			const AutomaticSizing::CircuitInformation & circuitInformation = createCircuitInformationForCircuit(outputNet, dopingBiasTran);
			twoStageOpAmp->setCircuitInformation(circuitInformation);
			Circuit * passedCircuit = & testCircuit(*twoStageOpAmp);
			if(passedCircuit != nullptr && !passedCircuit->fulfillsAllSpecifications())
			{
				delete passedCircuit;
			}
		}


	}

	const AutomaticSizing::CircuitInformation &  SearchSpaceExploration::createCircuitInformationForCircuit(Core::NetId outputNet, Core::TechType dopingBiasTran)
	{
		AutomaticSizing::CircuitInformation & newCircuitInformation =  getCircuitInformation().clone();

		AutomaticSizing::Pin * pin = new AutomaticSizing::Pin;
		pin->setNetId(outputNet);
		newCircuitInformation.getCircuitParameter().setOutputPin(*pin);

		float biasCurrent = getCircuitInformation().getCircuitParameter().getCurrentBiasPin().getValue();
		if(dopingBiasTran.isP() && biasCurrent > 0)
		{
			float fac = -1;
			newCircuitInformation.getCircuitParameter().getCurrentBiasPin().setValue(biasCurrent* fac);
		}


		return newCircuitInformation;
	}

	Core::NetId SearchSpaceExploration::createNetId(std::string netName)
	{
		Core::NetName net(netName);
		return net.createRootIdentifier();
	}

	Circuit & SearchSpaceExploration::testCircuit(Circuit & circuit)
	{
		Circuit * passedCircuit = nullptr;

//		runStrictSearch(circuit);
		passedCircuit = & runOptimizingSearch(circuit);

		return *passedCircuit;
	}

	void SearchSpaceExploration::runStrictSearch(Circuit & circuit)
	{
		AutomaticSizing::SimpleSearchSpace strictSearchSpace;
		strictSearchSpace.setResult(circuit.getPartitioningResult());
		strictSearchSpace.setCircuitInformation(circuit.getCircuitInformation());
		strictSearchSpace.setGraph(computeGraph(circuit));
		strictSearchSpace.setStructureRecognitionResult(circuit.getConstStructRecResult());
		strictSearchSpace.setTransistorModel(getTransistorModel());
		strictSearchSpace.setDefinition(getDefinition());
		strictSearchSpace.setUsesHSpiceLibrary(true);
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
				 break;
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
			getResult().add(circuit);
		}
		else
		{
			delete &circuit;
		}
		delete solution;
	}

	Circuit & SearchSpaceExploration::runOptimizingSearch(Circuit & circuit)
	{
		Circuit * passedCircuit = nullptr;

		logDebug("PartitioningResults: ");
		logDebug(circuit.getPartitioningResult());
		AutomaticSizing::OptimizingSearchSpace searchSpace;
		searchSpace.setResult(circuit.getPartitioningResult());
		searchSpace.setCircuitInformation(circuit.getCircuitInformation());
		searchSpace.setGraph(computeGraph(circuit));
		searchSpace.setStructureRecognitionResult(circuit.getConstStructRecResult());
		searchSpace.setTransistorModel(getTransistorModel());
		searchSpace.setDefinition(getDefinition());
		searchSpace.setUsesHSpiceLibrary(true);
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
		stat << "FailureNodes " << statistics.fail << std::endl << "Number of nodes " << statistics.node << std::endl << "Number of restarts " << statistics.restart << std::endl << "Depth " << statistics.depth << std::endl ;

		if(solution != NULL)
		{

			const AutomaticSizing::Result & sizingResult = solution->computeResult();
			solution->eraseVariables();
			circuit.setSizingResult(sizingResult);

			if(circuit.fulfillsFirstStageSpecifications())
			{
				passedCircuit =  &circuit;
			}
			if(circuit.fulfillsAllSpecifications())
			{
				getResult().add(circuit);
			}
		}
		else
		{
			delete &circuit;
		}


		delete solution;
		return *passedCircuit;
	}

	AutomaticSizing::Graph &SearchSpaceExploration::computeGraph(Circuit &circuit)
	{
		AutomaticSizing::Graph * graph = new AutomaticSizing::Graph;
		graph->setCircuitParameter(circuit.getCircuitInformation().getCircuitParameter());
		std::vector<Partitioning::Component*> components = circuit.getPartitioningResult().getAllComponents();
		for(std::vector<Partitioning::Component*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			Partitioning::Component * comp = *it;
			graph->addEdges(*comp);
		}
		graph->equalizeParallelEdges();

		return * graph;
	}

	std::string SearchSpaceExploration::getTransistorModel() const
	{
		assert(transistorModel_ == "SHM" || transistorModel_ == "EKV", "Transistor model not supported");
		return transistorModel_;
	}

	std::string SearchSpaceExploration::getDefinition() const
	{
		assert(definition_ == "1mum" || definition_ == "0.1mum", "Definition not supported");
		return definition_;
	}

	Core::TechType SearchSpaceExploration::findDopingBiasTran(const Core::Circuit & circuit)
	{
		Core::TechType dopingBiasTran = Core::TechType::undefined();
		Core::MosfetDeviceType mosfetDeviceType;
		Core::PinType gatePin = mosfetDeviceType.gate();

		Core::NetId biasNet = getCircuitInformation().getCircuitParameter().getCurrentBiasPin().getNetId();

		for(auto & comp : circuit.getDevices())
		{
			if(comp->getDeviceType() == mosfetDeviceType)
			{
				Core::NetId posBiasNet = comp->findNet(gatePin).getIdentifier();
				if(posBiasNet == biasNet)
				{
					dopingBiasTran = comp->getTechType();
					break;
				}
			}
		}
		assert(!dopingBiasTran.isUndefined(), "There should be a biasTran in the circuit!");
		return dopingBiasTran;
	}

}
