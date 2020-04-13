#ifndef FAP_H
#define FAP_H

#include "MBS.h"
#include "point.h"
#include <vector>
#include "UE.h"
#include "parameters.h"
#include "dataset_generator.h"
#include <utility>

class FAP
{
	public:
		int id;
		int usedSubChannels;
		double currentPower;
		double requiredPowerFromMBS;
		std :: pair<int,int> channelRangeAtMBS;
		
		point position;
		MBS* associatedMBS;
		std::vector<UE*> associatedUsers;

		FAP(int _id, point _position);
		void moveToNextTimeFrame();

};

FAP :: FAP(int _id, point _position)
{
	id = id;
	position = _position;
	usedSubChannels = 1;
	currentPower = 100;
	associatedMBS = NULL;
}

void FAP :: moveToNextTimeFrame()
{
	position = datasetGenerator :: movePointOnRoad(position);
}

#endif

