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


#include "AutomaticSizing/incl/ConstraintProgram/AutomaticSizing.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Log/incl/LogMacros.h"
#include <sstream>
#include <gecode/search.hh>



namespace AutomaticSizing {

	const int AutomaticSizing::NOT_INITIALIZED_ = -100;

	AutomaticSizing::AutomaticSizing() :
		partitioningResult_(NULL) ,
		circuitInformation_(NULL),
		graph_(NULL),
		structRecResult_(NULL),
		runtime_(NOT_INITIALIZED_)
	{
	}

	AutomaticSizing::~AutomaticSizing()
	{
		delete graph_;
	}

	void AutomaticSizing::setPartitioningResult(
			const Partitioning::Result& result)
	{
		partitioningResult_ = & result;
	}

	const Result & AutomaticSizing::compute()
	{
		graph_ = &computeGraph();
		const Result & result = computeSearch();
		return result;
	}


	const Result & AutomaticSizing::computeSearch()
	{
		initializeSearchSpace();
		const Result & result = computeGecodeSolution();
		return result;
	}

	Graph& AutomaticSizing::computeGraph()
	{
		Graph * graph = new Graph;
		graph->setCircuitParameter(getCircuitInformation().getCircuitParameter());
		std::vector<Partitioning::Component*> components = getPartitioningResult().getAllComponents();
		for(std::vector<Partitioning::Component*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			Partitioning::Component * comp = *it;
			graph->addEdges(*comp);
		}
		graph->equalizeParallelEdges();

		return * graph;
	}


	Graph& AutomaticSizing::getGraph()
	{
		assert(graph_ != NULL);
		return *graph_;
	}


	void AutomaticSizing::setCircuitInformation(CircuitInformation & circuitInformation)
	{
		circuitInformation_ = & circuitInformation;
	}


	CircuitInformation& AutomaticSizing::getCircuitInformation()
	{
		assert(circuitInformation_ != NULL);
		return *circuitInformation_;
	}



	void AutomaticSizing::initializeSearchSpace()
	{
		searchSpace_.setResult(getPartitioningResult());
		searchSpace_.setCircuitInformation(getCircuitInformation());
		searchSpace_.setGraph(getGraph());
		searchSpace_.setStructureRecognitionResult(getStructureRecognitionResult());
		searchSpace_.setTransistorModel(getTransistorModel());
		searchSpace_.setDefinition(getDefinition());
		searchSpace_.initialize();
	}

	const Result &  AutomaticSizing::computeGecodeSolution()
	{
		Gecode::FloatVal bestCircuitPerformance = -1;

		Gecode::Search::Stop* stop = Gecode::Search::Stop::time(getRuntime()*1000);
		SimpleSearchSpace * solution = NULL;
		searchSpace_.status();
		logDebug(searchSpace_.toStr());

		Gecode::Search::Cutoff * geometric = Gecode::Search::Cutoff::geometric(20,5);
		Gecode::Search::Cutoff * c1 = Gecode::Search::Cutoff::linear(20);
		Gecode::Search::Cutoff * c2 = Gecode::Search::Cutoff::constant(200);
		Gecode::Search::Cutoff * luby = Gecode::Search::Cutoff::luby(10);
		Gecode::Search::Cutoff * cutoff = Gecode::Search::Cutoff::append(c1,10, c2);
		Gecode::Search::Options opt;
		opt.cutoff = luby;
//		opt.nogoods_limit = 10;
		opt.stop = stop;
		Gecode::RBS<SimpleSearchSpace, Gecode::BAB>  search(&searchSpace_, opt );

		int numSolution =0;
		while(SimpleSearchSpace* space = search.next())
		{
			logDebug(space->toStr());
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
//				 break;
			 }
			else
			{
				delete space;
			 }
		}

		Gecode::Search::Statistics statistics = search.statistics();
		std::ostringstream stat;
		stat << "FailureNodes " << statistics.fail << std::endl << "Number of nodes " << statistics.node << std::endl << "Number of restarts " << statistics.restart << std::endl << "Depth " << statistics.depth << std::endl ;
		logDebug(stat.str());
		logDebug("<<<<<<<<numSolutions " << numSolution);

		assert(solution != NULL);
		const Result & result = solution->computeResult();
		solution->eraseVariables();

		delete solution;
		return result;

	}



	void AutomaticSizing::setStructureRecognitionResult(
			const StructRec::StructureCircuits& structRecResult)
	{
		structRecResult_ = & structRecResult;
	}

	const StructRec::StructureCircuits& AutomaticSizing::getStructureRecognitionResult() const
	{
		assert(structRecResult_ != NULL);
		return * structRecResult_;
	}

	const Partitioning::Result & AutomaticSizing::getPartitioningResult() const
	{
		assert(partitioningResult_ != NULL);
		return * partitioningResult_;
	}

	void AutomaticSizing::setTransistorModel(std::string model)
	{
		transistorModel_ = model;
	}

	std::string AutomaticSizing::getTransistorModel() const
	{
		assert(transistorModel_ == "SHM" || transistorModel_ == "EKV", "Transistor model not supported");
		return transistorModel_;
	}

	void AutomaticSizing::setDefinition(std::string definition)
	{
		definition_ = definition;
	}

	std::string AutomaticSizing::getDefinition() const
	{
		assert(definition_ == "1mum" || definition_ == "0.1mum", "Definition not supported");
		return definition_;
	}

	void AutomaticSizing::setRuntime(int runtime)
	{
		runtime_ = runtime;
	}

	int AutomaticSizing::getRuntime() const
	{
		assert(runtime_ != NOT_INITIALIZED_);
		return runtime_;
	}

}


