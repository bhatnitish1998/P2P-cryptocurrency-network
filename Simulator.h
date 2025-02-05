#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include "Network.h"
#include "utility_functions.h"

using namespace std;

extern int initial_bitcoin;
extern int initial_number_of_transactions;
extern int mean_transaction_inter_arrival_time;

class Network;
extern Network network;

class Simulator
{
    // Initializes each nodes blockchain with genesis block containing starting balances of all nodes
    void create_genesis();

    // Populates event queue with CREATE_TRANSACTION events
    static void create_initial_transactions();

public:
    Network network;
    // Assign initial balance, create genesis block, create initial transactions
    void initialize();
    // Start processing event queue
    void start();
};
#endif //SIMULATOR_H
