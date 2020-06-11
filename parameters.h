#ifndef parameters_H
#define parameters_H

#include <random>
#include <cmath>

const double ROAD_WIDTH = 20;  // metere
const double SIMULATION_SCOPE = 10000; // metere
const double BANDWIDTH = 1000000; // Herz
const double FEMTO_POWER = 1; //Watt
const double SUB_CHANNELS = 50;
const double FEMTO_SUBPOWER = 1; //Watt
const double SUB_CHANNEL_BANDWIDTH = BANDWIDTH;
const double PI = acos(-1);
const double GAMMA_TH = 17.6; // decibel
const double PMAX = 1; //watt

const int TIME_FRAMES = 20;
const int FAP_COUNT = 200;
const int MBS_COUNT = 30;
const int USER_COUNT = 500;
const int RANDOM_SEED = 2124;
const int RANDOM_MAX = 1000000000;


std :: mt19937 randomNumber(RANDOM_SEED);


#endif