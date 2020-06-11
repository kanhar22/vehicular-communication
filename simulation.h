#ifndef simulation_H
#define simulation_H

#include "UE.h"
#include "MBS.h"
#include "FAP.h"
#include <vector>
#include "parameters.h"
#include "dataset_generator.h"
#include "scheme.h"

class simulation
{
	public:
		std :: vector<double> SINRTable;
		std :: vector<double> bitRateTable;
		std :: vector<UE> userTable;
		std :: vector<FAP> FAPTable;
		std :: vector<MBS> MBSTable;
		std :: vector<double> FAPEnergy;
		std :: vector<double> MBSEnergy;

		void init();
		void simulateTimeFrame();
		void simulateMovables();
		void simulateFrontHaul();
		void simulateBackHaul();
		void calculateEnergyConsumption();
		void calculateEnergyConsumptionWithChannel();
		void printResults();

};

void simulation :: init()
{

	for(int iterator = 1; iterator <= USER_COUNT; iterator++)
	{
		point position = datasetGenerator :: getRandomPointOnRoad();
		int id = iterator;

		UE user(id, position);
		userTable.push_back(user);

	}

	for(int iterator = 1; iterator <= FAP_COUNT; iterator++)
	{
		point position = datasetGenerator :: getRandomPointOnRoad();
		int id = iterator;

		FAP fap(id, position);
		FAPTable.push_back(fap);
	}

	for(int iterator = 1; iterator <= MBS_COUNT; iterator++)
	{
		point position = datasetGenerator :: getRandomPointOutsideRoad();
		int id = iterator;
		
		MBS mbs(id, position);
		MBSTable.push_back(mbs);
	}
}

void simulation :: simulateTimeFrame()
{
	for(int iterator = 1; iterator <= TIME_FRAMES; iterator++)
	{
		simulateMovables();
		simulateFrontHaul();
		simulateBackHaul();
		calculateEnergyConsumption();
	}
	printResults();
}


void simulation :: simulateMovables()
{
	for(auto fap : FAPTable)
	{
		fap.moveToNextTimeFrame();
	}

	for(auto user : userTable)
	{
		user.moveToNextTimeFrame();
	}
}


void simulation :: simulateFrontHaul()
{
	scheme simulationScheme;
	simulationScheme.spreading(userTable, FAPTable);
}


void simulation :: simulateBackHaul()
{
	scheme simulationScheme;
	simulationScheme.maxRSRP(FAPTable, MBSTable);
}

void simulation :: calculateEnergyConsumption()
{
	double totalE = 0.0;

	for(auto fap : FAPTable)
	{
		totalE = totalE + fap.currentPower*SUB_CHANNELS;
	}

	FAPEnergy.push_back(totalE);

	totalE = 0;

	for(auto mbs : MBSTable)
	{
		for(auto energy : mbs.powerLevelAtSubChannel)
		{
			totalE = totalE + energy;
		}
	}

	MBSEnergy.push_back(totalE);
}

void simulation :: calculateEnergyConsumptionWithChannel()
{
	double totalE = 0.0;

	for(auto fap : FAPTable)
	{
		for(auto energy : fap.powerLevelAtSubChannel)
		{
			totalE = totalE + energy;
		}
	}

	FAPEnergy.push_back(totalE);

	totalE = 0;

	for(auto mbs : MBSTable)
	{
		for(auto energy : mbs.powerLevelAtSubChannel)
		{
			totalE = totalE + energy;
		}
	}

	MBSEnergy.push_back(totalE);
}

void simulation :: printResults()
{
	double totalE = 0.0;

	for(auto energy : MBSEnergy)
		totalE += energy;

	for(auto energy : FAPEnergy)
		totalE += energy;

	std :: cout << totalE << std :: endl;
	
}
#endif