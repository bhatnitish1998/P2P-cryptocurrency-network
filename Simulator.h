#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include "Event.h"
#include "utility_functions.h"
#include "Network.h"

using namespace std;

extern int initial_bitcoin;
extern int initial_number_of_transactions;
extern int transaction_amount_min;
extern int transaction_amount_max;
extern long long simulation_time;
extern int number_of_nodes;
extern int mean_transaction_inter_arrival_time;

class Network;
extern Network network;



extern EQ event_queue;

class Simulator
{
    void create_genesis();
    void create_initial_transactions();

public:
    Network network;
    void initialize();
    void start();

};
#endif //SIMULATOR_H
