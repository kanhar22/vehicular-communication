#ifndef scheme_H
#define scheme_H

#include "MBS.h"
#include "UE.h"
#include "FAP.h"
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
#include <iostream>

class scheme{
	public:
		void breathing(std :: vector<FAP> &FAPTable, std :: vector<MBS> &MBSTable);
		void spreading(std :: vector<UE> &userTable, std :: vector<FAP> &FAPTable);
		void maxRSRP(std::vector<FAP> &FAPTable, std::vector<MBS> &MBSTable);
		void maxRSRP1(std::vector<UE> &userTable, std::vector<FAP> &FAPTable);
};

void scheme :: spreading(std :: vector<UE> &userTable, std :: vector<FAP> &FAPTable)
{
	// Find the FAP with lowest utility factor
	// where utility factor represents gain in
	// signal strength of FAP per bitrate.
	// User is associated with lowest UF FAP.
	// Bitrate requirement is incremented for the FAP.

	for(auto &fap : FAPTable)
	{
		fap.associatedUsers.clear();
		fap.bitRate = 0;
		fap.currentPower = 0.2;
		fap.associatedMBS = NULL;
	}

	for(auto fap : FAPTable)
	{
		for(auto& user : userTable)
		{
			user.totalSignal += fap.currentPower / user.position.squaredDistance(fap.position);
		}
	}

	for(auto& user : userTable)
	{
		FAP *fapWithMinUtilityFactor = NULL;
		double minUtilityFactor = 1e9;
		double minPowerGain = 0;

		for(auto& fap : FAPTable)
		{
			double lowerVal = 0;
			double upperVal = FEMTO_SUBPOWER;
			//std :: cout << lowerVal << " " << upperVal << std :: endl;
			for(int iter = 0; iter < 40; iter++)
			{
				//std :: cout << iter << std :: endl;
				double mid = (lowerVal+upperVal)/2.0;
				int usedChannels = 0;

				for(auto tempUser : fap.associatedUsers)
				{
					double tempPower = mid / tempUser . position.squaredDistance(fap.position);
					double fapPower = fap.currentPower / tempUser . position.squaredDistance(fap.position);
					double tempSINR = tempPower / (tempUser . totalSignal - fapPower);

					usedChannels += ceil(tempUser . bitRate / (SUB_CHANNEL_BANDWIDTH * log(1+tempSINR)));
				}

				int remainingChannels = SUB_CHANNELS - usedChannels;
				//std :: cout << remainingChannels << std :: endl;
				double userRecieveingPower = mid / user.position.squaredDistance(fap.position);
				double fapPower = fap.currentPower / user . position.squaredDistance(fap.position);
				double userSINR = userRecieveingPower / (user.totalSignal - fapPower);
				double totalBitRate = remainingChannels * SUB_CHANNEL_BANDWIDTH * log(1+userSINR);
				//std :: cout << user.totalSignal<< " " << userRecieveingPower << " " << fapPower << " " <<  userSINR << std :: endl;
				if(totalBitRate >= user.bitRate)
				{
					upperVal = mid;
				}
				else
				{
					lowerVal = mid;
				}
			}
			//std :: cout << (lowerVal - fap.currentPower) << std :: endl;
			//std :: cout << fap.position.squaredDistance(user.position) << std :: endl;
			if(fabs(lowerVal-FEMTO_SUBPOWER)<1e-4) 
				continue;

			double utilityFactor = (lowerVal - fap.currentPower)/user.bitRate;
			if(utilityFactor < minUtilityFactor)
			{
				minUtilityFactor = utilityFactor;
				fapWithMinUtilityFactor = &fap;
				minPowerGain = lowerVal - fap.currentPower;
			}
			
		}

		//std :: cout << fapWithMinUtilityFactor->id << std :: endl;
		if(fapWithMinUtilityFactor != NULL)
		{
			fapWithMinUtilityFactor->associatedUsers.push_back(user);
			fapWithMinUtilityFactor->currentPower += minPowerGain;
			fapWithMinUtilityFactor->bitRate += user.bitRate;

			for(auto& iter : userTable)
			{
				iter.totalSignal += minPowerGain / iter.position.squaredDistance(fapWithMinUtilityFactor->position);
			}
		}
	}
}


void scheme :: breathing(std :: vector<FAP> &FAPTable, std :: vector<MBS> &MBSTable)
{

	// Calculate the closest FAP for all users and save the User and FAP association 

	for(auto& mbs : MBSTable)
	{
		mbs.usedSubChannels = 0;
		mbs.powerLevelAtSubChannel.clear();
		mbs.powerLevelAtSubChannel.resize(SUB_CHANNELS,0.00001);
	}
	double tot = 0;
	for(auto& fap : FAPTable)
	{
		tot += fap.bitRate;
		double minDist = 1e18;
		MBS *minDistMBS;
		for(auto& mbs : MBSTable)
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
		for(auto &fap : mbs.associatedFAP)
		{
			blockedFAP.insert(fap);
		}

		for(int iteratori = 0; iteratori < 2; iteratori++)
		{
			int subchannelUsed = 0;
			for(auto fap : servedFAP)
			{
				double totalSignal = 0;
				int maxPowerIndex = fap->channelRangeAtMBS.first;
				for(auto mbsTemp : MBSTable)
				{
					totalSignal += mbsTemp.powerLevelAtSubChannel[maxPowerIndex] / mbsTemp.position.squaredDistance(fap -> position);
				}
				MBS* curMBS = fap->associatedMBS;
				double signalFromAssociation = curMBS -> powerLevelAtSubChannel[maxPowerIndex] / curMBS -> position.squaredDistance(fap->position);
				double SINR = signalFromAssociation / (totalSignal - signalFromAssociation);

				fap->requiredPowerFromMBS = 1;

				if(SINR > GAMMA_TH)
				{
					fap->requiredPowerFromMBS = (curMBS -> powerLevelAtSubChannel[maxPowerIndex] * GAMMA_TH) / SINR;
				}
				int requiredSubChannel = ceil (fap -> bitRate / (BANDWIDTH * log(1+SINR)) );
				subchannelUsed +=   requiredSubChannel;
				fap -> channelRangeAtMBS = {0,requiredSubChannel};
				if(subchannelUsed  > SUB_CHANNELS)
				{
					subchannelUsed -= requiredSubChannel;
					servedFAP.erase(fap);
				}

			}
			for(auto fap : blockedFAP)
			{
				double totalSignal = 0;
				for(auto mbsTemp : MBSTable)	
				{
					totalSignal += mbsTemp.powerLevelAtSubChannel[0] / mbsTemp.position.squaredDistance(fap -> position);
				}
				MBS* curMBS = fap -> associatedMBS;
				double signalFromAssociation = curMBS -> powerLevelAtSubChannel[0] / curMBS -> position.squaredDistance(fap -> position);
				double SINR = signalFromAssociation / (totalSignal - signalFromAssociation);
				fap -> requiredPowerFromMBS = 1;
				if(SINR > GAMMA_TH)
				{
					fap -> requiredPowerFromMBS = (curMBS -> powerLevelAtSubChannel[0] * GAMMA_TH) / SINR;
				}

				int requiredSubChannel =   ceil (fap -> bitRate / (BANDWIDTH * log2(1+SINR)) );

				if(subchannelUsed + requiredSubChannel < SUB_CHANNELS)
				{
					subchannelUsed += requiredSubChannel;
					fap -> channelRangeAtMBS = {0,requiredSubChannel};
					blockedFAP.erase(fap);
					servedFAP.insert(fap);
				}
			}
			assert(subchannelUsed<=50);

			std :: vector<FAP*> inhaleSort;
			for(auto fap : servedFAP)
			{
				inhaleSort.push_back(fap);
			}

			auto compareFAP = [] (const FAP* fapA, const FAP* fapB)
			{
				return fapA->requiredPowerFromMBS > fapB->requiredPowerFromMBS;
			};

			std :: sort(inhaleSort.begin(), inhaleSort.end(), compareFAP);

			int curIndex = 0;
		
			for(auto fap : inhaleSort)
			{
				int reqChannels = fap->channelRangeAtMBS.second - fap->channelRangeAtMBS.first;
				for(int iterator = curIndex; iterator < curIndex + reqChannels; iterator++)
				{
					mbs.powerLevelAtSubChannel[iterator] = fap->requiredPowerFromMBS;
				}
				fap->channelRangeAtMBS = {curIndex, curIndex + reqChannels};
				curIndex += reqChannels;
			}

		}

	}

}

void scheme :: maxRSRP(std::vector<FAP> &FAPTable, std::vector<MBS> &MBSTable)
{

	for(auto &mbs : MBSTable)
	{
		mbs.associatedFAP.clear();
		mbs.powerLevelAtSubChannel.clear();
		mbs.powerLevelAtSubChannel.resize(SUB_CHANNELS, 0.1);
	}

	for(auto &fap : FAPTable)
	{
		double minDist = 1e18;
		MBS* minDistMBS = NULL;
		for(auto &mbs : MBSTable)
		{
			double dist = fap.position.squaredDistance(mbs.position);
			if(dist < minDist)
			{
				minDist = dist;
				minDistMBS = &mbs;
			}
		}
		minDistMBS->associatedFAP.push_back(&fap);
	}
	

	for(auto& mbs : MBSTable)
	{
		int index = 0;
		for(auto& fap : mbs.associatedFAP)
		{
			double totalSignal = 0;
			for(auto &mbsTemp : MBSTable)
			{
				totalSignal += mbsTemp.powerLevelAtSubChannel[index]/mbsTemp.position.squaredDistance(fap->position);
			}
			double recv = PMAX/mbs.position.squaredDistance(fap->position);
			double avl = mbs.powerLevelAtSubChannel[index]/mbs.position.squaredDistance(fap->position);

			double SINR = recv/(totalSignal - avl);
			int reqChannels = ceil(fap->bitRate/(BANDWIDTH*log(1+SINR)));
			for(int iter = index; iter < index+reqChannels && iter < SUB_CHANNELS; iter++)
			{
				mbs.powerLevelAtSubChannel[iter] = PMAX;
			}
			index += reqChannels;

			if(index >= SUB_CHANNELS)
				break;
		}
	}
}


void scheme :: maxRSRP1( std::vector<UE> &userTable, std::vector<FAP> &FAPTable)
{

	for(auto &fap : FAPTable)
	{
		fap.bitRate = 0;
		fap.associatedUsers.clear();
		fap.powerLevelAtSubChannel.clear();
		fap.powerLevelAtSubChannel.resize(SUB_CHANNELS, 0.1);
	}


	for(auto &user : userTable)
	{
		double minDist = 1e18;
		FAP* minDistFAP = NULL;
		for(auto &fap : FAPTable)
		{
			double dist = fap.position.squaredDistance(user.position);
			if(dist < minDist)
			{
				minDist = dist;
				minDistFAP = &fap;
			}
		}
		minDistFAP->associatedUsers.push_back(user);
	}

	for(auto& fap : FAPTable)
	{
		int index = 0;
		for(auto& user : fap.associatedUsers)
		{
			double totalSignal = 0;
			for(auto &fapTemp : FAPTable)
			{
				totalSignal += fapTemp.powerLevelAtSubChannel[index]/fapTemp.position.squaredDistance(user.position);
			}
			double recv = PMAX/fap.position.squaredDistance(user.position);
			double avl = fap.powerLevelAtSubChannel[index]/fap.position.squaredDistance(user.position);
			double SINR = recv/(totalSignal - avl);
			int reqChannels = ceil(user.bitRate/(BANDWIDTH*log(1+SINR)));
			for(int iter = index; iter < (index+reqChannels) && iter < SUB_CHANNELS; iter++)
			{
				fap.powerLevelAtSubChannel[iter] = PMAX;
			}
			index += reqChannels;
			fap.bitRate += user.bitRate;
			if(index >= SUB_CHANNELS)
				break;
		}
	}
}


#endif