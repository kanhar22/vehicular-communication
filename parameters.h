#ifndef parameters_H
#define parameters_H

#include <random>
#include <cmath>

const double ROAD_WIDTH = 10;  // metere
const double SIMULATION_SCOPE = 100; // metere
const double BANDWIDTH = 2000000; // Herz
const double FEMTO_POWER = 2000000; //Watt
const double SUB_CHANNELS = 50;
const double FEMTO_SUBPOWER = FEMTO_POWER/SUB_CHANNELS;
const double SUB_CHANNEL_BANDWIDTH = BANDWIDTH/SUB_CHANNELS;
const double PI = acos(-1);
const double GAMMA_TH = 17.6; // decibel

const int TIME_FRAMES = 2;
const int FAP_COUNT = 10;
const int MBS_COUNT = 3;
const int USER_COUNT = 50;
const int RANDOM_SEED = 2123;
const int RANDOM_MAX = 1000000000;


std :: mt19937 randomNumber(RANDOM_SEED);


#endif