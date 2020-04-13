#ifndef dataset_generator_H
#define dataset_generator_H

#include "parameters.h"
#include "point.h"

class datasetGenerator
{
	public:
		static int  getIntegerInRange(int a, int b);
		static double getRealNumberInRange(double a, double b);
		static point getRandomPointOnRoad();
		static point getRandomPointOutsideRoad();
		static point movePointOnRoad(point currentPosition);
};


double datasetGenerator :: getRealNumberInRange(double a, double b)
{
	double diff = (b-a);
	double relative = (diff/RANDOM_MAX)*(getIntegerInRange(0,RANDOM_MAX));
	return diff + relative;
}

int datasetGenerator :: getIntegerInRange(int a, int b) 
{
	int diff = b-a;
	return a + (randomNumber()%diff);
}

point datasetGenerator :: getRandomPointOnRoad()
{
	double x = getRealNumberInRange(-SIMULATION_SCOPE/2, SIMULATION_SCOPE/2);
	double y = getRealNumberInRange(-ROAD_WIDTH/2, ROAD_WIDTH/2);
	return point(x,y);
}

point datasetGenerator :: getRandomPointOutsideRoad()
{
	while(1)
	{
		double x = getRealNumberInRange(-SIMULATION_SCOPE/2, SIMULATION_SCOPE/2);
		double y = getRealNumberInRange(-SIMULATION_SCOPE/2, SIMULATION_SCOPE/2);
		if(y<-ROAD_WIDTH/2 || y > ROAD_WIDTH/2)
		{
			return point(x,y);
		}
	}
	
}

point datasetGenerator :: movePointOnRoad(point currentPosition)
{
	double newx = currentPosition.x + getRealNumberInRange(0,1);
	double newy = currentPosition.y + getRealNumberInRange(
									std :: max(-0.5,-ROAD_WIDTH/2-currentPosition.y),
									std :: min( 0.5, ROAD_WIDTH/2-currentPosition.y));

	if(newx > SIMULATION_SCOPE/2)
	{
		newx = -SIMULATION_SCOPE/2;
		newy = getRealNumberInRange(-ROAD_WIDTH/2, ROAD_WIDTH/2);
	}
	return point(newx, newy);
}

#endif