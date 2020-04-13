#ifndef scheme_H
#define scheme_H

#include "MBS.h"
#include "UE.h"
#include "FAP.h"
#include <vector>
#include <set>
#include <algorithm>

class scheme{
	public:
		void breathing(std :: vector<FAP> &FAPTable, std :: vector<MBS> &MBSTable);
		void spreading(std :: vector<UE> &userTable, std :: vector<FAP> &FAPTable);
		void maxRSRP();
};

void scheme :: spreading(std :: vector<UE> &userTable, std :: vector<FAP> &FAPTable)
{
	for(auto user : userTable)
	{
		FAP *fapWithMinUtilityFactor;
		double minUtilityFactor = 0;
		double minPowerGain = 0;

		for(auto fap : FAPTable)
		{
			double lowerVal = fap.currentPower;
			double upperVal = FEMTO_SUBPOWER;
			
			for(int iter = 0; iter = 40; iter++)
			{
				double mid = (lowerVal+upperVal)/2.0;
				int usedChannels = 0;

				for(auto tempUser : fap.associatedUsers)
				{
					double tempPower = mid / tempUser -> position.squaredDistance(fap.position);
					double tempSINR = tempPower / (tempUser -> totalSignal - tempPower);

					usedChannels += ceil(tempUser -> bitRate / (SUB_CHANNEL_BANDWIDTH * log(1+tempSINR)));
				}

				int remainingChannels = SUB_CHANNELS - usedChannels;

				double userRecieveingPower = mid / user.position.squaredDistance(fap.position);
				double userSINR = userRecieveingPower / (user.totalSignal - userRecieveingPower);
				double totalBitRate = remainingChannels * SUB_CHANNEL_BANDWIDTH * log(1+userSINR);

				if(totalBitRate >= user.bitRate)
				{
					upperVal = mid;
				}
				else
				{
					lowerVal = mid;
				}
			}

			double utilityFactor = (lowerVal - fap.currentPower)/user.bitRate;
			if(utilityFactor < minUtilityFactor)
			{
				minUtilityFactor = utilityFactor;
				fapWithMinUtilityFactor = &fap;
				minPowerGain = lowerVal - fap.currentPower;
			}
			
		}

		fapWithMinUtilityFactor->associatedUsers.push_back(&user);
		fapWithMinUtilityFactor->currentPower += minPowerGain;

		for(auto iter : userTable)
		{
			iter.totalSignal += minPowerGain / iter.position.squaredDistance(fapWithMinUtilityFactor->position);
		}
	}
}


void scheme :: breathing(std :: vector<FAP> &FAPTable, std :: vector<MBS> &MBSTable)
{
	for(auto& fap : FAPTable)
	{
		double minDist = 1e9;
		MBS *minDistMBS;
		for(auto mbs : MBSTable)
		{
			double dist = mbs.position.squaredDistance(fap.position);
			if(dist < minDist)
			{
				minDist = dist;
				minDistMBS = &mbs;
			}
		}

		minDistMBS->associatedFAP.push_back(&fap);
		fap.associatedMBS = minDistMBS;
	}

	for(auto& mbs : MBSTable)
	{
		std :: set<FAP*> blockedFAP;
		std :: set<FAP*> servedFAP;
		int index = 0;
		int subchannelUsed = 0;

		for(auto fap : servedFAP)
		{
			double totalSignal = 0;
			int maxPowerIndex = fap->channelRangeAtMBS.first;
			for(auto mbs : MBSTable)
			{
				totalSignal += mbs.powerLevelAtSubChannel[maxPowerIndex] / mbs.position.squaredDistance(fap -> position);
			}
			MBS* curMBS = fap->associatedMBS;
			double signalFromAssociation = curMBS -> powerLevelAtSubChannel[maxPowerIndex] / curMBS -> position.squaredDistance(fap->position);
			double SINR = signalFromAssociation / (totalSignal - signalFromAssociation);

			fap->requiredPowerFromMBS = curMBS -> powerLevelAtSubChannel[maxPowerIndex];

			if(SINR > GAMMA_TH)
			{
				fap->requiredPowerFromMBS = (curMBS -> powerLevelAtSubChannel[maxPowerIndex] * GAMMA_TH) / SINR;
			}

			subchannelUsed +=  1 ; // ceil (fap.bitRate / (BANDWIDTH * phi(SINR)) );
		}

		for(auto fap : blockedFAP)
		{

			double totalSignal = 0;
			for(auto mbs : MBSTable)
			{
				totalSignal += mbs.powerLevelAtSubChannel[0] / mbs.position.squaredDistance(fap -> position);
			}
			MBS* curMBS = fap -> associatedMBS;
			double signalFromAssociation = curMBS -> powerLevelAtSubChannel[0] / curMBS -> position.squaredDistance(fap -> position);
			double SINR = signalFromAssociation / (totalSignal - signalFromAssociation);

			fap -> requiredPowerFromMBS = curMBS -> powerLevelAtSubChannel[0];

			if(SINR > GAMMA_TH)
			{
				fap -> requiredPowerFromMBS = (curMBS -> powerLevelAtSubChannel[0] * GAMMA_TH) / SINR;
			}

			int requiredSubChannel =  1 ;// ceil (fap -> bitRate / (BANDWIDTH * phi(SINR)) );

			if(subchannelUsed + requiredSubChannel < SUB_CHANNELS)
			{
				subchannelUsed += requiredSubChannel;
				fap -> channelRangeAtMBS = {0,requiredSubChannel};
				blockedFAP.erase(fap);
				servedFAP.insert(fap);
			}
		}

		std :: vector<FAP*> inhaleSort;
		for(auto fap : servedFAP)
		{
			inhaleSort.push_back(fap);
		}

		std :: sort(inhaleSort.begin(), inhaleSort.end());
		int curIndex = 0;
		for(auto fap : inhaleSort)
		{
			int reqChannels = fap->channelRangeAtMBS.second - fap->channelRangeAtMBS.second;
			for(int iterator = curIndex; iterator < curIndex + reqChannels; iterator++)
			{
				mbs.powerLevelAtSubChannel[iterator] = fap->requiredPowerFromMBS;
			}
			fap->channelRangeAtMBS = {curIndex, curIndex + reqChannels};
		}
	}
}

#endif