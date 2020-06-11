#ifndef UE_H
#define UE_H

#include "point.h"
#include "parameters.h"
#include "dataset_generator.h"

class UE
{
	public:
		int id;
		point position;
		UE(int _id, point _position);
		void moveToNextTimeFrame();
		double bitRate;
		double totalSignal = 0;
	
};

UE :: UE(int _id, point _position)
{
	id = _id;
	position = _position;
	bitRate = 100*1024*8;   // bits/sec
	totalSignal = 0;
}

void UE :: moveToNextTimeFrame()
{
	position = datasetGenerator :: movePointOnRoad(position);
}


#endif