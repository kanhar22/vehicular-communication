#ifndef MBS_H
#define MBS_H

#include "parameters.h"
#include "point.h"
#include <vector>

class FAP;

class MBS
{
	public:
		int id;
		point position;
		MBS(int _id, point _position);
		std :: vector<FAP*> associatedFAP;
		std :: vector<double> powerLevelAtSubChannel;
		int usedSubChannels;

		friend class FAP;
};

MBS::MBS(int _id, point _position)
{
	id = _id;
	position = _position;
	usedSubChannels = 0;
	powerLevelAtSubChannel.resize(SUB_CHANNELS,100);
}

#endif