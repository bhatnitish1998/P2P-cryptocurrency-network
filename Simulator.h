#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include "utility_functions.h"
#include "Network.h"

using namespace std;

extern int initial_bitcoin;
extern int initial_number_of_transactions;
extern int mean_transaction_inter_arrival_time;

class Network;
extern Network network;

class Simulator
{
    void create_genesis();
    static void create_initial_transactions();

public:
    Network network;
    void initialize();
    void start();

};
#endif //SIMULATOR_H
